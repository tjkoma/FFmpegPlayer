//
// Created by Administrator on 2019/3/26 0026.
//
#include <jni.h>
#include <string>

extern "C"
JNIEXPORT jint JNICALL Java_com_example_administrator_ffmpegplayer_MainActivity_multi(
        JNIEnv* env,
        jobject /* this */,
        jint a,
        jint b){
    return a*b;
}
