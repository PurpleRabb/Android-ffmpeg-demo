//
// Created by liushuo on 2020/8/26.
//

extern "C" {
#include <libavutil/time.h>
}

#include "AudioChannel.h"
#include "common.h"

void *audioinit(void *argv) {
    AudioChannel *audioChannel = static_cast<AudioChannel *>(argv);
    audioChannel->audio_init();
    return 0;
}

void *audiodecode(void *argv) {
    AudioChannel *audioChannel = static_cast<AudioChannel *>(argv);
    audioChannel->audio_decode();
    return 0;
}

AudioChannel::AudioChannel(int id, CallJavaHelper *javaHelper, AVCodecContext *codecContext, AVRational time_base)
        : BaseChannel(id, javaHelper, codecContext, time_base) {
    this->javaHelper = javaHelper;
    this->avCodecContext = avCodecContext;

    out_channels = av_get_channel_layout_nb_channels(AV_CH_LAYOUT_STEREO);
    sample_size = av_get_bytes_per_sample(AV_SAMPLE_FMT_S16);
    sample_rate = 44100;
    buffer = (uint8_t *) malloc(out_channels * sample_rate *sample_size);
    pkt_queue.setReleaseCallback(releasePacket);
    frame_queue.setReleaseCallback(releaseFrame);
}

AudioChannel::~AudioChannel() {

}

void bqPlayerCallBack(SLAndroidSimpleBufferQueueItf bq, void *context) {
    AudioChannel *audioChannel = static_cast<AudioChannel *>(context);
    int buffer_size = audioChannel->getPcm();
    if (buffer_size > 0) {
        (*bq)->Enqueue(bq, audioChannel->buffer, buffer_size);
    }
}

void AudioChannel::play() {
    swrContext = swr_alloc_set_opts(0, AV_CH_LAYOUT_STEREO, AV_SAMPLE_FMT_S16, sample_rate,
                                    avCodecContext->channel_layout,
                                    avCodecContext->sample_fmt,
                                    avCodecContext->sample_rate, 0, 0);
    swr_init(swrContext);

    pkt_queue.setWork(1);
    frame_queue.setWork(1);
    isPlaying = true;
    pthread_create(&pid_audioinit, nullptr, audioinit, this);
    pthread_create(&pid_decode, nullptr, audiodecode, this);
}

void AudioChannel::stop() {
    this->pkt_queue.clear();
    this->frame_queue.clear();
}

