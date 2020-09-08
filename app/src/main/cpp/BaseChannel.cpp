//
// Created by liushuo on 2020/8/26.
//

#include "BaseChannel.h"

BaseChannel::BaseChannel(int id, CallJavaHelper *javaHelper, AVCodecContext *codecContext, AVRational time_base)
        : channelId(id), javaHelper(javaHelper), avCodecContext(codecContext),time_base(time_base) {

}

BaseChannel::~BaseChannel() {
    if (avCodecContext) {
        avcodec_close(avCodecContext);
        avcodec_free_context(&avCodecContext);
        avCodecContext = 0;
        pkt_queue.clear();
        frame_queue.clear();
    }
}


void BaseChannel::releasePacket(AVPacket *&pPacket) {
    if (pPacket) {
        av_packet_free(&pPacket);
        pPacket = 0;
    }
}

void BaseChannel::releaseFrame(AVFrame *&frame) {
    if (frame) {
        av_frame_free(&frame);
        frame = 0;
    }
}

void BaseChannel::reset() {
    pkt_queue.setWork(0);
    frame_queue.setWork(0);
    pkt_queue.clear();
    frame_queue.clear();
    pkt_queue.setWork(1);
    frame_queue.setWork(1);
}
