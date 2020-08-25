//
// Created by 36472 on 2020/8/24.
//

#ifndef FFMPEGDEMO_NATIVEPLAYER_H
#define FFMPEGDEMO_NATIVEPLAYER_H
#include <pthread.h>
#include <android/native_window_jni.h>
#include <android/log.h>
#include "common.h"
#include "CallJavaHelper.h"

extern "C" {
#include <libavcodec/avcodec.h>
#include <libavutil/time.h>
#include <libavformat/avformat.h>
}

class NativePlayer {

public:
    NativePlayer(CallJavaHelper *callJavaHelper,const char* dataSouce);
    void prepare();

    void realPrepare();

private:
    pthread_t prepare_pid{};
    AVFormatContext *formatContext;
    CallJavaHelper *javaHelper;
    char *url;
};


#endif //FFMPEGDEMO_NATIVEPLAYER_H
