package com.example.ffmpegdemo

import androidx.appcompat.app.AppCompatActivity
import android.os.Bundle
import android.util.Log
import android.view.SurfaceHolder
import android.view.SurfaceView
import android.widget.SeekBar
import android.widget.SeekBar.OnSeekBarChangeListener
import androidx.lifecycle.Observer
import androidx.lifecycle.ViewModelProvider
import kotlinx.android.synthetic.main.activity_main.*
import androidx.lifecycle.lifecycleScope
import kotlinx.coroutines.delay
import kotlinx.coroutines.launch

class MainActivity : AppCompatActivity() {

    private var dataSource = "someplaces"
    lateinit var ffPlayerViewModel: FFPlayerViewModel
    private val TAG = "java_ffmplayer"
    private lateinit var surfaceHolder: SurfaceHolder
    private var isSeek = false;

    override fun onCreate(savedInstanceState: Bundle?) {
        super.onCreate(savedInstanceState)
        setContentView(R.layout.activity_main)

        ffPlayerViewModel = ViewModelProvider(
            this,
            ViewModelProvider.AndroidViewModelFactory(this.application)
        ).get(FFPlayerViewModel::class.java)

        surfaceHolder = surfaceView.holder
        surfaceHolder.addCallback(object : SurfaceHolder.Callback {
            override fun surfaceChanged(
                holder: SurfaceHolder?,
                format: Int,
                width: Int,
                height: Int
            ) {
                Log.i("setSurfaceView", "surfaceChanged")
                ffPlayerViewModel.setSurfaceView(surfaceView)
            }

            override fun surfaceDestroyed(holder: SurfaceHolder?) {
                Log.i("setSurfaceView", "surfaceDestroyed")
            }

            override fun surfaceCreated(holder: SurfaceHolder?) {
                Log.i("setSurfaceView", "surfaceCreated")
                //ffPlayerViewModel.setSurfaceView(surfaceView)
            }
        })

        lifecycle.addObserver(ffPlayerViewModel)
        seekBar.max = 100
        seekBar.setOnSeekBarChangeListener(object : OnSeekBarChangeListener {
            override fun onProgressChanged(seekBar: SeekBar?, progress: Int, fromUser: Boolean) {

            }

            override fun onStartTrackingTouch(seekBar: SeekBar?) {
                isSeek = true
            }

            override fun onStopTrackingTouch(seekBar: SeekBar?) {
                isSeek = false
                if (seekBar != null) {
                    ffPlayerViewModel.setProgress(seekBar.progress)
                }
            }
        })
        updateMediaProgress()

        ffPlayerViewModel.error.observe(this, Observer {
            //when (it) {  //test code
            Log.i(TAG, it.toString())
            //}
        })

        ffPlayerViewModel.progress.observe(this, Observer {
            //when (it) { //test code
            Log.i(TAG, it.toString())
            //}
        })

        ffPlayerViewModel.duration.observe(this, Observer {
            Log.i("java_ffmplayer","###duration" + it.toString())
            textView.text = getMinutes(it) + ":" + getSeconds(it)
        })

        ffPlayerViewModel.playStatus.observe(this, Observer {
            when (it) {
                PlayStatus.PLAYING -> {
                    Log.i(TAG, "start playing")
                    controlButton.setImageResource(R.drawable.ic_baseline_pause_24)
                }
                PlayStatus.PAUSE -> {
                    Log.i(TAG, "pause")
                    controlButton.setImageResource(R.drawable.ic_baseline_play_arrow_24)
                }
                PlayStatus.NOT_READY -> {
                    //controlButton.isEnabled = false
                }
                PlayStatus.STOP -> {
                    Log.i(TAG, "stopped")
                    //seekBar.progress = seekBar.max
                }
            }
        })

        controlButton.setOnClickListener {
            ffPlayerViewModel.setUri(getExternalFilesDir(null)?.absolutePath.toString() + "/test.mp4")
            ffPlayerViewModel.switchStatus()
        }
    }

    private fun getMinutes(secs: Long): String {
        var mins = secs / 60
        if (mins > 0) {
            if (mins < 10) {
                return "0" + mins.toString()
            } else {
                return mins.toString()
            }
        }
        return "00"
    }

    private fun getSeconds(secs: Long): String {
        var seconds = secs % 60
        if (seconds > 0) {
            if (seconds < 10) {
                return "0" + seconds.toString()
            } else {
                return seconds.toString()
            }
        }
        return "00"
    }


    private fun updateMediaProgress() {
        lifecycleScope.launch {
            while (true) {
                delay(1000)
                if (isSeek) {
                    continue
                }
                seekBar.progress = ffPlayerViewModel.getProgress()
            }
        }
    }
}
