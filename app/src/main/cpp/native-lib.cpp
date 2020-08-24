#include <jni.h>
#include <string>
#include <android/native_window.h>
#include <android/native_window_jni.h>
#include "NativePlayer.h"

extern "C" {
#include <libavcodec/avcodec.h>
}

ANativeWindow *nativeWindow = NULL;
NativePlayer *nativePlayer = NULL;

extern "C"
JNIEXPORT void JNICALL
Java_com_example_ffmpegdemo_FFPlayer_nativePrepare(JNIEnv *env, jobject thiz,
                                                   jstring data_source) {
    const char* dataSource = env->GetStringUTFChars(data_source,0);
    nativePlayer = new NativePlayer(dataSource);
    nativePlayer->prepare();
    env->ReleaseStringUTFChars(data_source,dataSource);
}

extern "C"
JNIEXPORT void JNICALL
Java_com_example_ffmpegdemo_FFPlayer_nativeStart(JNIEnv *env, jobject thiz) {
    // TODO: implement native_start()
}

extern "C"
JNIEXPORT void JNICALL
Java_com_example_ffmpegdemo_FFPlayer_nativeSetSurface(JNIEnv *env, jobject thiz,
                                                      jobject surface) {
    if (nativeWindow) {
        //考虑到转屏，需要先释放之前的window
        ANativeWindow_release(nativeWindow);
        nativeWindow = NULL;
    }
    nativeWindow = ANativeWindow_fromSurface(env,surface);

}