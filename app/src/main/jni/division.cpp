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

        //注册组件
        av_register_all();
        //初始化AVFormatContext的上下文
        AVFormatContext *format_context = avformat_alloc_context();
}