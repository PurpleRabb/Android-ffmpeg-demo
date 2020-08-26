//
// Created by liushuo on 2020/8/26.
//

#ifndef FFMPEGDEMO_AUDIOCHANNEL_H
#define FFMPEGDEMO_AUDIOCHANNEL_H

#include "BaseChannel.h"
class AudioChannel : public BaseChannel {
public:
    AudioChannel(int id, CallJavaHelper *javaHelper, AVCodecContext *codecContext);
    ~AudioChannel();
    void play();
    void stop();
};
#endif //FFMPEGDEMO_AUDIOCHANNEL_H
