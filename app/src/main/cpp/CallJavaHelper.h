//
// Created by liushuo on 2020/8/25.
//

#ifndef FFMPEGDEMO_CALLJAVAHELPER_H
#define FFMPEGDEMO_CALLJAVAHELPER_H
#include <jni.h>

class CallJavaHelper {

public:
    CallJavaHelper(JavaVM *javaVm, JNIEnv *env, jobject &obj);
    ~CallJavaHelper();

    void onError(int thread, int code);
    void onPrepare(int thread);
    void onProgress(int thread,int progress);

private:
    JavaVM *javaVm;
    JNIEnv *jniEnv;
    jobject jobj;

    //下面是Java的回调方法
    jmethodID jmid_prepare;
    jmethodID jmid_error;
    jmethodID jmid_progress;
};


#endif //FFMPEGDEMO_CALLJAVAHELPER_H
