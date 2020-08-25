package com.example.ffmpegdemo

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
        fun onProgress(progress : Int)
    }

    interface OnErrorListener {
        fun onError(error : Int)
    }

    private lateinit var surfaceHolder : SurfaceHolder
    private var onPrepareListener : OnPrepareListener? = null
    private var onProgressListener : OnProgressListener? = null
    private var onErrorListener : OnErrorListener? = null

    public fun setSurfaceView(surfaceView: SurfaceView) {
        surfaceHolder = surfaceView.holder
        surfaceHolder.addCallback( object : SurfaceHolder.Callback {
            override fun surfaceChanged(
                holder: SurfaceHolder?,
                format: Int,
                width: Int,
                height: Int
            ) {

            }

            override fun surfaceDestroyed(holder: SurfaceHolder?) {
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

    external fun nativePrepare(dataSource : String)
    external fun nativeStart()
    external fun nativeSetSurface(surface : Surface)
}

