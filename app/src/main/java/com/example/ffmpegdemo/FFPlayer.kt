package com.example.ffmpegdemo

import android.util.Log
import android.view.Surface
import android.view.SurfaceHolder
import android.view.SurfaceView

class FFPlayer {

    init {
        System.loadLibrary("mediaplayerlib")
    }

    interface OnPrepareListener {
        fun onPrepare()
    }

    interface OnProgressListener {
        fun onProgress(progress: Int)
    }

    interface OnErrorListener {
        fun onError(error: Int)
    }

    private lateinit var surfaceHolder: SurfaceHolder
    private var onPrepareListener: OnPrepareListener? = null
    private var onProgressListener: OnProgressListener? = null
    private var onErrorListener: OnErrorListener? = null
    private var status: PlayStatus = PlayStatus.STOP
    private var pausedByUser = false
    private lateinit var dataSource: String


    public fun setSurfaceView(surfaceView: SurfaceView) {
        surfaceHolder = surfaceView.holder
        surfaceHolder.addCallback(object : SurfaceHolder.Callback {
            override fun surfaceChanged(
                holder: SurfaceHolder?,
                format: Int,
                width: Int,
                height: Int
            ) {
                Log.i("setSurfaceView","surfaceChanged")
            }

            override fun surfaceDestroyed(holder: SurfaceHolder?) {
                Log.i("setSurfaceView","surfaceDestroyed")
            }

            override fun surfaceCreated(holder: SurfaceHolder?) {
                holder?.surface?.let { nativeSetSurface(it) }
            }

        })
    }

    fun setOnPrepareListener(listener: OnPrepareListener) {
        onPrepareListener = listener
    }

    fun setOnProgressListener(listener: OnProgressListener) {
        onProgressListener = listener
    }

    fun setOnErrorListener(listener: OnErrorListener) {
        onErrorListener = listener
    }

    fun onPrepare() {  //called by native
        this.onPrepareListener?.onPrepare()
    }

    fun onProgress(progress: Int) {  //called by native
        this.onProgressListener?.onProgress(progress)
    }

    fun onError(error: Int) { //called by native
        this.onErrorListener?.onError(error)
    }

    fun setStatus(status: PlayStatus) {
        this.status = status
    }

    fun getStatus(): PlayStatus {
        return this.status
    }

    fun switchStatus() {
        when (status) {
            PlayStatus.PAUSE -> {
                status = PlayStatus.PLAYING
                pausedByUser = false
                nativeResume()
            }
            PlayStatus.PLAYING -> {
                status = PlayStatus.PAUSE
                pausedByUser = true
                nativePause()
            }
            PlayStatus.STOP -> {
                status = PlayStatus.PLAYING
                pausedByUser = false
                nativePrepare(dataSource)
            }
        }
    }

    fun setUri(s: String) {
        dataSource = s
    }

    fun getProgress(): Int {
        return (nativeGetCurrentPosition() * 100).toInt()
    }

    fun resume() {
        if (status == PlayStatus.PAUSE) {
            status = PlayStatus.PLAYING
            nativeResume()
        }
    }

    fun pause() {
        if (status == PlayStatus.PLAYING) {
            status = PlayStatus.PAUSE
            nativePause()
        }
    }

    external fun nativePrepare(dataSource: String)
    external fun nativeStart()
    external fun nativeSetSurface(surface: Surface)
    external fun nativeGetCurrentPosition(): Double
    external fun nativePause()
    external fun nativeResume()
}

