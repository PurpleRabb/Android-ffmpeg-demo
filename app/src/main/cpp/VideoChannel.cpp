//
// Created by liushuo on 2020/8/26.
//

#include "VideoChannel.h"

VideoChannel::VideoChannel(int id, CallJavaHelper *javaHelper, AVCodecContext *codecContext)
        : BaseChannel(id, javaHelper, codecContext) {

}

VideoChannel::~VideoChannel() {

}

void VideoChannel::play() {

}
