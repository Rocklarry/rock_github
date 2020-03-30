package com.a3nod.lenovo.sparrowfactory.runin.video;

import android.annotation.SuppressLint;
import android.content.Context;
import android.content.res.AssetManager;
import android.media.AudioManager;
import android.media.MediaPlayer;
import android.media.MediaPlayer.OnCompletionListener;
import android.media.MediaPlayer.OnErrorListener;
import android.media.MediaPlayer.OnInfoListener;
import android.media.MediaPlayer.OnPreparedListener;
import android.media.MediaPlayer.OnSeekCompleteListener;
import android.media.MediaPlayer.OnVideoSizeChangedListener;
import android.os.Bundle;
import android.os.Handler;
import android.os.Message;
import android.util.Log;
import android.view.Display;
import android.view.SurfaceHolder;
import android.view.SurfaceView;
import android.widget.Toast;

import com.a3nod.lenovo.sparrowfactory.Constant;
import com.a3nod.lenovo.sparrowfactory.CrashHandler;
import com.a3nod.lenovo.sparrowfactory.R;
import com.a3nod.lenovo.sparrowfactory.runin.RuninBaseActivity;
import com.a3nod.lenovo.sparrowfactory.runin.RuninConfig;
import com.a3nod.lenovo.sparrowfactory.tool.LogUtil;
import com.a3nod.lenovo.sparrowfactory.tool.SPUtils;

import java.io.IOException;
import java.lang.ref.WeakReference;


