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

void renderFrame(uint8_t *data, int linesize, int w, int h) {
    //真正的渲染函数
    //__android_log_print(ANDROID_LOG_INFO, TAG, "%s\n", __func__);
    if (nativeWindow == nullptr) {
        return;
    }
    ANativeWindow_setBuffersGeometry(nativeWindow, w, h, WINDOW_FORMAT_RGBA_8888);
    ANativeWindow_Buffer windowBuffer;

    if (ANativeWindow_lock(nativeWindow, &windowBuffer, 0)) {
        ANativeWindow_release(nativeWindow);
        nativeWindow = 0;
        return;
    }
    uint8_t *dst_data = static_cast<uint8_t *>(windowBuffer.bits);
    int window_linesize = windowBuffer.stride * 4;
    uint8_t *src_data = data;
    for (int i = 0; i < windowBuffer.height; i++) {//按行copy
        memcpy(dst_data + i * window_linesize, src_data + i * linesize, window_linesize);
    }
    ANativeWindow_unlockAndPost(nativeWindow);
}

JNIEXPORT jint JNI_OnLoad(JavaVM *vm, void *reserved) {
    __android_log_print(ANDROID_LOG_INFO, TAG, "%s\n", __func__);
    javaVM = vm;
    return JNI_VERSION_1_6;
}

extern "C"
JNIEXPORT void JNICALL
Java_com_example_ffmpegdemo_FFPlayer_nativePrepare(JNIEnv *env, jobject thiz,
                                                   jstring data_source) {
    const char *dataSource = env->GetStringUTFChars(data_source, nullptr);
    callJavaHelper = new CallJavaHelper(javaVM, env, thiz);
    nativePlayer = new NativePlayer(callJavaHelper, dataSource);
    nativePlayer->setRenderFrameCallBack(renderFrame);
    nativePlayer->prepare();//开启准备线程
    env->ReleaseStringUTFChars(data_source, dataSource);
}

extern "C"
JNIEXPORT void JNICALL
Java_com_example_ffmpegdemo_FFPlayer_nativeStart(JNIEnv *env, jobject thiz) {
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
    __android_log_print(ANDROID_LOG_INFO, TAG, "%s\n", __func__);
    nativeWindow = ANativeWindow_fromSurface(env, surface);
}

extern "C"
JNIEXPORT void JNICALL
Java_com_example_ffmpegdemo_FFPlayer_nativePause(JNIEnv *env, jobject thiz) {
    LOGI("%s",__func__);
    nativePlayer->pause();
}

extern "C"
JNIEXPORT void JNICALL
Java_com_example_ffmpegdemo_FFPlayer_nativeResume(JNIEnv *env, jobject thiz) {
    LOGI("%s",__func__);
    nativePlayer->resume();
}

extern "C"
JNIEXPORT jdouble JNICALL
Java_com_example_ffmpegdemo_FFPlayer_nativeGetCurrentPosition(JNIEnv *env, jobject thiz) {
    if (nativePlayer != nullptr) {
        return nativePlayer->getCurrentPosition();
    }
    return 0;
}

extern "C"
JNIEXPORT void JNICALL
Java_com_example_ffmpegdemo_FFPlayer_nativeSetProgress(JNIEnv *env, jobject thiz, jint progress) {
    if (nativePlayer != nullptr) {
        nativePlayer->seek(progress);
    }
}
extern "C"
JNIEXPORT jlong JNICALL
Java_com_example_ffmpegdemo_FFPlayer_getDuration(JNIEnv *env, jobject thiz) {
    if (nativePlayer != nullptr) {
        return nativePlayer->getDuration();
    }
    return 0;
}

extern "C"
JNIEXPORT void JNICALL
Java_com_example_ffmpegdemo_FFPlayer_nativeStop(JNIEnv *env, jobject thiz) {
    if (nativePlayer != nullptr) {
        nativePlayer->stop();
    }
}