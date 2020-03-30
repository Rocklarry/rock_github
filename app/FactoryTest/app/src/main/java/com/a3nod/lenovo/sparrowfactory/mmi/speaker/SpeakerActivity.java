package com.a3nod.lenovo.sparrowfactory.mmi.speaker;

import android.app.Activity;
import android.content.Context;
import android.content.Intent;
import android.graphics.Color;
import android.media.AudioManager;
import android.media.MediaPlayer;
import android.os.Bundle;
import android.os.SystemClock;
import android.util.Log;
import android.view.View;
import android.widget.Button;
import android.widget.Toast;

import com.a3nod.lenovo.sparrowfactory.CrashHandler;
import com.a3nod.lenovo.sparrowfactory.mmi.MMITestProcessManager;
import com.a3nod.lenovo.sparrowfactory.mmi.result.ASSYEntity;
import com.a3nod.lenovo.sparrowfactory.CrashHandler;
import com.a3nod.lenovo.sparrowfactory.mmi.MMITestProcessManager;
import com.a3nod.lenovo.sparrowfactory.mmi.result.ASSYEntity;
import com.a3nod.lenovo.sparrowfactory.CrashHandler;
import com.a3nod.lenovo.sparrowfactory.R;
import com.a3nod.lenovo.sparrowfactory.mmi.MMITestProcessManager;
import com.a3nod.lenovo.sparrowfactory.mmi.result.ASSYEntity;
import com.a3nod.lenovo.sparrowfactory.tool.LogUtil;

import java.util.Random;

/**
 * Created by Lenovo on 2017/11/4.
 */

public class SpeakerActivity extends Activity
{
    public static final String TAG = "MMI_SPEAKER_TEST";
    AudioManager mAudioManager;
    MediaPlayer mMediaPlayer;
    int currentMusic = 0;
    int[] musicID = {R.raw.audio_left_test, R.raw.audio_right_test};
    private int Intervals = 500;
    private long currenttime = 1200;
    private long lasttime = 0;
    private  int  times = 0;
    private Boolean leftFlag = false ;   //左声道标志位
    private Boolean rightFlag = false;   // 有声道标志位
    private Button shot, piano , applause;

    @Override
    protected void onCreate(Bundle savedInstanceState)
    {
        CrashHandler.getInstance().setCurrentTag(TAG, CrashHandler.TYPE_MMI);
        super.onCreate(savedInstanceState);
        currentMusic = new Random().nextInt(3);
        setContentView(R.layout.activity_speaker);
        initConfirmButton();
    }

    public void playMusic(int i)
    {
        currentMusic = i ;
        LogUtil.d(TAG, "currentMusic =" + currentMusic);
        if (currentMusic < 0 || currentMusic > 2)
        {
            currentMusic = 0;
        }
        mAudioManager = (AudioManager) getApplicationContext().getSystemService(Context.AUDIO_SERVICE);
        //设置扬声器播放
        mAudioManager.setMode(AudioManager.MODE_NORMAL);
        //设置声音大小
        mAudioManager.setStreamVolume(AudioManager.STREAM_MUSIC, mAudioManager.getStreamMaxVolume(AudioManager.STREAM_MUSIC) * 2 / 5, 0);
        if (mMediaPlayer == null)
        {
            LogUtil.i("speakerAndRecord() mMediaPlayer is Null");
            mMediaPlayer = MediaPlayer.create(getApplicationContext(), musicID[currentMusic]);
            mMediaPlayer.setAudioStreamType(AudioManager.STREAM_MUSIC);
            mMediaPlayer.setVolume(1f, 1f);
            //对文件进行循环播放
//            mMediaPlayer.setLooping(true);
            mMediaPlayer.start();
        } else
        {
            LogUtil.i("speakerAndRecord() mMediaPlayer is not Null");
            mMediaPlayer.stop();
            mMediaPlayer = MediaPlayer.create(getApplicationContext(), musicID[currentMusic]);
            mMediaPlayer.setAudioStreamType(AudioManager.STREAM_MUSIC);
            mMediaPlayer.setVolume(1f, 1f);
            //对文件进行循环播放
//            mMediaPlayer.setLooping(true);
            mMediaPlayer.start();
        }

        lasttime = System.currentTimeMillis();
        times++;


        if(leftFlag && rightFlag){
            shot.setEnabled(true);
        }

    }

    public void initConfirmButton()
    {

        Button noSound = findViewById(R.id.noSound);
        noSound.setOnClickListener(new View.OnClickListener()
        {
            @Override
            public void onClick(View v)
            {
                LogUtil.d(TAG, "noSound click . currentMusic = " + currentMusic);
                ASSYEntity.getInstants().setSpeakerTestResult(false);
                MMITestProcessManager.getInstance().testFail();
                finish();
            }
        });

        applause = findViewById(R.id.applause);
        applause.setOnClickListener(new View.OnClickListener()
        {
            @Override
            public void onClick(View v)
            {

                leftFlag = true;
                applause.setBackgroundColor(Color.GREEN);

                if(times != 0){

                    currenttime = System.currentTimeMillis();
                }
                if(currenttime - lasttime < Intervals){
                    Log.d(TAG,"点击间隔时间太短000");
                    Toast.makeText(getApplicationContext(),getString(R.string.click_too_frequently),Toast.LENGTH_SHORT).show();
                    return;
                }
                piano.setBackgroundColor(Color.LTGRAY);
                playMusic(0);
//                applause.setBackgroundColor(Color.WHITE);

            }
        });

         piano = findViewById(R.id.piano);
        piano.setOnClickListener(new View.OnClickListener()
        {
            @Override
            public void onClick(View v)
            {

                rightFlag = true;
                piano.setBackgroundColor(Color.GREEN);
                if(times != 0){

                    currenttime = System.currentTimeMillis();
                }
                if(currenttime - lasttime < Intervals){
                    Log.d(TAG,"点击间隔时间太短111");
                    Toast.makeText(getApplicationContext(),getString(R.string.click_too_frequently),Toast.LENGTH_SHORT).show();
                    return;
                }
                applause.setBackgroundColor(Color.LTGRAY);
                playMusic(1);
            }
        });

        shot = findViewById(R.id.shot);
        shot.setEnabled(false);
        shot.setOnClickListener(new View.OnClickListener()
        {
            @Override
            public void onClick(View v)
            {
                if ( (mMediaPlayer != null) && mMediaPlayer.isPlaying() )
                {
                    LogUtil.i("mMediaPlayer  stop() and release()");
                    mMediaPlayer.stop();
                    mMediaPlayer.release();
                    mMediaPlayer = null;
                }
                    ASSYEntity.getInstants().setSpeakerTestResult(true);
                    MMITestProcessManager.getInstance().toNextTest();
                    finish();
                // add by penghuijun to start next activity
            }
        });
    }


    @Override
    protected void onResume() {
        super.onResume();
    }

    @Override
    protected void onDestroy()
    {

        if ( (mMediaPlayer != null) && mMediaPlayer.isPlaying() )
        {
            LogUtil.i("mMediaPlayer  stop() and release()");
            mMediaPlayer.stop();
            mMediaPlayer.release();
            mMediaPlayer = null;
        }
        super.onDestroy();

    }
}
