//
// Created by Administrator on 2019/3/26 0026.
//

extern "C" {
#include <stdio.h>
#include <time.h>
#include <jni.h>
#include <android/log.h>
#include <libavutil/imgutils.h>
#include <libavcodec/avcodec.h>
#include <libavformat/avformat.h>
#include <libswscale/swscale.h>
#include "libavutil/log.h"
#include <android/native_window.h>
#include <android/native_window_jni.h>
#define LOGE(format, ...)  __android_log_print(ANDROID_LOG_ERROR, "(>_<)", format, ##__VA_ARGS__)
#define LOGI(format, ...)  __android_log_print(ANDROID_LOG_INFO,  "(^_^)", format, ##__VA_ARGS__)
}

extern "C"
JNIEXPORT jint JNICALL Java_com_example_administrator_ffmpegplayer_MainActivity_multi(
        JNIEnv *env,
        jobject /* this */,
        jint a,
        jint b) {
    return a * b;
}

extern "C"
JNIEXPORT jint JNICALL Java_com_example_administrator_ffmpegplayer_MainActivity_playVideo(
        JNIEnv *env,
        jobject instance,
        jstring path_,
        jobject surface) {

    const char *path = env->GetStringUTFChars(path_, 0);

    int width = 0;
    int height = 0;
    int bufferSize = 0;
    int videoIndex = -1;

    AVPacket *vPacket = NULL;
    AVFrame *vFrame = NULL, *pFrameRGBA = NULL;
    AVCodecContext *vCodecCtx = NULL;
    SwsContext *sws_ctx = NULL;
    AVFormatContext *pFormatCtx = NULL;
    uint8_t *out_buffer = NULL;
    ANativeWindow_Buffer windowBuffer;

    AVCodec *vCodec = NULL;

    //1.初始化所有组件
    av_register_all();
    //分配一个AVFormatContext结构
    pFormatCtx = avformat_alloc_context();
    //2.打开输入文件
    if (avformat_open_input(&pFormatCtx, path, NULL, NULL) != 0) {
        LOGE("Could not open input stream");
        return -1;
    }
    //3.查找文件的流信息
    if (avformat_find_stream_info(pFormatCtx, NULL) < 0) {
        LOGE("Could not find stream information");
        return -1;
    }
    //4.查找视频轨
    for (int index = 0; index < pFormatCtx->nb_streams; index++) {
        if (pFormatCtx->streams[index]->codecpar->codec_type == AVMEDIA_TYPE_VIDEO) {
            videoIndex = index;
            break;
        }
    }
    if (videoIndex == -1) {
        LOGE("Could not find a video stream");
        return -1;;
    }
    //5.查找解码器
    vCodec = avcodec_find_decoder(pFormatCtx->streams[videoIndex]->codecpar->codec_id);
    if (vCodec == NULL) {
        LOGE("could not find codec");
        return -1;;
    }
    //6.配置解码器
    vCodecCtx = avcodec_alloc_context3(vCodec);
    avcodec_parameters_to_context(vCodecCtx, pFormatCtx->streams[videoIndex]->codecpar);
    //7.打开解码器
    if (avcodec_open2(vCodecCtx, vCodec, NULL) < 0) {
        LOGE("Could not open codec");
        return -1;;
    }
    width = vCodecCtx->width;
    height = vCodecCtx->height;
    //分配一个帧指针，指向解码后的原始帧
    vFrame = av_frame_alloc();
    vPacket = (AVPacket *) av_malloc(sizeof(AVPacket));
    pFrameRGBA = av_frame_alloc();
    //绑定输出buffer
    bufferSize = av_image_get_buffer_size(AV_PIX_FMT_RGBA, width, height, 1);
    out_buffer = (uint8_t *) av_malloc(bufferSize * sizeof(uint8_t));
    av_image_fill_arrays(pFrameRGBA->data, pFrameRGBA->linesize, out_buffer, AV_PIX_FMT_RGBA,
                         width, height, 1);
    sws_ctx = sws_getContext(width, height, vCodecCtx->pix_fmt,
                             width, height, AV_PIX_FMT_RGBA, SWS_BICUBIC, NULL, NULL, NULL);
    //取到nativewindow
    ANativeWindow *nativeWindow = ANativeWindow_fromSurface(env, surface);
    //默认为RGB565
    if (ANativeWindow_setBuffersGeometry(nativeWindow, width, height, WINDOW_FORMAT_RGBA_8888) <
        0) {
        LOGE("Could not set buffers geometry");
        ANativeWindow_release(nativeWindow);
        return -1;;
    }
    //读取帧
    while (av_read_frame(pFormatCtx, vPacket) >= 0) {
        if (vPacket->stream_index == videoIndex) {
            //视频解码
            if (avcodec_send_packet(vCodecCtx, vPacket) != 0) {
                return -1;
            }
            while (avcodec_receive_frame(vCodecCtx, vFrame) == 0) {
                //转化格式
                sws_scale(sws_ctx, (const uint8_t *const *) vFrame->data, vFrame->linesize,
                          0,
                          vCodecCtx->height,
                          pFrameRGBA->data, pFrameRGBA->linesize);
                if (ANativeWindow_lock(nativeWindow, &windowBuffer, NULL) < 0) {
                    LOGE("cannot lock window");
                } else {
                    //逐行复制
                    uint8_t *bufferBits = (uint8_t *) windowBuffer.bits;
                    for (int h = 0; h < height; h++) {
                        memcpy(bufferBits + h * windowBuffer.stride * 4,
                               out_buffer + h * pFrameRGBA->linesize[0],
                               pFrameRGBA->linesize[0]);
                    }
                    ANativeWindow_unlockAndPost(nativeWindow);
                }
            }
        }
        av_packet_unref(vPacket);
    }
    //释放内存
    sws_freeContext(sws_ctx);

    av_free(vPacket);
    av_free(pFrameRGBA);
    avcodec_close(vCodecCtx);
    avformat_close_input(&pFormatCtx);
    return 0;
}