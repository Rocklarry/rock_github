package com.a3nod.lenovo.sparrowfactory.runin.audio;

import android.annotation.SuppressLint;
import android.content.Context;
import android.media.AudioManager;
import android.media.MediaPlayer;
import android.media.MediaRecorder;
import android.os.Bundle;
import android.os.Environment;
import android.os.Handler;
import android.os.Message;
import android.util.Log;
import android.widget.TextView;

import com.a3nod.lenovo.sparrowfactory.Constant;
import com.a3nod.lenovo.sparrowfactory.CrashHandler;
import com.a3nod.lenovo.sparrowfactory.R;
import com.a3nod.lenovo.sparrowfactory.runin.RuninBaseActivity;
import com.a3nod.lenovo.sparrowfactory.runin.RuninConfig;
import com.a3nod.lenovo.sparrowfactory.tool.LogUtil;
import com.a3nod.lenovo.sparrowfactory.tool.SPUtils;

import java.io.File;
import java.io.FileInputStream;
import java.io.IOException;
import java.lang.ref.WeakReference;
import java.util.ArrayList;
import java.util.Random;

public class AudioTestActivity extends RuninBaseActivity
{

    public static final String TAG = "AudioTestActivity";

    private AudioManager mAudioManager;
    private MediaPlayer mMediaPlayer;
    private MediaRecorder mMediaRecorder;

    private TextView audioTestTv;

    private String mAudioFilePath;
    private boolean isRecording = false;


    private static final int delay = 1000;
    private static int recordTime = 7 * 1000;

    private static final int STOP_MAIN_MIC_RECORD = 9;
    private static final int PLAY_MAIN_MIC_RECCORD = 10;
    private static final int STOP_PLAY_MAIN_MIC_RECORD = 11;
    private static final int MAX_RECORD_TIME = 60 * 1000;

    private ArrayList<Integer> mMaxAmplitudeList = new ArrayList<>();

    private int settingTime = 0;
    private Random ran = new Random();
    AudioHandler mHandler;

    @Override
    public void onCreate(Bundle savedInstanceState)
    {
        super.onCreate(savedInstanceState);

        setContentView(R.layout.activity_runin_audio);

        CrashHandler.getInstance().setCurrentTag(TAG, CrashHandler.TYPE_RUNIN);
        testTime = config.getItemDuration(RuninConfig.RUNIN_AUDIO_ID);
        settingTime = testTime / 2;

        if (settingTime < MAX_RECORD_TIME)
        {
            recordTime = settingTime;
        } else
        {
            recordTime = MAX_RECORD_TIME;
        }
        recordTime = (recordTime < 6000) ? 6000 : recordTime;
        settingTime = settingTime - recordTime;

        LogUtil.i("test time : " + testTime + " record time " + recordTime + "  setting time : " + settingTime);

        mHandler = new AudioHandler(this);

        mAudioManager = (AudioManager) getApplicationContext().getSystemService(Context.AUDIO_SERVICE);
        audioTestTv = findViewById(R.id.audio_test_tv);
        if (isErrorRestart)
        {
            String errInfo = getIntent().getStringExtra(Constant.INTENT_KEY_ERROR_MSG);
            SPUtils.put(this, Constant.SP_KEY_AUDIO_TEST_REMARK, errInfo);
            mTestSuccess = false;
            mHandler.sendEmptyMessage(MSG_TEST_FINISH);
        } else
        {
            mHandler.sendEmptyMessage(MSG_START_TEST);
        }
    }

    @Override
    protected void onResume()
    {
        super.onResume();
    }


    @SuppressLint("HandlerLeak")
    static class AudioHandler extends Handler
    {
        WeakReference<AudioTestActivity> wra;

        public AudioHandler(AudioTestActivity activity)
        {
            wra = new WeakReference<AudioTestActivity>(activity);
        }

