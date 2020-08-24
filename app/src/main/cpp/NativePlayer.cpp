//
// Created by 36472 on 2020/8/24.
//

#include "NativePlayer.h"

void *__prepare(void *arg) {
    NativePlayer *player = static_cast<NativePlayer*>(arg);
    player->realPrepare();
}

NativePlayer::NativePlayer(const char *dataSouce) {

}

void NativePlayer::prepare() {
    pthread_create(&prepare_pid,NULL,__prepare,this);
}

void NativePlayer::realPrepare() {

}
