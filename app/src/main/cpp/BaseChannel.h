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
    BaseChannel(int id, CallJavaHelper *javaHelper, AVCodecContext *codecContext,AVRational time_base);

    virtual ~BaseChannel();
    virtual void play() = 0;
    virtual void stop() = 0;
    virtual void pause() {
        isPause = true;
        frame_queue.setWork(0);
        pkt_queue.setWork(0);
    };
    virtual void resume() {
        isPause = false;
        frame_queue.setWork(1);
        pkt_queue.setWork(1);
    };

    static void releasePacket(AVPacket *&pPacket);
    static void releaseFrame(AVFrame *&frame);

    SafeQueue<AVFrame *> frame_queue;
    volatile int channelId;
    volatile bool isPlaying;
    volatile bool isPause = false;
    CallJavaHelper *javaHelper;
    AVCodecContext *avCodecContext;
    SafeQueue<AVPacket *> pkt_queue;
    AVRational time_base;
    double clock = 0;
};

#endif //FFMPEGDEMO_BASECHANNEL_H