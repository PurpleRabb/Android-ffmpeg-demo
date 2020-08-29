//
// Created by liushuo on 2020/8/26.
//

#ifndef FFMPEGDEMO_AUDIOCHANNEL_H
#define FFMPEGDEMO_AUDIOCHANNEL_H

#include <SLES/OpenSLES.h>
#include <SLES/OpenSLES_Android.h>
extern "C" {
#include <libswresample/swresample.h>
}

#include "BaseChannel.h"

class AudioChannel : public BaseChannel {
public:
    AudioChannel(int id, CallJavaHelper *javaHelper, AVCodecContext *codecContext);
    ~AudioChannel();
    void play();
    void stop();

    void audio_init();

    void audio_decode();

    int getPcm();

    uint8_t *buffer;
private:
    pthread_t pid_audioinit;
    pthread_t pid_decode;

    SwrContext *swrContext = nullptr;
    int out_channels;
    int sample_size;
    int sample_rate;

};
#endif //FFMPEGDEMO_AUDIOCHANNEL_H
