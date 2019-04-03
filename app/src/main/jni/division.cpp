#include <jni.h>
#include <string>

extern "C"{
#include <libavcodec/avcodec.h>
#include <libavformat/avformat.h>
#include <libavfilter/avfilter.h>
}
extern "C"
JNIEXPORT jint JNICALL Java_com_example_administrator_ffmpegplayer_MainActivity_division(
        JNIEnv *,
        jobject /* this */,
        jint a,
        jint b){
    return a/b;
}

//获取编码器信息
extern "C"
JNIEXPORT jstring JNICALL Java_com_example_administrator_ffmpegplayer_MainActivity_getAvcodecInfo(
        JNIEnv* env,
        jobject /* this */){
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
JNIEXPORT jstring JNICALL Java_com_example_administrator_ffmpegplayer_MainActivity_play(
        JNIEnv* env,
        jobject,
        jstring path){

       av_register_all();

       AVFormatContext *avFormatContext = avformat_alloc_context();//获取上下文
       avformat_open_input(&avFormatContext, path, NULL, NULL)//解封装
       avformat_find_stream_info(avFormatContext, NULL)

       int video_index = -1;
           for (int i = 0; i < avFormatContext->nb_streams; ++i) {
               if (avFormatContext->streams[i]->codec->codec_type == AVMEDIA_TYPE_VIDEO) {
                   //如果是视频流,标记一哈
                   video_index = i;
               }
           }

       //获取解码器上下文
        AVCodecContext *avCodecContext = avFormatContext->streams[video_index]->codec;
           //获取解码器
           AVCodec *avCodec = avcodec_find_decoder(avCodecContext->codec_id);
           //打开解码器
           if (avcodec_open2(avCodecContext, avCodec, NULL) < 0) {
               LOGE("打开失败")
               return;
           }

       //申请AVPacket
           AVPacket *packet = (AVPacket *) av_malloc(sizeof(AVPacket));
           av_init_packet(packet);
           //申请AVFrame
           AVFrame *frame = av_frame_alloc();//分配一个AVFrame结构体,AVFrame结构体一般用于存储原始数据，指向解码后的原始帧
           AVFrame *rgb_frame = av_frame_alloc();//分配一个AVFrame结构体，指向存放转换成rgb后的帧
       //缓存区
        uint8_t  *out_buffer= (uint8_t *)av_malloc(avpicture_get_size(AV_PIX_FMT_RGBA,
                                                    avCodecContext->width,avCodecContext->height));
       //与缓存区相关联，设置rgb_frame缓存区
       avpicture_fill((AVPicture *)rgb_frame,out_buffer,AV_PIX_FMT_RGBA,avCodecContext->width,avCodecContext->height);

        //取到nativewindow
           ANativeWindow *nativeWindow=ANativeWindow_fromSurface(env,surface);
           if(nativeWindow==0){
               LOGE("nativewindow取到失败")
               return;
           }
           //视频缓冲区
           ANativeWindow_Buffer native_outBuffer;

           while (av_read_frame(avFormatContext, packet) >= 0) {
                   LOGE("解码 %d",packet->stream_index)
                   LOGE("VINDEX %d",video_index)
                   if(packet->stream_index==video_index){
                       LOGE("解码 hhhhh")
                       //如果是视频流
                       //解码
                       avcodec_decode_video2(avCodecContext, frame, &frameCount, packet)
                   }
                   av_free_packet(packet);
               }

       if (frameCount) {
                       LOGE("转换并绘制")
                       //说明有内容
                       //绘制之前配置nativewindow
                       ANativeWindow_setBuffersGeometry(nativeWindow,avCodecContext->width,avCodecContext->height,WINDOW_FORMAT_RGBA_8888);
                       //上锁
                       ANativeWindow_lock(nativeWindow, &native_outBuffer, NULL);
                       //转换为rgb格式
                       sws_scale(swsContext,(const uint8_t *const *)frame->data,frame->linesize,0,
                                 frame->height,rgb_frame->data,
                                 rgb_frame->linesize);
                     //  rgb_frame是有画面数据
                       uint8_t *dst= (uint8_t *) native_outBuffer.bits;
       //            拿到一行有多少个字节 RGBA
                       int destStride=native_outBuffer.stride*4;
                   //像素数据的首地址
                       uint8_t * src=  rgb_frame->data[0];
       //            实际内存一行数量
                       int srcStride = rgb_frame->linesize[0];
                       //int i=0;
                       for (int i = 0; i < avCodecContext->height; ++i) {
       //                memcpy(void *dest, const void *src, size_t n)
                           //将rgb_frame中每一行的数据复制给nativewindow
                           memcpy(dst + i * destStride,  src + i * srcStride, srcStride);
                       }
       //解锁
                       ANativeWindow_unlockAndPost(nativeWindow);
                       usleep(1000 * 16);

                   }

                   ANativeWindow_release(nativeWindow);
                       av_frame_free(&frame);
                       av_frame_free(&rgb_frame);
                       avcodec_close(avCodecContext);
                       avformat_free_context(avFormatContext);
                       env->ReleaseStringUTFChars(inputStr_, inputPath);

}