void AudioChannel::audio_init() {

    //1. 音频引擎
    SLEngineItf engineInterface = NULL;
    //音频对比昂
    SLObjectItf engineObject = NULL;
    //2. 设置混音器
    SLObjectItf outputMixObject = NULL;
    //3. 创建播放器
    SLObjectItf  bqPlayerObject = NULL;
    // 回调接口.
    SLPlayItf  bqPlayerInterface = NULL;
    //4. 创建缓冲队列和回调函数
//    SLAndroidSimpleBufferQueueItf bqPlayerBufferQueue = NULL;
    SLAndroidSimpleBufferQueueItf bqPlayerBufferQueue ;
    //创建音频引擎 .

    // ----------------------------1. 初始化播放器引擎-----------------------------------------------
    SLresult  result =   slCreateEngine(&engineObject, 0, NULL, 0, NULL, NULL);
    if(SL_RESULT_SUCCESS != result){
        return ;
    }
    // realize the engine .
    result = (*engineObject)->Realize(engineObject, SL_BOOLEAN_FALSE);
    if(SL_RESULT_SUCCESS != result){
        return ;
    }
    //获取音频接口，相当于surfaceHolder对于surfaceView的控制一样.
    result = (*engineObject)->GetInterface(engineObject, SL_IID_ENGINE, &engineInterface);
    if(SL_RESULT_SUCCESS != result){
        return ;
    }

    // ---------------------------2. 初始化混音器----------------------------------------------------
    //创建混音器.
    const SLInterfaceID ids[1] = {SL_IID_ENVIRONMENTALREVERB};
    const SLboolean req[1] = {SL_BOOLEAN_FALSE};
    result = (*engineInterface)->CreateOutputMix(engineInterface, &outputMixObject, 1, ids, req);
    //初始化混音器.
    result = (*outputMixObject)->Realize(outputMixObject, SL_BOOLEAN_FALSE);
    if(SL_RESULT_SUCCESS != result){
        return ;
    }

    //5. 设置播放状态
    //创建播放器.双声道最后写2. configure audio source
    SLDataLocator_AndroidSimpleBufferQueue android_queue = {SL_DATALOCATOR_ANDROIDSIMPLEBUFFERQUEUE,2};

    SLDataFormat_PCM pcm = {SL_DATAFORMAT_PCM,//播放pcm格式数据.
                            2,                //2个声道(立体声).
                            SL_SAMPLINGRATE_44_1,//44100hz的频率.
                            SL_PCMSAMPLEFORMAT_FIXED_16,//位数16bit
                            SL_PCMSAMPLEFORMAT_FIXED_16,//位数16bit.
                            SL_SPEAKER_FRONT_LEFT|SL_SPEAKER_FRONT_RIGHT,//立体声(前左前右)
                            SL_BYTEORDER_LITTLEENDIAN//小端模式.
    };

    SLDataSource audioSrc = {&android_queue, &pcm};

    // configure audio sink
    SLDataLocator_OutputMix outputMix = {SL_DATALOCATOR_OUTPUTMIX, outputMixObject};
    SLDataSink audioSnk = {&outputMix, NULL};
    /*
        * create audio player:
        *     fast audio does not support when SL_IID_EFFECTSEND is required, skip it
        *     for fast audio case
        */
    const SLInterfaceID ids2[1] = {SL_IID_BUFFERQUEUE/*, SL_IID_VOLUME, SL_IID_EFFECTSEND,
            SL_IID_MUTESOLO,*/};
    const SLboolean req2[1] = {SL_BOOLEAN_TRUE/*, SL_BOOLEAN_TRUE, SL_BOOLEAN_TRUE,
            SL_BOOLEAN_TRUE,*/ };
    (*engineInterface)->CreateAudioPlayer(engineInterface
            ,&bqPlayerObject //播放器
            ,&audioSrc//播放器参数 播放器缓冲队列 播放格式
            ,&audioSnk // 播放缓冲区
            ,1//播放接口回调个数.
            ,ids2 //设置播放队列ID
            ,req2 //是否采用内置的播放队列
    );
    //初始化播放器.
    // realize the player
    result = (*bqPlayerObject)->Realize(bqPlayerObject, SL_BOOLEAN_FALSE);
    if(SL_RESULT_SUCCESS != result){
        return ;
    }
    //LOGE("realize the player success!");
    //获取播放器接口player interface.
    // get the play interface
    result = (*bqPlayerObject)->GetInterface(bqPlayerObject, SL_IID_PLAY, &bqPlayerInterface);

    //获取pcm缓冲队列.
    // get the buffer queue interface
    result = (*bqPlayerObject)->GetInterface(bqPlayerObject, SL_IID_BUFFERQUEUE, &bqPlayerBufferQueue);
    //注册回调函数.
    if(SL_RESULT_SUCCESS != result){
        return ;
    }
    //LOGE("get the buffer queue interface success!");

    // register callback on the buffer queue
    result = (*bqPlayerBufferQueue)->RegisterCallback(bqPlayerBufferQueue, bqPlayerCallBack, this);
    if(SL_RESULT_SUCCESS != result){
        return ;
    }
    //LOGE("RegisterCallback success!");
    //设置播放状态.
    result = (*bqPlayerInterface)->SetPlayState(bqPlayerInterface, SL_PLAYSTATE_PLAYING);
    if(SL_RESULT_SUCCESS != result){
        return ;
    }

    //LOGE("SetPlayState SL_PLAYSTATE_PLAYING success!");
    //6. 启动回调函数.
    //LOGE("手动调用播放器 packet:%d",this->pkt_queue.size());
    bqPlayerCallBack(bqPlayerBufferQueue , this);
}

void AudioChannel::audio_decode() {
    AVPacket *avPacket;
    while (isPlaying) {
        if (isPause) {
            continue;
        }
        int ret = pkt_queue.deQueue(avPacket);
        if (!isPlaying) {
            break;
        }
        if (!ret)
            continue;
        ret = avcodec_send_packet(avCodecContext, avPacket);
        releasePacket(avPacket);
        if (ret == AVERROR(EAGAIN)) {
            continue;
        } else if (ret < 0) {
            break;
        }
        AVFrame *avFrame = av_frame_alloc();
        ret = avcodec_receive_frame(avCodecContext, avFrame);
        if (ret == AVERROR(EAGAIN)) {
            continue;
        } else if (ret < 0) {
            break;
        }

        while (frame_queue.size() > 100 && isPlaying) {
            av_usleep(1000 * 10);
            continue;
        }
        frame_queue.enQueue(avFrame);
    }
    releasePacket(avPacket);
}

int AudioChannel::getPcm() {
    AVFrame *frame;
    int data_size;
    while (isPlaying) {
        if (isPause) {
            continue;
        }
        int ret = frame_queue.deQueue(frame);
        if (!isPlaying) {
            break;
        }
        if (!ret) {
            continue;
        }
        uint64_t dst_nb_samples = av_rescale_rnd(
                swr_get_delay(swrContext, frame->sample_rate) + frame->nb_samples,
                sample_rate,
                frame->sample_rate,
                AV_ROUND_UP);

        int nb = swr_convert(swrContext, &buffer, dst_nb_samples,
                             (const uint8_t **) (frame->data), frame->nb_samples);
        data_size = nb * out_channels * sample_size;
        clock = frame->pts * av_q2d(time_base);
        break;
    }
    return data_size;
}