        public void handleMessage(Message msg)
        {
            if (wra.get() == null)
            {
                return;
            }
            switch (msg.what)
            {
                case MSG_START_TEST:
                    wra.get().speakerAndMainMicRecord();
                    break;
                case STOP_MAIN_MIC_RECORD:
                    wra.get().stopMediaPlayerAndMainRecord();
                    break;
                case PLAY_MAIN_MIC_RECCORD:
                    wra.get().mainMicRecordAndPlay();
                    break;
                case STOP_PLAY_MAIN_MIC_RECORD:
                    wra.get().stopMainRecordPlay();
                    break;
                case MSG_TEST_FINISH:
                    wra.get().recycle();
                    break;
                default:
                    break;
            }

        }
    }

    ;

    public void recycle()
    {

        if (settingTime > 0)
        {
            if (settingTime < MAX_RECORD_TIME)
            {
                recordTime = settingTime;
            } else
            {
                recordTime = MAX_RECORD_TIME;
            }
            recordTime = (recordTime < 6000) ? 6000 : recordTime;
            settingTime = settingTime - recordTime;
            LogUtil.i(" record time " + recordTime + "  setting time : " + settingTime);
            mHandler.sendEmptyMessage(MSG_START_TEST);
        } else
        {
            testFinish();
        }
    }

    public void testFinish()
    {
        if (mTestSuccess)
        {
            config.saveTestResult(Constant.SP_KEY_AUDIO_TEST_SUCCESS);
        } else
        {
            config.saveTestResult(Constant.SP_KEY_AUDIO_TEST_FAIL);
        }
        if (isAutoRunin)
        {
            toNextText();
            finish();
            LogUtil.d(" audio 测试完成  结果 : " + (mTestSuccess ? "成功" : "失败"));
         //   LogUtil.d(" audio 测试结束 前往音频测试  ");
        } else
        {
            audioTestTv.setText(audioTestTv.getText() + "\n" + getString(R.string.audio_test_end));
            LogUtil.d(" audio 测试完成  结果 : " + (mTestSuccess ? "成功" : "失败"));
        }
    }

    //扬声器播放音乐  并用主麦录音
    private void speakerAndMainMicRecord()
    {
        LogUtil.i(TAG, "  current record time : " + recordTime);

        LogUtil.i(TAG, "speakerAndRecord() AudioManager.MODE_NORMAL AudioSource.MIC");
        //设置扬声器播放
        mAudioManager.setMode(AudioManager.MODE_NORMAL);
        //设置声音大小
        mAudioManager.setStreamVolume(AudioManager.STREAM_MUSIC, mAudioManager.getStreamMaxVolume(AudioManager.STREAM_MUSIC) * 2 / 3, 0);
        audioTestTv.setText(R.string.audio_test_start);

        audioTestTv.setText(audioTestTv.getText() + "\n" + getString(R.string.playing_music));
        if (mMediaPlayer == null)
        {
            LogUtil.i(TAG, "speakerAndRecord() mMediaPlayer is Null");
            mMediaPlayer = MediaPlayer.create(getApplicationContext(), R.raw.musictest);
            mMediaPlayer.setVolume(1f, 1f);
            //对文件进行循环播放
            mMediaPlayer.setLooping(true);
            mMediaPlayer.start();
        } else
        {
            LogUtil.i(TAG, "speakerAndRecord() mMediaPlayer is not Null");
        }

        //进行主麦录音
        audioTestTv.setText(audioTestTv.getText() + "\n" + getString(R.string.speaker_play_mainmic_record));
        inRecord(MediaRecorder.AudioSource.MIC);
        mHandler.sendEmptyMessageDelayed(STOP_MAIN_MIC_RECORD, recordTime);
    }

