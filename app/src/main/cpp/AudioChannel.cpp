//
// Created by liushuo on 2020/8/26.
//

#include "AudioChannel.h"

void *audioinit(void *argv) {
    AudioChannel *audioChannel = static_cast<AudioChannel *>(argv);
    audioChannel->audio_init();
}

void *audiodecode(void *argv) {
    AudioChannel *audioChannel = static_cast<AudioChannel *>(argv);
    audioChannel->audio_decode();
}

AudioChannel::AudioChannel(int id, CallJavaHelper *javaHelper, AVCodecContext *codecContext)
        : BaseChannel(id, javaHelper, codecContext) {
    pkt_queue.setReleaseCallback(releasePacket);
    frame_queue.setReleaseCallback(releaseFrame);
}

AudioChannel::~AudioChannel() {

}

void AudioChannel::play() {
    pkt_queue.setWork(1);
    frame_queue.setWork(1);
    isPlaying = true;
    pthread_create(&pid_audioinit, nullptr,audioinit,this);
    pthread_create(&pid_decode, nullptr,audiodecode,this);
}

void AudioChannel::stop() {
    isPlaying = false;
}

void AudioChannel::audio_init() {

    SLEngineItf engineInterface = nullptr; //音频引擎
    SLObjectItf engineObject = nullptr;  //音频对象
    SLObjectItf outputMixObject = nullptr; //混音器
    SLObjectItf bqPlayerObject = nullptr; //播放器
    SLPlayItf bqPlayerInterface = nullptr; //回调接口
    SLAndroidSimpleBufferQueueItf bqPlayerBufferQueue = nullptr;

    SLresult result;
    //初始化播放引擎====>
    result = slCreateEngine(&engineObject, 0, nullptr,0, nullptr, nullptr);
    if (result != SL_RESULT_SUCCESS)
        return;
    result = (*engineObject)->Realize(engineObject,SL_BOOLEAN_FALSE);
    if (result != SL_RESULT_SUCCESS)
        return;
    result = (*engineObject)->GetInterface(engineObject,SL_IID_ENGINE,&engineInterface);
    if (result != SL_RESULT_SUCCESS)
        return;
    //<=======初始化播放引擎
}

void AudioChannel::audio_decode() {

}
