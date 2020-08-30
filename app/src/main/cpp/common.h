//
// Created by liushuo on 2020/8/25.
//

#ifndef FFMPEGDEMO_COMMON_H
#define FFMPEGDEMO_COMMON_H
#include <android/log.h>

#define TAG "ffmplayer"
#define MAIN_THREAD 1
#define CHILD_THREAD 2

#define LOGI(...) __android_log_print(AV_LOG_INFO,TAG,__VA_ARGS__)
#define FFMPEG_OPEN_ERROR -200
#define FFMPEG_NOT_FIND_STREAM -201
#define FFMPEG_NOT_FIND_DECODER -202
#define FFMPEG_CREATE_CODEC_CONTEXT_FAIL -203
#define FFMPEG_CODEC_CONTEXT_PARAMETERS_FAIL -204
#define FFMPEG_OPEN_CODEC_FAIL -205
#endif //FFMPEGDEMO_COMMON_H
