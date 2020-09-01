//
// Created by liushuo on 2020/8/26.
//

#ifndef FFMPEGDEMO_VIDEOCHANNEL_H
#define FFMPEGDEMO_VIDEOCHANNEL_H
#include "BaseChannel.h"
#include "AudioChannel.h"""

extern "C" {
#include <libavutil/time.h>
#include <libavutil/avutil.h>
#include <libswscale/swscale.h>
#include <libavutil/imgutils.h>
};
typedef void (*RenderFrame)(uint8_t *,int,int,int);
class VideoChannel : public BaseChannel {
public:
    VideoChannel(int id, CallJavaHelper *javaHelper, AVCodecContext *codecContext, AVRational time_base);
    ~VideoChannel();
    void play();

    void stop();

    void decodePacket();

    void syncFrame();

    void setRenderFrame(RenderFrame func);

    void setFps(int fps);

    AudioChannel *audioChannel;
private:
    pthread_t pid_video_play; //解码线程(消费者线程)
    pthread_t pid_sync; //绘制线程
    RenderFrame renderFrame;
    int fps;
};
#endif //FFMPEGDEMO_VIDEOCHANNEL_H
