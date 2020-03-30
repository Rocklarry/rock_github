package com.a3nod.lenovo.sparrowfactory.pcba;

import android.content.Context;
import android.graphics.Color;
import android.media.AudioManager;
import android.media.MediaPlayer;
import android.os.Bundle;
import android.util.Log;
import android.view.View;
import android.widget.Button;
import android.widget.TextView;
import android.widget.Toast;

import com.a3nod.lenovo.sparrowfactory.CrashHandler;
import com.a3nod.lenovo.sparrowfactory.interact.ConnectManager;
import com.a3nod.lenovo.sparrowfactory.interact.Protocol;
import com.a3nod.lenovo.sparrowfactory.CrashHandler;
import com.a3nod.lenovo.sparrowfactory.interact.ConnectManager;
import com.a3nod.lenovo.sparrowfactory.interact.Protocol;
import com.a3nod.lenovo.sparrowfactory.CrashHandler;
import com.a3nod.lenovo.sparrowfactory.R;
import com.a3nod.lenovo.sparrowfactory.interact.ConnectManager;
import com.a3nod.lenovo.sparrowfactory.interact.Protocol;
import com.a3nod.lenovo.sparrowfactory.tool.LogUtil;

import java.util.Random;

public class PCBASpeakerTestActivity extends PCBABaseActivity
{
    public static final String TAG = "pcba_speaker_test";
    private AudioManager mAudioManager;
    private MediaPlayer mMediaPlayer;
    TextView audioTestTv;
    private Context mContext = null;
    int currentMusic = 0;
    int[] musicID = {R.raw.audio_right_test, R.raw.audio_left_test, R.raw.audio_hij};
    private int Intervals = 500;
    private long currenttime = 1200;
    private long lasttime = 0;
    private  int  times = 0;
    private boolean[] flag ={false, false};
    private Button shot, piano, applause;

    @Override
    protected void onCreate(Bundle savedInstanceState)
    {
        CrashHandler.getInstance().setCurrentTag(TAG, CrashHandler.TYPE_PCBA);

        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_pcbaspeaker_test);
        mContext = getApplicationContext();
        audioTestTv = findViewById(R.id.tv_speaker_status);
        audioTestTv.setText(audioTestTv.getText());
        currentMusic = new Random().nextInt(2);
        initConfirmButton();
    }

    public void initConfirmButton()
    {

        Button noSound = findViewById(R.id.noSound);
        noSound.setOnClickListener(new View.OnClickListener()
        {
            @Override
            public void onClick(View v)
            {
                ConnectManager.getInstance().setPCBAResult(Protocol.PCBA_TEST_ITEM_SPEAKER, false, "007");
                finish();
            }
        });

        applause = findViewById(R.id.applause);
        applause.setOnClickListener(new View.OnClickListener()
        {
            @Override
            public void onClick(View v)
            {
                flag[0] = true;
                applause.setBackgroundColor(Color.GREEN);
                LogUtil.d(TAG, "applause click . currentMusic = " + currentMusic + " result=" + (currentMusic == 0));
                if(times != 0){

                    currenttime = System.currentTimeMillis();
                }
                if(currenttime - lasttime < Intervals){
                    Log.d(TAG,"点击间隔时间太短000");
                    Toast.makeText(getApplicationContext(),getString(R.string.click_too_frequently),Toast.LENGTH_SHORT).show();
                    return;
                }

                piano.setBackgroundColor(Color.LTGRAY);
                playMusic(1);
            }
        });

         piano = findViewById(R.id.piano);
        piano.setOnClickListener(new View.OnClickListener()
        {
            @Override
            public void onClick(View v)
            {

                piano.setBackgroundColor(Color.GREEN);
                flag[1] = true;
                LogUtil.d(TAG, "piano click . currentMusic = " + currentMusic + " result=" + (currentMusic == 1));
                if(times != 0){

                    currenttime = System.currentTimeMillis();
                }
                if(currenttime - lasttime < Intervals){
                    Log.d(TAG,"点击间隔时间太短500");
                    Toast.makeText(getApplicationContext(),getString(R.string.click_too_frequently),Toast.LENGTH_SHORT).show();
                    return;
                }
                applause.setBackgroundColor(Color.LTGRAY);
                playMusic(0);
              //  ConnectManager.getInstance().setPCBAResult(Protocol.PCBA_TEST_ITEM_SPEAKER, currentMusic == 1, "007");
               // finish();
            }
        });

        shot = findViewById(R.id.shot);
        shot.setEnabled(false);
        shot.setOnClickListener(new View.OnClickListener()
        {
            @Override
            public void onClick(View v)
            {
                Log.d(TAG, "onClick Pass");
                ConnectManager.getInstance().setPCBAResult(Protocol.PCBA_TEST_ITEM_SPEAKER, true, "007");
                finish();
            }
        });
    }

    public void playMusic(int i)
    {
        currentMusic = i;
        LogUtil.d(TAG, "currentMusic =" + currentMusic);
        if (currentMusic < 0 || currentMusic > 2)
        {
            currentMusic = 0;
        }
        mAudioManager = (AudioManager) getApplicationContext().getSystemService(AUDIO_SERVICE);
        //设置扬声器播放
        mAudioManager.setMode(AudioManager.MODE_NORMAL);
        //设置声音大小
        mAudioManager.setStreamVolume(AudioManager.STREAM_MUSIC, mAudioManager.getStreamMaxVolume(AudioManager.STREAM_MUSIC) * 3 / 5, 0);
        if (mMediaPlayer == null)
        {

            LogUtil.i("speakerAndRecord() mMediaPlayer is Null");
            mMediaPlayer = MediaPlayer.create(getApplicationContext(), musicID[currentMusic]);
            mMediaPlayer.setAudioStreamType(AudioManager.STREAM_MUSIC);
            mMediaPlayer.setVolume(1f, 1f);
            //对文件进行循环播放
          //  mMediaPlayer.setLooping(true);
            mMediaPlayer.start();
        } else
        {
            mMediaPlayer.stop();
            mMediaPlayer.release();
            mMediaPlayer = null;
            mMediaPlayer = MediaPlayer.create(getApplicationContext(), musicID[currentMusic]);
            mMediaPlayer.setAudioStreamType(AudioManager.STREAM_MUSIC);
            mMediaPlayer.setVolume(1f, 1f);
            //对文件进行循环播放
            //  mMediaPlayer.setLooping(true);
            mMediaPlayer.start();
            LogUtil.i("speakerAndRecord() mMediaPlayer is not Null");
        }

        lasttime = System.currentTimeMillis();
        times++;

        if(flag[0] == true && flag[1] == true){
            shot.setEnabled(true);
        }

    }

    @Override
    protected void onDestroy()
    {
        super.onDestroy();
        if ((mMediaPlayer != null) && mMediaPlayer.isPlaying())
        {
            LogUtil.i("mMediaPlayer  stop() and release()");
            mMediaPlayer.stop();
            mMediaPlayer.release();
            mMediaPlayer = null;
        }
    }
}
