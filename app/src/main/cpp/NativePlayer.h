//
// Created by 36472 on 2020/8/24.
//

#ifndef FFMPEGDEMO_NATIVEPLAYER_H
#define FFMPEGDEMO_NATIVEPLAYER_H
#include <pthread.h>
#include <android/native_window_jni.h>
extern "C" {
#include <libavcodec/avcodec.h>
#include <libavutil/time.h>
};

class NativePlayer {

public:
    NativePlayer(const char* dataSouce);
    void prepare();

    void realPrepare();

private:
    pthread_t prepare_pid;
};


#endif //FFMPEGDEMO_NATIVEPLAYER_H
