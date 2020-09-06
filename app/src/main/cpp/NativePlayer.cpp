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
    __android_log_print(AV_LOG_INFO,TAG,"%s:%s",__func__,dataSouce);
    url = new char[strlen(dataSouce) + 1];
    strcpy(url,dataSouce);
    this->javaHelper = javaHelper;
}

void NativePlayer::prepare() {
    //这里通过this指针将NativePlayer实例传入线程函数
    pthread_create(&prepare_pid,nullptr,__prepare,this);
}

void NativePlayer::realPrepare() {
    //该函数在线程中执行
    avformat_network_init();

    formatContext = avformat_alloc_context();

    AVDictionary *opts = nullptr;
    av_dict_set(&opts, "timeout", "3000000", 0);
    __android_log_print(AV_LOG_INFO,TAG,"%s:%s",__func__,url);
    int ret = avformat_open_input(&formatContext, url, nullptr, &opts);
    if (ret != 0) {
        // 回调java空间函数
        __android_log_print(AV_LOG_INFO, TAG, "%s=%d", __func__, ret);
        report_error_to_java(CHILD_THREAD, FFMPEG_OPEN_ERROR);
        return;
    }

    //查找流
    if (avformat_find_stream_info(formatContext, nullptr) < 0) {
        report_error_to_java(CHILD_THREAD, FFMPEG_NOT_FIND_DECODER);
        return;
    }

    for (int i = 0; i < formatContext->nb_streams; i++) {
        AVCodecParameters *codecParameters = formatContext->streams[i]->codecpar;
        AVStream *curStream = formatContext->streams[i];
        //找到解码器
        AVCodec *dec = avcodec_find_decoder(codecParameters->codec_id);
        if (!dec) {
            report_error_to_java(CHILD_THREAD, FFMPEG_NOT_FIND_DECODER);
            return;
        }

        //创建上下文
        AVCodecContext *codecContext = avcodec_alloc_context3(dec);
        if (!codecContext) {
            report_error_to_java(CHILD_THREAD, FFMPEG_CREATE_CODEC_CONTEXT_FAIL);
            return;
        }

        //复制参数
        if (avcodec_parameters_to_context(codecContext,codecParameters) < 0) {
            report_error_to_java(CHILD_THREAD, FFMPEG_CODEC_CONTEXT_PARAMETERS_FAIL);
            return;
        }

        //打开解码器
        if ((ret = avcodec_open2(codecContext,dec,0)) != 0) {
            __android_log_print(AV_LOG_INFO, TAG, "%s=%d", __func__, ret);
            report_error_to_java(CHILD_THREAD, FFMPEG_OPEN_CODEC_FAIL);
            return;
        }

        if (formatContext->streams[i]->codecpar->codec_type == AVMEDIA_TYPE_VIDEO) {
            videoChannel = new VideoChannel(i, javaHelper, codecContext, curStream->time_base);
            AVRational frame_rate = curStream->avg_frame_rate;
            duration = formatContext->duration; //微秒
            int fps = av_q2d(frame_rate);
            LOGI("den=%d,num=%d,fps=%d,duration=%ld",frame_rate.den,frame_rate.num,fps,duration);
            videoChannel->setFps(fps);
            videoChannel->setDuration(duration);
            videoChannel->setRenderFrame(this->renderFrame);
        }

        if (formatContext->streams[i]->codecpar->codec_type == AVMEDIA_TYPE_AUDIO) {
            audioChannel = new AudioChannel(i, javaHelper, codecContext, curStream->time_base);
            videoChannel->audioChannel = audioChannel;
        }
    }

    if (videoChannel && videoChannel) {
        javaHelper->onPrepare(CHILD_THREAD);
    }
}

void *startThread(void *argv) {
    //生产者
    NativePlayer *nativePlayer = static_cast<NativePlayer *>(argv);
    nativePlayer->play();
    return 0;
}

void NativePlayer::play() {
    int ret = 0;
    while(isPlaying) {
        if (isPause) {
            continue;
        }
        //生产者线程
        if (audioChannel && audioChannel->pkt_queue.size() > 100) {
            //避免生产太快，消费太慢，队列崩溃
            av_usleep(1000 * 10);
            continue;
        }
        if (videoChannel && videoChannel->pkt_queue.size() > 100) {
            av_usleep(1000 * 10);
            continue;
        }
        AVPacket *packet = av_packet_alloc();
        if (!packet) {
            __android_log_print(AV_LOG_INFO,TAG,"push video packet %s","av_packet_alloc error!!!");
        }
        ret = av_read_frame(formatContext,packet);
        if (ret == 0) {
            if (videoChannel && packet->stream_index == videoChannel->channelId) {
                //__android_log_print(AV_LOG_INFO,TAG,"push video packet %d",videoChannel->pkt_queue.size());
                videoChannel->pkt_queue.enQueue(packet);
            }
            if (audioChannel && packet->stream_index == audioChannel->channelId) {
                //__android_log_print(AV_LOG_INFO,TAG,"push audio packet %d",audioChannel->pkt_queue.size());
                audioChannel->pkt_queue.enQueue(packet);
            }
        } else if (ret == AVERROR_EOF) { //播放完毕
            if (videoChannel->pkt_queue.empty() && videoChannel->frame_queue.empty()
                    && audioChannel->pkt_queue.empty() && audioChannel->frame_queue.empty()) {
                report_progress_to_java(CHILD_THREAD,1);
                __android_log_print(AV_LOG_INFO,TAG,"%s","play eof");
                break;
            }
       } else {
            __android_log_print(AV_LOG_INFO,TAG,"%s","play %d",ret);
            break;
        }
    }
    isPlaying = false;
    if (audioChannel)
        audioChannel->stop();
    if (videoChannel)
        videoChannel->stop();
}

void NativePlayer::start() {
    __android_log_print(AV_LOG_INFO,TAG,"%s",__func__);
    isPlaying = true;
    if (videoChannel) {
        videoChannel->play();
    }
    if(audioChannel) {
        audioChannel->play();
    }
    pthread_create(&play_pid, nullptr,startThread,this);
}

void NativePlayer::pause() {
    isPause = true;
    videoChannel->pause();
    audioChannel->pause();
}

void NativePlayer::resume() {
    isPause = false;
    audioChannel->resume();
    videoChannel->resume();
}

void NativePlayer::report_error_to_java(int thread_env, int error_code) {
    if (javaHelper) {
        javaHelper->onError(thread_env, error_code);
    }
}


void NativePlayer::report_progress_to_java(int thread_env,int progress) {
    if (javaHelper) {
        javaHelper->onProgress(thread_env,progress);
    }
}

NativePlayer::~NativePlayer() {
    delete url;
}

void NativePlayer::setRenderFrameCallBack(void (*fun)(uint8_t *, int, int, int)) {
    this->renderFrame = fun; //暂存
}

double NativePlayer::getCurrentPosition() {
    if (videoChannel != nullptr) {
        return videoChannel->getCurrentPosition();
    }
    return 0;
}

void NativePlayer::seek(int progress) {
    if (videoChannel == nullptr) {
        return;
    }
    videoChannel->stop();
    audioChannel->stop();
    int _progress = ((double)progress/100) * duration;
    LOGI("_progress=%d",_progress);
    int ret = av_seek_frame(formatContext, -1, _progress, AVSEEK_FLAG_BACKWARD);
    if (ret < 0) {
        javaHelper->onError(MAIN_THREAD, FFMPEG_SEEK_ERROR);
        return;
    }
}