    //停止扬声器播放和主麦录音
    private void stopMediaPlayerAndMainRecord()
    {
        if (mMediaPlayer.isPlaying())
        {
            LogUtil.i(TAG, "stopMediaPlayerAndMainRecord() mMediaPlayer  stop() and release()");
            mMediaPlayer.stop();
            mMediaPlayer.release();
            mMediaPlayer = null;
        }
        if (isRecording && mMediaRecorder != null)
        {
            mMediaRecorder.stop();
            mMediaRecorder.release();
            mMediaRecorder = null;
            LogUtil.i(TAG, "stopMediaPlayerAndMainRecord() MediaRecorder  stop()  release()");
            isRecording = false;
        }
        audioTestTv.setText(audioTestTv.getText() + "\n" + getString(R.string.end_speaker_player_mainmic_record));
        mHandler.sendEmptyMessageDelayed(PLAY_MAIN_MIC_RECCORD, delay);
    }

    //播放上一步的主麦录音
    private void mainMicRecordAndPlay()
    {
        LogUtil.i(TAG, "mainMicRecordAndPlay()");
        audioTestTv.setText(audioTestTv.getText() + "\n" + getString(R.string.mainmic_file_play));
        mAudioManager.setMode(AudioManager.MODE_NORMAL);
        mAudioManager.setStreamVolume(AudioManager.STREAM_MUSIC, mAudioManager.getStreamMaxVolume(AudioManager.STREAM_MUSIC), 0);
        try
        {
            replay();
        } catch (Exception e)
        {
            mTestSuccess = false;
            e.printStackTrace();
        }
        mHandler.sendEmptyMessageDelayed(STOP_PLAY_MAIN_MIC_RECORD, recordTime);

    }

    //主麦录音文件播放结束
    private void stopMainRecordPlay()
    {
        LogUtil.i(TAG, " stopMainRecordPlay()");
        if (mMediaPlayer != null && mMediaPlayer.isPlaying())
        {
            try
            {
                LogUtil.i(TAG, "stopMainRecordPlay() mMediaPlayer  stop() and release()");
                mMediaPlayer.stop();
                mMediaPlayer.release();
            } catch (IllegalStateException e)
            {
                e.printStackTrace();
            } catch (RuntimeException e)
            {
                e.printStackTrace();
            } catch (Exception e)
            {
                e.printStackTrace();
            }
            mMediaPlayer = null;
        }

        mTestSuccess = true;
        mHandler.sendEmptyMessage(MSG_TEST_FINISH);
        //        audioTestTv.setText(audioTestTv.getText() + "\n" + getString(R.string.end_mainmic_file_play));
        //        mHandler.sendEmptyMessageDelayed(DEPUTY_WHEAT_RECORD, delay);
    }


    //清空振幅集合
    private void clearArrayList()
    {
        mMaxAmplitudeList.clear();
    }


    /**
     * 播放
     *
     * @throws IllegalArgumentException
     * @throws IllegalStateException
     * @throws IOException
     */
    private void replay() throws IllegalArgumentException, IllegalStateException, IOException
    {
        File file = new File(mAudioFilePath);
        FileInputStream mFileInputStream = new FileInputStream(file);
        if (mMediaPlayer != null)
        {
            mMediaPlayer.reset();
            Log.d(TAG, "replay() mMediaPlayer != null");
        } else
        {
            mMediaPlayer = new MediaPlayer();
            Log.d(TAG, "replay()-----mMediaPlayer == null");
        }

        mMediaPlayer.setOnErrorListener(new MediaPlayer.OnErrorListener()
        {

            @Override
            public boolean onError(MediaPlayer player, int arg1, int arg2)
            {
                if (player != null && player.isPlaying())
                {
                    LogUtil.i(TAG, "replay() ------->" + "onError====mMediaPlayer");
                    player.stop();
                    LogUtil.i(TAG, "replay() ------->" + "onError====mMediaPlayer---stop()");
                    player.release();
                    LogUtil.i(TAG, "replay() ------->" + "onError====mMediaPlayer---release()");
                    player = null;
                }
                LogUtil.i(TAG, "replay() ------->" + "------->" + "onError");
                mTestSuccess = false;
                return false;
            }
        });

        mMediaPlayer.setDataSource(mFileInputStream.getFD());
        mMediaPlayer.prepare();
        mMediaPlayer.setOnPreparedListener(new MediaPlayer.OnPreparedListener()
        {
            @Override
            public void onPrepared(MediaPlayer mp)
            {
                LogUtil.i(TAG, "replay() MediaPlayer------->" + "onPrepared");
                mp.start();
                LogUtil.i(TAG, "replay() MediaPlayer------->" + "start");
            }
        });

        mMediaPlayer.setOnCompletionListener(new MediaPlayer.OnCompletionListener()
        {
            @Override
            public void onCompletion(MediaPlayer mPlayer)
            {
                LogUtil.i(TAG, "replay() MediaPlayer------->" + "onCompletion");
                mPlayer.stop();
                LogUtil.i(TAG, "replay() MediaPlayer------->" + "onCompletion===stop()");
                File file = new File(mAudioFilePath);
                file.delete();
                LogUtil.i(TAG, "replay() Delete audioFile------->");
            }
        });
    }

