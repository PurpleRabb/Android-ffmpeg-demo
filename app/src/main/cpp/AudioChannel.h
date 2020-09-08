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
    AudioChannel(int id, CallJavaHelper *javaHelper, AVCodecContext *codecContext, AVRational time_base);
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

    //1. 音频引擎
    SLEngineItf engineInterface = NULL;
    //音频接口
    SLObjectItf engineObject = NULL;
    //2. 设置混音器
    SLObjectItf outputMixObject = NULL;
    //3. 创建播放器
    SLObjectItf  bqPlayerObject = NULL;
    // 回调接口.
    SLPlayItf  bqPlayerInterface = NULL;
    //4. 创建缓冲队列和回调函数
//    SLAndroidSimpleBufferQueueItf bqPlayerBufferQueue = NULL;
    SLAndroidSimpleBufferQueueItf bqPlayerBufferQueue ;

    SwrContext *swrContext = nullptr;
    int out_channels;
    int sample_size;
    int sample_rate;

};
#endif //FFMPEGDEMO_AUDIOCHANNEL_H
