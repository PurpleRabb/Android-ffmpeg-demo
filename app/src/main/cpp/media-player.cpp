#include <jni.h>
#include <string>
#include <android/native_window.h>
#include <android/native_window_jni.h>
#include "NativePlayer.h"
#include "CallJavaHelper.h"

extern "C" {
#include <libavcodec/avcodec.h>
}

CallJavaHelper *callJavaHelper;
JavaVM *javaVM = nullptr;
ANativeWindow *nativeWindow = nullptr;
NativePlayer *nativePlayer = nullptr;

JNIEXPORT jint JNI_OnLoad(JavaVM* vm, void* reserved){
    __android_log_print(ANDROID_LOG_INFO,TAG,"%s\n",__func__);
    javaVM = vm;
    return JNI_VERSION_1_6;
}

extern "C"
JNIEXPORT void JNICALL
Java_com_example_ffmpegdemo_FFPlayer_nativePrepare(JNIEnv *env, jobject thiz,
                                                   jstring data_source) {
    const char* dataSource = env->GetStringUTFChars(data_source,nullptr);
    callJavaHelper = new CallJavaHelper(javaVM,env,thiz);
    nativePlayer = new NativePlayer(callJavaHelper,dataSource);
    nativePlayer->prepare();//开启准备线程
    env->ReleaseStringUTFChars(data_source,dataSource);
}

extern "C"
JNIEXPORT void JNICALL
Java_com_example_ffmpegdemo_FFPlayer_nativeStart(JNIEnv *env, jobject thiz) {
    // TODO: implement native_start()
    nativePlayer->start();
}

extern "C"
JNIEXPORT void JNICALL
Java_com_example_ffmpegdemo_FFPlayer_nativeSetSurface(JNIEnv *env, jobject thiz,
                                                      jobject surface) {
    if (nativeWindow) {
        //考虑到转屏，需要先释放之前的window
        ANativeWindow_release(nativeWindow);
        nativeWindow = nullptr;
    }
    nativeWindow = ANativeWindow_fromSurface(env,surface);
}