    /***
     * 进行录音
     * @param audioSource
     */
    private void inRecord(int audioSource)
    {
        LogUtil.i(TAG, "inRecord()");
        try
        {
            if (Environment.getExternalStorageState().equals(Environment.MEDIA_MOUNTED))
            {
                LogUtil.i(TAG, "Environment.getExternalStorageState():" + Environment.getExternalStorageState());
            }
            record(audioSource);
            isRecording = true;
        } catch (Exception e)
        {
            mTestSuccess = false;
            LogUtil.i(TAG, "inRecord() Exception:" + e);
        }
    }

    /**
     * 录音
     *
     * @param audioSource
     * @throws IllegalStateException
     * @throws IOException
     * @throws InterruptedException
     */
    private void record(int audioSource) throws IllegalStateException, IOException, InterruptedException
    {
        if (mMediaRecorder != null)
        {
            Log.d(TAG, "mMediaRecorder != null");
            mMediaRecorder.reset();
        } else
        {
            mMediaRecorder = new MediaRecorder();
        }

        mMediaRecorder.setAudioSource(audioSource);
        mMediaRecorder.setOutputFormat(MediaRecorder.OutputFormat.AAC_ADTS);
        mMediaRecorder.setAudioEncoder(MediaRecorder.AudioEncoder.AAC);
        mMediaRecorder.setOnErrorListener(new MediaRecorder.OnErrorListener()
        {

            @Override
            public void onError(MediaRecorder mediarecorder, int arg1, int arg2)
            {
                // TODO Auto-generated method stub
                LogUtil.i(TAG, "record() mMediaRecorder is onError");
                if (mediarecorder != null && isRecording)
                {
                    Log.d(TAG, "mMediaRecorder is xxxxxxx");
                    mediarecorder.stop();
                    mediarecorder.release();
                    LogUtil.i(TAG, "record() mMediaRecorder stop() release()");
                    mediarecorder = null;
                    mTestSuccess = false;
                }
            }
        });
        mMediaRecorder.setAudioChannels(2);
        mAudioFilePath = Environment.getExternalStorageDirectory().getAbsolutePath() + "/sparrow/testHeadset.aac";
        mMediaRecorder.setOutputFile(mAudioFilePath);
        LogUtil.i(TAG, "path :" + mAudioFilePath);
        mMediaRecorder.prepare();
        mMediaRecorder.start();

    }

    @Override
    protected void onPause()
    {
        super.onPause();
        mHandler.removeCallbacksAndMessages(null);
        if (mMediaPlayer != null)
        {
            mMediaPlayer.stop();
            mMediaPlayer.release();
            mMediaPlayer = null;
        }

        if (mAudioManager != null)
        {
            mAudioManager.setMode(AudioManager.MODE_NORMAL);
        }

        if (isRecording && mMediaRecorder != null)
        {
            mMediaRecorder.stop();
            mMediaRecorder.release();
            mMediaRecorder = null;
            Log.d(TAG, "------onStop----1-----");
        }
    }

}
