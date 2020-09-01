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
#include "VideoChannel.h"
#include "AudioChannel.h"

extern "C" {
#include <libavcodec/avcodec.h>
#include <libavutil/time.h>
#include <libavformat/avformat.h>
}

class NativePlayer {

public:
    NativePlayer(CallJavaHelper *callJavaHelper,const char* dataSouce);
    ~NativePlayer();
    void prepare();

    void realPrepare();

    void start();

    void play();

    void pause();

    void resume();

    void setRenderFrameCallBack(void (*fun)(uint8_t *, int, int, int));

private:
    void report_error_to_java(int thread_env,int error_code);

private:
    pthread_t prepare_pid;
    pthread_t play_pid;
    AVFormatContext *formatContext;
    CallJavaHelper *javaHelper;
    VideoChannel *videoChannel;
    AudioChannel *audioChannel;
    RenderFrame renderFrame;
    bool isPlaying;
    bool isPause = false;
    char *url;
};


#endif //FFMPEGDEMO_NATIVEPLAYER_H
