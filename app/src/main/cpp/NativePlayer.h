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
    friend void *stop_fun(void *argv);
public:
    NativePlayer(CallJavaHelper *callJavaHelper,const char* dataSouce);
    ~NativePlayer();
    void prepare();

    void realPrepare();

    void start();

    void play();

    void pause();

    void resume();

    void stop();

    void setRenderFrameCallBack(void (*fun)(uint8_t *, int, int, int));

    void seek(int progress);

    double getCurrentPosition();

    int64_t getDuration();

private:
    void report_error_to_java(int thread_env,int error_code);
    void report_progress_to_java(int thread_env, int progress);
    void _stop();

private:
    pthread_t prepare_pid;
    pthread_t play_pid;
    pthread_t stop_pid;
    pthread_mutex_t seek_mutex;
    AVFormatContext *formatContext;
    CallJavaHelper *javaHelper;
    VideoChannel *videoChannel;
    AudioChannel *audioChannel;
    RenderFrame renderFrame;
    bool isPlaying;
    bool isPause = false;
    int64_t duration = 0; //视频时间
    char *url;
};


#endif //FFMPEGDEMO_NATIVEPLAYER_H
