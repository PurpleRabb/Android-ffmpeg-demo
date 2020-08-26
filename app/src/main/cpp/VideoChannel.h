//
// Created by liushuo on 2020/8/26.
//

#ifndef FFMPEGDEMO_VIDEOCHANNEL_H
#define FFMPEGDEMO_VIDEOCHANNEL_H
#include "BaseChannel.h"

class VideoChannel : BaseChannel {
public:
    VideoChannel(int id, CallJavaHelper *javaHelper, AVCodecContext *codecContext);
    ~VideoChannel();
    void play();
};
#endif //FFMPEGDEMO_VIDEOCHANNEL_H
