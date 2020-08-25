//
// Created by 36472 on 2020/8/24.
//

#include "NativePlayer.h"

void *__prepare(void *arg) {
    auto *player = static_cast<NativePlayer*>(arg);
    player->realPrepare();
    return nullptr;
}

NativePlayer::NativePlayer(CallJavaHelper *javaHelper,const char *dataSouce) {
    url = new char[strlen(dataSouce) + 1];
    this->javaHelper = javaHelper;
}

void NativePlayer::prepare() {
    //这里通过this指针将NativePlayer实例传入线程函数
    pthread_create(&prepare_pid,nullptr,__prepare,this);
}

void NativePlayer::realPrepare() {

    avformat_network_init();
    formatContext = avformat_alloc_context();

    AVDictionary *opts = nullptr;
    av_dict_set(&opts, "timeout", "3000000", 0);

    int ret = avformat_open_input(&formatContext, url, nullptr, &opts);
    if (ret != 0) {
        // 回调java空间函数
        javaHelper->onError(CHILD_THREAD,ret);
    }
}
