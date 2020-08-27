//
// Created by liushuo on 2020/8/26.
//

#include "AudioChannel.h"

AudioChannel::AudioChannel(int id, CallJavaHelper *javaHelper, AVCodecContext *codecContext)
        : BaseChannel(id, javaHelper, codecContext) {

}

AudioChannel::~AudioChannel() {

}

void AudioChannel::play() {

}

void AudioChannel::stop() {
    isPlaying = false;
}
