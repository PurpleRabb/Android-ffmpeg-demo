//
// Created by liushuo on 2020/8/26.
//

#include "BaseChannel.h"

BaseChannel::BaseChannel(int id, CallJavaHelper *javaHelper, AVCodecContext *codecContext)
        : channelId(id), javaHelper(javaHelper), avCodecContext(codecContext) {

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