public class VideoTestActivity extends RuninBaseActivity implements
        OnCompletionListener, OnErrorListener, OnInfoListener,
        OnPreparedListener, OnSeekCompleteListener, OnVideoSizeChangedListener,
        SurfaceHolder.Callback
{
    public static final String TAG = "VideoTestActivity";
    private Display currDisplay;
    private SurfaceView surfaceView;
    private SurfaceHolder holder;
    private MediaPlayer player;
    private int vWidth, vHeight;
    private AudioManager audioManager;
    VideoHandler handler;

    public void onCreate(Bundle savedInstanceState)
    {
        super.onCreate(savedInstanceState);

        CrashHandler.getInstance().setCurrentTag(TAG, CrashHandler.TYPE_RUNIN);

        setContentView(R.layout.activity_runin_video);
        testTime = config.getItemDuration(RuninConfig.RUNIN_VIDEO_ID);
        LogUtil.i("test time : " + testTime);
        handler=new VideoHandler(this);
        if (isErrorRestart)
        {
            String errInfo = getIntent().getStringExtra(Constant.INTENT_KEY_ERROR_MSG);
            SPUtils.put(this, Constant.SP_KEY_VIDEO_TEST_REMARK, errInfo);
            mTestSuccess = false;
            handler.sendEmptyMessage(MSG_TEST_FINISH);
        } else
        {
            prepareVideo();
        }

    }

    private void prepareVideo()
    {
        surfaceView = findViewById(R.id.video_surface);
        holder = surfaceView.getHolder();
        holder.addCallback(this);
        holder.setType(SurfaceHolder.SURFACE_TYPE_PUSH_BUFFERS);

        audioManager = (AudioManager) this.getSystemService(Context.AUDIO_SERVICE);
        audioManager.setMode(AudioManager.MODE_NORMAL);
        audioManager.setStreamVolume(AudioManager.STREAM_MUSIC, audioManager.getStreamMaxVolume(AudioManager.STREAM_MUSIC) /2, 0);


        player = new MediaPlayer();
        player.setOnCompletionListener(this);
        player.setOnErrorListener(this);
        player.setOnInfoListener(this);
        player.setOnPreparedListener(this);
        player.setOnSeekCompleteListener(this);
        player.setOnVideoSizeChangedListener(this);


        String exception = null;
        AssetManager am = getAssets();
        try
        {
            player.setDataSource(am.openFd("aaa.mp4").getFileDescriptor(),
                    am.openFd("aaa.mp4").getStartOffset(),
                    am.openFd("aaa.mp4").getLength());
        } catch (IllegalArgumentException e)
        {
            exception = Log.getStackTraceString(e);
        } catch (IllegalStateException e)
        {
            exception = Log.getStackTraceString(e);
        } catch (IOException e)
        {
            exception = Log.getStackTraceString(e);
        }
        currDisplay = this.getWindowManager().getDefaultDisplay();


        if (null != exception)
        {
            mTestSuccess = false;
            handler.sendEmptyMessage(MSG_TEST_FINISH);
        } else
        {
            mTestSuccess = true;
            handler.sendEmptyMessageDelayed(MSG_TEST_FINISH, testTime);
        }
    }

    @Override
    public void surfaceChanged(SurfaceHolder arg0, int arg1, int arg2, int arg3)
    {
        LogUtil.i(TAG, "surfaceChanged called");
    }

    @Override
    public void surfaceCreated(SurfaceHolder holder)
    {
        player.setDisplay(holder);
        player.prepareAsync();
    }

    @Override
    public void surfaceDestroyed(SurfaceHolder holder)
    {
        LogUtil.i(TAG, "surfaceDestroyed called");
    }

    @Override
    public void onVideoSizeChanged(MediaPlayer arg0, int arg1, int arg2)
    {
        LogUtil.i(TAG, "onVideoSizeChanged called");
    }

    @Override
    public void onSeekComplete(MediaPlayer arg0)
    {
        LogUtil.i(TAG, "onSeekComplete called");
    }

    @Override
    public void onPrepared(MediaPlayer player)
    {
        vWidth = player.getVideoWidth();
        vHeight = player.getVideoHeight();
        if (vWidth > currDisplay.getWidth() || vHeight > currDisplay.getHeight())
        {
            float wRatio = (float) vWidth / (float) currDisplay.getWidth();
            float hRatio = (float) vHeight / (float) currDisplay.getHeight();

            float ratio = Math.max(wRatio, hRatio);

            vWidth = (int) Math.ceil((float) vWidth / ratio);
            vHeight = (int) Math.ceil((float) vHeight / ratio);
//            surfaceView.setLayoutParams(new LinearLayout.LayoutParams(vWidth,vHeight));
            player.setLooping(true);
            player.start();
        } else
        {
//            surfaceView.setLayoutParams(new LinearLayout.LayoutParams(currDisplay.getWidth(),currDisplay.getHeight()));
            player.setLooping(true);
            player.start();
        }
    }

    @Override
    public boolean onInfo(MediaPlayer player, int whatInfo, int extra)
    {
        LogUtil.i(TAG, "whatInfo " + whatInfo);
        switch (whatInfo)
        {
            case MediaPlayer.MEDIA_INFO_BAD_INTERLEAVING:
                break;
            case MediaPlayer.MEDIA_INFO_METADATA_UPDATE:
                break;
            case MediaPlayer.MEDIA_INFO_VIDEO_TRACK_LAGGING:
                break;
            case MediaPlayer.MEDIA_INFO_NOT_SEEKABLE:
                break;
        }
        return false;
    }

    @Override
    public boolean onError(MediaPlayer player, int whatError, int extra)
    {
        LogUtil.i(TAG, "whatError " + whatError);
        switch (whatError)
        {
            case MediaPlayer.MEDIA_ERROR_SERVER_DIED:
                player.reset();
                break;
            case MediaPlayer.MEDIA_ERROR_UNKNOWN:
                break;
            default:
                break;
        }
        return false;
    }

    @Override
    public void onCompletion(MediaPlayer player)
    {
        LogUtil.d("video test finish ");
    }


    @SuppressLint("HandlerLeak")
    static class VideoHandler extends Handler
    {
        WeakReference<VideoTestActivity> wra;

        public VideoHandler(VideoTestActivity activity)
        {
            wra = new WeakReference<VideoTestActivity>(activity);
        }

        public void handleMessage(Message msg)
        {
            super.handleMessage(msg);
            if (wra.get() == null)
            {
                return;
            }
            switch (msg.what)
            {
                case MSG_TEST_FINISH:
                    wra.get().videoTestFinish();
                    break;
            }
        }
    }

    private void videoTestFinish()
    {
        if (mTestSuccess)
        {
            config.saveTestResult(Constant.SP_KEY_VIDEO_TEST_SUCCESS);
        } else
        {
            config.saveTestResult(Constant.SP_KEY_VIDEO_TEST_FAIL);
        }
        if (isAutoRunin)
        {
            toNextText();
            finish();
            LogUtil.d(" video 测试完成  结果 : " + (mTestSuccess ? "成功" : "失败"));
            LogUtil.d(" video 测试结束 前往相机测试  ");
        } else
        {
            Toast.makeText(this, "video test " + (mTestSuccess ? "success" : "fail"), Toast.LENGTH_LONG).show();
            LogUtil.d(" video 测试完成  结果 : " + (mTestSuccess ? "成功" : "失败"));
            if (player.isPlaying())
            {
                player.stop();
            }
        }
    }

    public void onResume()
    {
        super.onResume();
    }

    @Override
    protected void onPause()
    {
        super.onPause();
        if (player != null)
        {
            player.release();
        }
        handler.removeCallbacksAndMessages(null);
    }

    @Override
    protected void onDestroy()
    {
        super.onDestroy();
    }
}
