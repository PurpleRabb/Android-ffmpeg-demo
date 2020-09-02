package com.example.ffmpegdemo

import android.app.Application
import android.util.Log
import android.view.SurfaceView
import androidx.lifecycle.*

enum class PlayStatus {
    PLAYING,
    STOP,
    PAUSE,
    RESTART
}

class FFPlayerViewModel(application: Application) : AndroidViewModel(application),
    LifecycleObserver {
    private var _error: MutableLiveData<Int> = MutableLiveData()
    private var _progress: MutableLiveData<Int> = MutableLiveData()
    private var _playStatus: MutableLiveData<PlayStatus> = MutableLiveData()
    var ffPlayer: FFPlayer = FFPlayer()

    var error: LiveData<Int> = _error
    var progress: LiveData<Int> = _progress
    var playStatus: LiveData<PlayStatus> = _playStatus

    init {
        setPlayer(ffPlayer)
    }

    fun setSurfaceView(surfaceView: SurfaceView) {
        ffPlayer.setSurfaceView(surfaceView)
    }

    fun setPlayer(ffPlayer: FFPlayer) {
        ffPlayer.setOnPrepareListener(object : FFPlayer.OnPrepareListener {
            override fun onPrepare() {
                _playStatus.postValue(PlayStatus.PLAYING)
                ffPlayer.setStatus(PlayStatus.PLAYING)
                ffPlayer.nativeStart();
            }
        })

        this.ffPlayer.setOnErrorListener(object : FFPlayer.OnErrorListener {
            override fun onError(error: Int) {
                _error.postValue(error)
            }
        })

        this.ffPlayer.setOnProgressListener(object : FFPlayer.OnProgressListener {
            override fun onProgress(progress: Int) {
                if (progress == 1)
                    _playStatus.postValue(PlayStatus.STOP)
            }
        })
    }

    @OnLifecycleEvent(Lifecycle.Event.ON_PAUSE)
    fun pausePlayer() {
        Log.i("ffmplayer", "pausePlayer: ")
        ffPlayer.pause()
    }

    @OnLifecycleEvent(Lifecycle.Event.ON_RESUME)
    fun resumePlayer() {
        Log.i("ffmplayer", "resumePlayer: ")
        ffPlayer.resume();
    }
}