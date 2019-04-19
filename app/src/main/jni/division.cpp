#include <jni.h>
#include <string>

extern "C" {
#include <libavcodec/avcodec.h>
#include <libavformat/avformat.h>
#include <libavfilter/avfilter.h>
#include <android/log.h>
#include <android/native_window.h>
#include <android/native_window_jni.h>
#define LOGE(FORMAT, ...) __android_log_print(ANDROID_LOG_ERROR,"ffmpeg",FORMAT,##__VA_ARGS__);
}

extern "C"
JNIEXPORT jint JNICALL Java_com_example_administrator_ffmpegplayer_MainActivity_division(
        JNIEnv *,
        jobject /* this */,
        jint a,
        jint b) {
    return a / b;
}

//获取编码器信息
extern "C"
JNIEXPORT jstring JNICALL Java_com_example_administrator_ffmpegplayer_MainActivity_getAvcodecInfo(
        JNIEnv *env,
        jobject /* this */) {
    char info[40000] = {0};

    av_register_all();

    AVCodec *c_temp = av_codec_next(NULL);

    while (c_temp != NULL) {
        if (c_temp->decode != NULL) {
            sprintf(info, "%sdecode:", info);
        } else {
            sprintf(info, "%sencode:", info);
        }
        switch (c_temp->type) {
            case AVMEDIA_TYPE_VIDEO:
                sprintf(info, "%s(video):", info);
                break;
            case AVMEDIA_TYPE_AUDIO:
                sprintf(info, "%s(audio):", info);
                break;
            default:
                sprintf(info, "%s(other):", info);
                break;
        }
        sprintf(info, "%s[%10s]\n", info, c_temp->name);
        c_temp = c_temp->next;
    }

    return env->NewStringUTF(info);
}

//播放
extern "C"
JNIEXPORT void JNICALL Java_com_example_administrator_ffmpegplayer_MainActivity_play(
        JNIEnv *env,
        jobject,
        jstring path, jobject surface) {
    const char *new_path = env->GetStringUTFChars(path, JNI_FALSE);
    //注册组件
    av_register_all();
    //初始化AVFormatContext的上下文
    AVFormatContext *format_context = avformat_alloc_context();
    //打开输入视频文件
    if (avformat_open_input(&format_context, new_path, NULL, NULL) != 0) {
        LOGE("%s", "打开输入视频文件失败！");
        return;
    };
    //获取视频信息
    if (avformat_find_stream_info(format_context, NULL) < 0) {
        LOGE("%S", "获取视频信息失败");
        return;
    }
    //视频解码，需要找到视频对应的AVStream所在pFormatCtx->streams的索引位置
    int video_stream_index = -1;
    for (int i = 0; i < format_context->nb_streams; i++) {
        if (format_context->streams[i]->codecpar->codec_type == AVMEDIA_TYPE_VIDEO) {
            video_stream_index = i;
            break;
        }
    }
    if (video_stream_index == -1) {
        LOGE("%s", "找不到视频流");
        return;
    }
    // 根据编解码上下文中的编码id查找对应的解码器
    // 只有知道视频的编码方式，才能够根据编码方式去找到解码器
    // avFormatContext->streams[video_stream_idx]->codec已经过时了，这里用codecpar代替
    AVCodecParameters *avCodecParameters = format_context->streams[video_stream_index]->codecpar;
    AVCodec *avCodec = avcodec_find_decoder(avCodecParameters->codec_id);
    if (avCodec == NULL) {
        LOGE("%s", "找不到编码器，或视频已加密");
        return;
    }
    //打开解码器
    AVCodecContext *avCodecContext = avcodec_alloc_context3(avCodec);
    avcodec_parameters_to_context(avCodecContext, avCodecParameters);
    if (avcodec_open2(avCodecContext, avCodec, NULL) < 0) {
        LOGE("%s", "解码器无法打开");
        return;
    }
    //解码并绘制
    // AVPacket用于存储一帧一帧的压缩数据（H264）
    AVPacket *avPacket = av_packet_alloc();
    // AVFrame用于存储解码后的像素数据(YUV)
    AVFrame *yuvAVFrame = av_frame_alloc();
    AVFrame *rgbAVFrame = av_frame_alloc();

    int frame_count = 0;
    int width = avCodecContext->width;
    int height = avCodecContext->height;

    //窗体
    ANativeWindow *aNativeWindow = ANativeWindow_fromSurface(env, surface);
    // 绘制时的缓冲区
    ANativeWindow_Buffer *aNativeWindow_buffer;
    //一帧一帧的读取压缩数据
    while (av_read_frame(format_context, avPacket) >= 0) {
        //只要视频压缩数据（根据流的索引位置判断）
        if (avPacket->stream_index == video_stream_index) {
            //解码一帧视频压缩数据，得到视频像素数据
            if (avcodec_send_packet(avCodecContext, avPacket) == 0) {
                // 一个avPacket可能包含多帧数据，所以需要使用while循环一直读取
                while (avcodec_receive_frame(avCodecContext, yuvAVFrame) == 0) {
                    // lock window
                    // 设置缓冲区的属性：宽高、像素格式（需要与Java层的格式一致）
                    ANativeWindow_setBuffersGeometry(aNativeWindow, width, height,
                                                     WINDOW_FORMAT_RGBA_8888);
                    ANativeWindow_lock(aNativeWindow, aNativeWindow_buffer, NULL);
                    // fix buffer
                    // 初始化缓冲区
                    // 设置属性，像素格式、宽高
                    avpicture_fill(reinterpret_cast<AVPicture *>(rgbAVFrame),
                                   static_cast<const uint8_t *>(aNativeWindow_buffer->bits),
                                   AV_PIX_FMT_RGBA, width, height);
                    // YUV格式的数据转换成RGBA 8888格式的数据
//                    I420ToARGB(yuvAVFrame->data[0], yuvAVFrame->linesize[0],
//                               yuvAVFrame->data[2], yuvAVFrame->linesize[2],
//                               yuvAVFrame->data[1], yuvAVFrame->linesize[1],
//                               yuvAVFrame->data[0], yuvAVFrame->linesize[0],
//                               width, height);
                }
            }
        }
        av_packet_unref(avPacket);
    }

}