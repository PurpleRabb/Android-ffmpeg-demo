package com.example.ffmpegdemo

import android.app.Application
import androidx.lifecycle.AndroidViewModel
import androidx.lifecycle.LiveData
import androidx.lifecycle.MutableLiveData

class FFPlayerViewModel(application: Application) : AndroidViewModel(application) {
    private var _error : MutableLiveData<Int> = MutableLiveData()
    private var _progress : MutableLiveData<Int> = MutableLiveData()
    private lateinit var _ffPlayer : FFPlayer

    var error : LiveData<Int> = _error
    var progress : LiveData<Int> = _progress

    fun setPlayer(ffPlayer: FFPlayer) {
        this._ffPlayer = ffPlayer
        _ffPlayer.setOnPrepareListener(object : FFPlayer.OnPrepareListener {
            override fun onPrepare() {

            }
        })

        _ffPlayer.setOnErrorListener(object : FFPlayer.OnErrorListener {
            override fun onError(error: Int) {
                _error.postValue(error)
            }
        })
    }
}