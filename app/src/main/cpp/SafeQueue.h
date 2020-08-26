#ifndef FFMPEGDEMO_SAFE_QUEUE_H
#define FFMPEGDEMO_SAFE_QUEUE_H

#include <queue>
#include <pthread.h>

using namespace std;
//线程安全的队列
template<typename T>
class SafeQueue {
    typedef void (*ReleaseCallBack)(T &);

    typedef void (*SyncHandle)(queue<T> &);

public:
    SafeQueue() {
        pthread_mutex_init(&mutex, NULL);
        pthread_cond_init(&cond, NULL);
    }

    ~SafeQueue() {
        pthread_cond_destroy(&cond);
        pthread_mutex_destroy(&mutex);
    }

    void push(const T new_value) {
        pthread_mutex_lock(&mutex);
        if (work) {
            q.push(new_value);
            pthread_cond_signal(&cond);
            pthread_mutex_unlock(&mutex);
        }
        pthread_mutex_unlock(&mutex);
    }


    int pop(T &value) {
        int ret = 0;
        pthread_mutex_lock(&mutex);
        //在多核处理器下 由于竞争可能虚假唤醒 包括jdk也说明了
        while (work && q.empty()) {
            pthread_cond_wait(&cond, &mutex);
        }
        if (!q.empty()) {
            value = q.front();
            q.pop();
            ret = 1;
        }
        pthread_mutex_unlock(&mutex);
        return ret;
    }

    void setWork(int work) {
        pthread_mutex_lock(&mutex);
        this->work = work;
        pthread_cond_signal(&cond);
        pthread_mutex_unlock(&mutex);
    }

    int empty() {
        return q.empty();
    }

    int size() {
        return q.size();
    }

    void clear() {
        pthread_mutex_lock(&mutex);
        int size = q.size();
        for (int i = 0; i < size; ++i) {
            T value = q.front();
            releaseCallBack(value);
            q.pop();
        }
        pthread_mutex_unlock(&mutex);
    }

    void sync() {
        pthread_mutex_lock(&mutex);
        // 同步代码块 能在线程安全的背景下操作 queue :例如 主动丢包
        syncHandle(q);
        pthread_mutex_unlock(&mutex);
    }

    void setReleaseCallBack(ReleaseCallBack r) {
        releaseCallBack = r;
    }

    void setSyncHandle(SyncHandle s) {
        syncHandle = s;
    }

private:

    pthread_cond_t cond;
    pthread_mutex_t mutex;

    queue<T> q;
    // 是否工作  1工作  0不工作
    int work;
    ReleaseCallBack releaseCallBack;
    SyncHandle syncHandle;

};


#endif //FFMPEGDEMO_SAFE_QUEUE_H
