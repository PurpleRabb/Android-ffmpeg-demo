package com.example.ffmpegdemo

import androidx.appcompat.app.AppCompatActivity
import android.os.Bundle
import android.util.Log
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
    lateinit var ffPlayerViewModel : FFPlayerViewModel
    private val TAG = "java_ffmplayer"
    private lateinit var ffPlayer : FFPlayer
    override fun onCreate(savedInstanceState: Bundle?) {
        super.onCreate(savedInstanceState)
        setContentView(R.layout.activity_main)

        ffPlayerViewModel = ViewModelProvider(this, ViewModelProvider.AndroidViewModelFactory(this.application)).get(FFPlayerViewModel::class.java)
        ffPlayerViewModel.setSurfaceView(surfaceView)

        lifecycle.addObserver(ffPlayerViewModel)
        seekBar.max = 100
        seekBar.setOnSeekBarChangeListener( object : OnSeekBarChangeListener {
            override fun onProgressChanged(seekBar: SeekBar?, progress: Int, fromUser: Boolean) {

            }

            override fun onStartTrackingTouch(seekBar: SeekBar?) {

            }

            override fun onStopTrackingTouch(seekBar: SeekBar?) {

            }
        })
        updateMediaProgress()

        ffPlayerViewModel.error.observe(this, Observer {
            //when (it) {  //test code
                Log.i(TAG,it.toString())
            //}
        })

        ffPlayerViewModel.progress.observe(this, Observer {
            //when (it) { //test code
                Log.i(TAG,it.toString())
            //}
        })

        ffPlayerViewModel.playStatus.observe(this, Observer {
            when (it) {
                PlayStatus.PLAYING ->  {
                    Log.i(TAG, "start playing")
                }
                PlayStatus.STOP -> {
                    Log.i(TAG, "stopped")
                    seekBar.progress = seekBar.max
                }
            }
        })

        controlButton.setOnClickListener {
            ffPlayerViewModel.ffPlayer.setUri(getExternalFilesDir(null)?.absolutePath.toString()+"/test.mp4")
            ffPlayerViewModel.ffPlayer.switchStatus()
        }
    }


    private fun updateMediaProgress() {
        lifecycleScope.launch {
            while(true) {
                delay(800)
                seekBar.progress = ffPlayerViewModel.ffPlayer.getProgress()
            }
        }
    }
}
