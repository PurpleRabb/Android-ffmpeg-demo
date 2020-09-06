package com.example.ffmpegdemo

import android.app.Application
import android.util.Log
import android.view.SurfaceView
import androidx.lifecycle.*

enum class PlayStatus {
    PLAYING,
    STOP,
    PAUSE,
    NOT_READY,
    RESTART
}

class FFPlayerViewModel(application: Application) : AndroidViewModel(application),
    LifecycleObserver {
    private lateinit var dataSource: String
    private var _error: MutableLiveData<Int> = MutableLiveData()
    private var _progress: MutableLiveData<Int> = MutableLiveData()
    private var _playStatus: MutableLiveData<PlayStatus> = MutableLiveData()

    private var ffPlayer: FFPlayer = FFPlayer()

    var error: LiveData<Int> = _error
    var progress: LiveData<Int> = _progress
    var playStatus: LiveData<PlayStatus> = _playStatus

    init {
        setPlayer(ffPlayer)
        _playStatus.value = PlayStatus.NOT_READY
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

    fun setProgress(progress: Int) {
        ffPlayer.setProgress(progress)
    }

    fun setUri(s: String) {
        this.dataSource = s
        ffPlayer.setUri(s)
    }

    fun switchStatus() {
        when (_playStatus.value) {
            PlayStatus.PAUSE -> {
                ffPlayer.setStatus(PlayStatus.PAUSE)
                _playStatus.value = PlayStatus.PLAYING
                ffPlayer.pausedByUser = false
                ffPlayer.resume()
            }
            PlayStatus.PLAYING -> {
                ffPlayer.setStatus(PlayStatus.PLAYING)
                _playStatus.value = PlayStatus.PAUSE
                ffPlayer.pausedByUser = true
                ffPlayer.pause()
            }
            PlayStatus.NOT_READY -> {
                ffPlayer.nativePrepare(dataSource)
            }
            PlayStatus.STOP -> {
                ffPlayer.setStatus(PlayStatus.PLAYING)
                ffPlayer.pausedByUser = false
                ffPlayer.nativePrepare(dataSource)
            }
        }
    }

    fun getProgress(): Int {
        return ffPlayer.getProgress()
    }
}