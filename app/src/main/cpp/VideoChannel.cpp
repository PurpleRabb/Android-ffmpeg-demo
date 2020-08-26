//
// Created by liushuo on 2020/8/26.
//

#include "VideoChannel.h"

VideoChannel::VideoChannel(int id, CallJavaHelper *javaHelper, AVCodecContext *codecContext)
        : BaseChannel(id, javaHelper, codecContext) {

}

VideoChannel::~VideoChannel() {

}

void *_decode(void *argv) {
    VideoChannel *videoChannel = static_cast<VideoChannel *>(argv);
    videoChannel->decodePacket();
}


void VideoChannel::decodePacket() {
    //消费者线程，消费来自NativePlayer的packet
    AVPacket *packet = 0;
    while (isPlaying) {
        int ret = pkt_queue.pop(packet);
        if (isPlaying) {
            break;
        }
        if (!ret) {
            continue;
        }
        ret = avcodec_send_packet(avCodecContext,packet);
        releasePacket(packet);
        if (ret == AVERROR(EAGAIN)) {
            continue;
        } else if (ret < 0) {
            break;
        }

        AVFrame *frame = av_frame_alloc();
        ret = avcodec_receive_frame(avCodecContext,frame);
        frame_queue.push(frame);
        while (frame_queue.size() > 100 && isPlaying) {
            av_usleep(1000 * 10);
            continue;
        }
    }
    releasePacket(packet);
}

void VideoChannel::play() {
    pkt_queue.setWork(1);
    frame_queue.setWork(1);
    isPlaying = true;
    pthread_create(&pid_video_play,nullptr,_decode,this);
}

void VideoChannel::stop() {

}

void VideoChannel::releasePacket(AVPacket *pPacket) {
    if (pPacket) {
        av_packet_free(&pPacket);
        pPacket = 0;
    }
}

void VideoChannel::releaseFrame(AVFrame *frame) {
    if (frame) {
        av_frame_free(&frame);
        frame = 0;
    }
}

