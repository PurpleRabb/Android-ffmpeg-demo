package com.example.ffmpegdemo

import android.app.Application
import android.util.Log
import androidx.lifecycle.*

enum class PlayStatus {
    PLAYING,
    STOP,
    PAUSE,
    RESTART
}

class FFPlayerViewModel(application: Application) : AndroidViewModel(application),
    LifecycleObserver {
    private var _error : MutableLiveData<Int> = MutableLiveData()
    private var _progress : MutableLiveData<Int> = MutableLiveData()
    private var _playStatus : MutableLiveData<PlayStatus> = MutableLiveData()
    private lateinit var _ffPlayer : FFPlayer

    var error : LiveData<Int> = _error
    var progress : LiveData<Int> = _progress
    var playStatus : LiveData<PlayStatus> = _playStatus


    fun setPlayer(ffPlayer: FFPlayer) {
        this._ffPlayer = ffPlayer
        _ffPlayer.setOnPrepareListener(object : FFPlayer.OnPrepareListener {
            override fun onPrepare() {
                _playStatus.postValue(PlayStatus.PLAYING)
                _ffPlayer.setStatus(PlayStatus.PLAYING)
                _ffPlayer.nativeStart();
            }
        })

        _ffPlayer.setOnErrorListener(object : FFPlayer.OnErrorListener {
            override fun onError(error: Int) {
                _error.postValue(error)
            }
        })
    }

    @OnLifecycleEvent(Lifecycle.Event.ON_PAUSE)
    fun pausePlayer() {
        Log.i("ffmplayer", "pausePlayer: ")
    }

    @OnLifecycleEvent(Lifecycle.Event.ON_RESUME)
    fun resumePlayer() {
        Log.i("ffmplayer", "resumePlayer: ")
    }
}