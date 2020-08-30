//
// Created by liushuo on 2020/8/26.
//

#include "VideoChannel.h"
#include "common.h"

VideoChannel::VideoChannel(int id, CallJavaHelper *javaHelper, AVCodecContext *codecContext,
                           AVRational time_base)
        : BaseChannel(id, javaHelper, codecContext, time_base) {
    pkt_queue.setReleaseCallback(releasePacket);
    frame_queue.setReleaseCallback(releaseFrame);
}

VideoChannel::~VideoChannel() {

}

void *_decode(void *argv) {
    VideoChannel *videoChannel = static_cast<VideoChannel *>(argv);
    videoChannel->decodePacket();
}

void *_syncframe(void *argv) {
    VideoChannel *videoChannel = static_cast<VideoChannel *>(argv);
    videoChannel->syncFrame();
}


void VideoChannel::decodePacket() {
    //消费者线程，消费来自NativePlayer的packet
    AVPacket *packet = 0;
    while (isPlaying) {
        int ret = pkt_queue.deQueue(packet);
        if (!isPlaying) {
            break;
        }
        if (!ret) {
            continue;
        }
        ret = avcodec_send_packet(avCodecContext, packet);
        releasePacket(packet);
        if (ret == AVERROR(EAGAIN)) {
            continue;
        } else if (ret < 0) {
            break;
        }

        AVFrame *frame = av_frame_alloc();
        ret = avcodec_receive_frame(avCodecContext, frame);
        frame_queue.enQueue(frame);
        while (frame_queue.size() > 100 && isPlaying) {
            av_usleep(1000 * 10);
            continue;
        }
    }
    releasePacket(packet);
}


void VideoChannel::syncFrame() {
    //格式转换，把YUV的数据转换成RGB
    SwsContext *swsContext = sws_getContext(
            avCodecContext->width, avCodecContext->height, avCodecContext->pix_fmt,
            avCodecContext->width, avCodecContext->height, AV_PIX_FMT_RGBA, SWS_BILINEAR, 0, 0, 0);
    uint8_t *dst_data[4]; //RGBA
    int dst_linesize[4];
    av_image_alloc(dst_data, dst_linesize, avCodecContext->width, avCodecContext->height,
                   AV_PIX_FMT_RGBA, 1);
    AVFrame *frame = 0;
    while (isPlaying) {
        int ret = frame_queue.deQueue(frame);
        if (!isPlaying) {
            break;
        }
        if (!ret) {
            continue;
        }
        sws_scale(swsContext, reinterpret_cast<const uint8_t *const *>(frame->data),
                  frame->linesize, 0,
                  frame->height, dst_data, dst_linesize);
        //回调渲染
        renderFrame(dst_data[0], dst_linesize[0], avCodecContext->width, avCodecContext->height);
        //这里根据音频的clock开始同步

        clock = frame->pts * av_q2d(time_base);
        double audioClock = this->audioChannel->clock;
        double diff = clock - audioClock;
        double frame_delay = 1.0 / this->fps;
        if (clock > audioClock) { //视频播的快

        } else {

        }
        LOGI("xxxxfps=%d,xxxxdiff=%f",this->fps,diff);
        //av_usleep(30 * 1000);//30帧/s
        av_usleep((frame_delay + diff) * 1000000);
        releaseFrame(frame);
    }
    av_freep(&dst_data[0]);
    isPlaying = false;
    releaseFrame(frame);
    sws_freeContext(swsContext);
}

void VideoChannel::play() {
    pkt_queue.setWork(1);
    frame_queue.setWork(1);
    isPlaying = true;
    pthread_create(&pid_video_play, nullptr, _decode, this);
    pthread_create(&pid_sync, nullptr, _syncframe, this);
}


void VideoChannel::stop() {

}

void VideoChannel::setRenderFrame(RenderFrame func) {
    this->renderFrame = func;
}

void VideoChannel::setFps(int fps) {
    this->fps = fps;
    LOGI("this->fps=%d",fps);
}


