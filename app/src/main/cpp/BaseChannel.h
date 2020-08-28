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
    virtual void stop() = 0;

    static void releasePacket(AVPacket *&pPacket);
    static void releaseFrame(AVFrame *&frame);

    SafeQueue<AVFrame *> frame_queue;
    volatile int channelId;
    volatile bool isPlaying;
    CallJavaHelper *javaHelper;
    AVCodecContext *avCodecContext;
    SafeQueue<AVPacket *> pkt_queue;
};

#endif //FFMPEGDEMO_BASECHANNEL_H