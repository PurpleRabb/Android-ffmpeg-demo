package com.example.ffmpegdemo

import android.view.Surface
import android.view.SurfaceHolder
import android.view.SurfaceView

class FFPlayer {
    //external fun stringFromJNI(): String
    companion object {
        // Used to load the 'native-lib' library on application startup.
        init {
            System.loadLibrary("mediaplayerlib")
        }
    }

    private lateinit var surfaceHolder : SurfaceHolder

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

    external fun nativePrepare(dataSource : String)
    external fun nativeStart()
    external fun nativeSetSurface(surface : Surface)
}