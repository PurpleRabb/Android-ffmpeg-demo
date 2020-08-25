package com.example.ffmpegdemo

import androidx.appcompat.app.AppCompatActivity
import android.os.Bundle
import android.util.Log
import android.view.View
import android.widget.SeekBar
import android.widget.SeekBar.OnSeekBarChangeListener
import androidx.lifecycle.Observer
import androidx.lifecycle.ViewModelProvider
import kotlinx.android.synthetic.main.activity_main.*

class MainActivity : AppCompatActivity() {

    private var dataSource = "someplaces"
    lateinit var ffPlayerViewModel : FFPlayerViewModel
    private val TAG = "ffmplayer"
    private lateinit var ffPlayer : FFPlayer
    override fun onCreate(savedInstanceState: Bundle?) {
        super.onCreate(savedInstanceState)
        setContentView(R.layout.activity_main)
//        galleryViewModel = ViewModelProvider(
//            requireActivity(),
//            SavedStateViewModelFactory(requireActivity().application, this)
//        ).get(GalleryViewModel::class.java)
        ffPlayerViewModel = ViewModelProvider(this, ViewModelProvider.AndroidViewModelFactory(this.application)).get(FFPlayerViewModel::class.java)
        ffPlayer = FFPlayer()
        ffPlayerViewModel.setPlayer(ffPlayer)

        seekBar.setOnSeekBarChangeListener( object : OnSeekBarChangeListener {
            override fun onProgressChanged(seekBar: SeekBar?, progress: Int, fromUser: Boolean) {

            }

            override fun onStartTrackingTouch(seekBar: SeekBar?) {

            }

            override fun onStopTrackingTouch(seekBar: SeekBar?) {

            }
        })

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

        controlButton.setOnClickListener { View.OnClickListener { ffPlayer.nativePrepare(dataSource) } }
        ffPlayer.nativePrepare(dataSource)
    }
}
