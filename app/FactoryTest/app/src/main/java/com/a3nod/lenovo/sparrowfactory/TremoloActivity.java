package com.a3nod.lenovo.sparrowfactory;

import android.app.Activity;
import android.content.BroadcastReceiver;
import android.content.Context;
import android.content.Intent;
import android.content.IntentFilter;
import android.media.AudioManager;
import android.media.MediaPlayer;
import android.os.Bundle;
import android.util.Log;
import android.view.View;
import android.widget.ImageView;
import android.widget.TextView;

public class TremoloActivity extends Activity {
    private TextView mTextView;
    private MediaPlayer mediaPlayer;
    private BroadcastReceiver mBroadcastReceiver=new BroadcastReceiver() {
        @Override
        public void onReceive(Context context, Intent intent) {
            AudioManager am=(AudioManager)getSystemService(Context.AUDIO_SERVICE);
//            Toast.makeText(getApplicationContext(),"onReceive"+am.getStreamVolume(AudioManager.STREAM_MUSIC),Toast.LENGTH_SHORT).show();
            mTextView.setText("Current Volume==>"+am.getStreamVolume(AudioManager.STREAM_MUSIC));
        }
    };

    public void initTitle()
    {
        TextView tv_title=findViewById(R.id.tv_menu_title);
        tv_title.setText("Tremolo-test");
        ImageView iv_back=findViewById(R.id.iv_menu_back);
        iv_back.setOnClickListener(new View.OnClickListener()
        {
            @Override
            public void onClick(View v)
            {
                finish();
            }
        });
    }
    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_tremolo);
        mTextView=findViewById(R.id.tv_1);
        initTitle();
        IntentFilter intentFilter = new IntentFilter();
        //设置接收广播的类型
        intentFilter.addAction("android.media.VOLUME_CHANGED_ACTION");
        //调用Context的registerReceiver（）方法进行动态注册
        registerReceiver(mBroadcastReceiver, intentFilter);
        AudioManager am=(AudioManager)getSystemService(Context.AUDIO_SERVICE);
        Log.v("TAG",""+am.getStreamMaxVolume(AudioManager.STREAM_MUSIC)+"====>"+am.getStreamVolume(AudioManager.STREAM_MUSIC));
        am.setStreamVolume(AudioManager.STREAM_MUSIC,6,AudioManager.FLAG_PLAY_SOUND);
        findViewById(R.id.bn1).setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View view) {
                if(mediaPlayer!=null&&mediaPlayer.isPlaying()){
                    mediaPlayer.stop();
                    mediaPlayer.release();
                    mediaPlayer=null;
                }
                mediaPlayer= MediaPlayer.create(TremoloActivity.this, R.raw.sweeptest);
                mediaPlayer.start();

            }
        });
        findViewById(R.id.bn2).setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View view) {
                if(mediaPlayer!=null&&mediaPlayer.isPlaying()){
                    mediaPlayer.stop();
                    mediaPlayer.release();
                    mediaPlayer=null;
                }
                mediaPlayer= MediaPlayer.create(TremoloActivity.this, R.raw.mixmusic);
                mediaPlayer.start();
            }
        });
    }

    @Override
    protected void onDestroy()
    {
        super.onDestroy();
        if(mediaPlayer!=null){
            if (mediaPlayer.isPlaying())
            {
                mediaPlayer.stop();
                mediaPlayer.release();
                mediaPlayer = null;

            }
        }
    }
}
