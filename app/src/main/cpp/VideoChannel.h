//
// Created by liushuo on 2020/8/26.
//

#ifndef FFMPEGDEMO_VIDEOCHANNEL_H
#define FFMPEGDEMO_VIDEOCHANNEL_H
#include "BaseChannel.h"

extern "C" {
#include <libavutil/time.h>
};

class VideoChannel : public BaseChannel {
public:
    VideoChannel(int id, CallJavaHelper *javaHelper, AVCodecContext *codecContext);
    ~VideoChannel();
    void play();
    void stop();

    void decodePacket();

    void syncFrame();

private:
    pthread_t pid_video_play; //解码线程(消费者线程)
    pthread_t pid_sync; //绘制线程
    inline void releasePacket(AVPacket *pPacket);
    inline void releaseFrame(AVFrame *frame);
};
#endif //FFMPEGDEMO_VIDEOCHANNEL_H
