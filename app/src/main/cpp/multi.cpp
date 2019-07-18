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
#ifdef ANDROID
#define LOGE(format, ...)  __android_log_print(ANDROID_LOG_ERROR, "(>_<)", format, ##__VA_ARGS__)
#define LOGI(format, ...)  __android_log_print(ANDROID_LOG_INFO,  "(^_^)", format, ##__VA_ARGS__)
#else
#endif
}

extern "C"
JNIEXPORT jint JNICALL Java_com_example_administrator_ffmpegplayer_MainActivity_multi(
        JNIEnv *env,
        jobject /* this */,
        jint a,
        jint b) {
    return a * b;
}