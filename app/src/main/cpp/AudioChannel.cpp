//
// Created by liushuo on 2020/8/26.
//

#include "AudioChannel.h"

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
}

void AudioChannel::stop() {
    isPlaying = false;
}
