//
// Created by liushuo on 2020/8/25.
//

#include "CallJavaHelper.h"
#include "common.h"

CallJavaHelper::CallJavaHelper(JavaVM *_javaVm, JNIEnv *_env, jobject &obj) : javaVm(_javaVm), jniEnv(_env){
    jobj = jniEnv->NewGlobalRef(obj);//延长obj的生命周期
    jclass jclazz = jniEnv->GetObjectClass(jobj);
    jmid_error = jniEnv->GetMethodID(jclazz,"onError","(I)V");
    jmid_prepare = jniEnv->GetMethodID(jclazz,"onPrepare","()V");
    jmid_progress = jniEnv->GetMethodID(jclazz,"onProgress","(I)V");
}

CallJavaHelper::~CallJavaHelper() {

}

void CallJavaHelper::onError(int thread, int code) {
    if (thread == CHILD_THREAD) {
        //如果调用者在子线程，需要先绑定当前的ENV
        if ( javaVm->AttachCurrentThread(&jniEnv,0) != JNI_OK ) {
            __android_log_print(ANDROID_LOG_INFO,TAG,"%s\n",__func__);
            return;
        }
        jniEnv->CallVoidMethod(jobj,jmid_error,code);
        javaVm->DetachCurrentThread();
    } else {
        jniEnv->CallVoidMethod(jobj,jmid_error,code);
    }
}

void CallJavaHelper::onPrepare(int thread) {
    if (thread == CHILD_THREAD) {
        //如果调用者在子线程，需要先绑定当前的ENV
        if ( javaVm->AttachCurrentThread(&jniEnv,0) != JNI_OK ) {
            __android_log_print(ANDROID_LOG_INFO,TAG,"%s\n",__func__);
            return;
        }
        jniEnv->CallVoidMethod(jobj,jmid_prepare);
        javaVm->DetachCurrentThread();
    } else {
        jniEnv->CallVoidMethod(jobj,jmid_prepare);
    }
}

void CallJavaHelper::onProgress(int thread, int progress) {
    if (thread == CHILD_THREAD) {
        //如果调用者在子线程，需要先绑定当前的ENV
        if ( javaVm->AttachCurrentThread(&jniEnv,0) != JNI_OK ) {
            __android_log_print(ANDROID_LOG_INFO,TAG,"%s\n",__func__);
            return;
        }
        jniEnv->CallVoidMethod(jobj,jmid_progress,progress);
        javaVm->DetachCurrentThread();
    } else {
        jniEnv->CallVoidMethod(jobj,jmid_progress,progress);
    }
}
