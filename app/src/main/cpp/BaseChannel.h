//
// Created by liushuo on 2020/8/26.
//

#ifndef FFMPEGDEMO_BASECHANNEL_H
#define FFMPEGDEMO_BASECHANNEL_H
#include "CallJavaHelper.h"
#include "SafeQueue.h"

extern "C" {
#include <libavformat/avformat.h>
}

class BaseChannel {
public:
    BaseChannel(int id,CallJavaHelper *javaHelper,AVCodecContext *codecContext);
    virtual ~BaseChannel();
    virtual void play() = 0;
};

#endif //FFMPEGDEMO_BASECHANNEL_H