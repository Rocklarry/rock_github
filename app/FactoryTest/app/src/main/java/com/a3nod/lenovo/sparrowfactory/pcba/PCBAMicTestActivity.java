package com.a3nod.lenovo.sparrowfactory.pcba;

import android.annotation.SuppressLint;
import android.content.Context;
import android.graphics.Color;
import android.media.AudioManager;
import android.media.MediaRecorder;
import android.os.Bundle;
import android.os.Environment;
import android.os.Handler;
import android.os.Message;
import android.os.PersistableBundle;
import android.support.annotation.Nullable;
import android.util.Log;
import android.view.Menu;
import android.view.View;
import android.widget.Button;
import android.widget.ImageView;
import android.widget.TextView;

import com.a3nod.lenovo.sparrowfactory.Constant;
import com.a3nod.lenovo.sparrowfactory.CrashHandler;
import com.a3nod.lenovo.sparrowfactory.R;
import com.a3nod.lenovo.sparrowfactory.interact.ConnectManager;
import com.a3nod.lenovo.sparrowfactory.interact.Protocol;
import com.a3nod.lenovo.sparrowfactory.mmi.MMITestProcessManager;
import com.a3nod.lenovo.sparrowfactory.mmi.result.ASSYEntity;
import com.a3nod.lenovo.sparrowfactory.tool.MicControl;
import com.a3nod.lenovo.sparrowfactory.tool.LogUtil;

import java.io.File;
import java.io.IOException;
import java.util.Timer;
import java.util.TimerTask;

public class PCBAMicTestActivity extends PCBABaseActivity
implements View.OnClickListener{

    public static final String TAG = "pcba_mic_test";
    private AudioManager mAudioManager;
    private MediaRecorder mMediaRecorder;
    private ImageView iv_audio;
    private String mAudioFilePath;
    private boolean isRecording = false;
    private static final int MSG_START_TEST = 0;
    private static final int MSG_VOICE_CHANGE = 11;
    private static final int END_TIME = 3 * 1000;
    private static final int MIC_TIME = 8 * 1000;
    private static final int STOP_MIC_RECORD = 9;
    private static final int MAINMICRECORDANDPLAY = 10;


    private Context mContext = null;
    private MicControl micControl;
   // private int testModel = Constant.INTENT_VALUE_TEST_MODEL_PCBA;
    private int testModel = Constant.INTENT_VALUE_TEST_MODEL_MMI;


    private Timer timer = null;
    private TimerTask myTimerTask = null;
    private int times = 0;
    private int flag = 0;
    private Button bt_mic1,bt_mic2,tv_pass;
    private TextView tv_mic1,tv_mic2;
    private boolean autoTestFlag = false;
    @Override
    public void onCreate(Bundle savedInstanceState)
    {

        CrashHandler.getInstance().setCurrentTag(TAG, CrashHandler.TYPE_PCBA);
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_pcbamic_test);

        if(getIntent().getExtras()!=null){
            if (Constant.INTENT_VALUE_TEST_MODEL_MMI==(int)getIntent().getExtras().get(Constant.INTENT_KEY_TEST_MODEL)){
                autoTestFlag = true;
            }
        }
       // testModel = getIntent().getIntExtra(Constant.INTENT_KEY_TEST_MODEL, Constant.INTENT_VALUE_TEST_MODEL_PCBA);
        testModel = getIntent().getIntExtra(Constant.INTENT_KEY_TEST_MODEL, Constant.INTENT_VALUE_TEST_MODEL_MMI);
        if(testModel== Constant.INTENT_VALUE_TEST_MODEL_PCBA){
            autoTestFlag=true;
        }

        mAudioManager = (AudioManager) getApplicationContext().getSystemService(Context.AUDIO_SERVICE);
        micControl = new MicControl();
        micControl.switchMic(false);
        mContext = this;

        mContext = getApplicationContext();
        initConfirmButton();
        initView();
        registerOnClickEvents();
        mHandler.sendEmptyMessage(MSG_START_TEST);
    }

    private void registerOnClickEvents() {
        bt_mic1.setOnClickListener(this);
        bt_mic2.setOnClickListener(this);
//        bt_mic3.setOnClickListener(this);
//        bt_mic4.setOnClickListener(this);

    }

    public void initView(){
        iv_audio = findViewById(R.id.iv_audio);
        //tv_mic_show = findViewById(R.id.mic_icon);
        //tv_test_icon = findViewById(R.id.test_icon);

        bt_mic1 = findViewById(R.id.mic1);
        bt_mic2 = findViewById(R.id.mic2);
//        bt_mic3 = findViewById(R.id.mic3);
//        bt_mic4 = findViewById(R.id.mic4);

        bt_mic2.setEnabled(false);
//        bt_mic3.setEnabled(false);
//        bt_mic4.setEnabled(false);

        tv_mic1 = findViewById(R.id.tv_mic1);
        tv_mic2 = findViewById(R.id.tv_mic2);
//        tv_mic3 = findViewById(R.id.tv_mic3);
//        tv_mic4 = findViewById(R.id.tv_mic4);


    }
    public void initConfirmButton()
    {
        tv_pass = findViewById(R.id.btn_result_pass);
        tv_pass.setEnabled(false);
        tv_pass.setOnClickListener(new View.OnClickListener()
        {
            @Override
            public void onClick(View v)
            {
                mHandler.sendEmptyMessage(STOP_MIC_RECORD);
                if (testModel == Constant.INTENT_VALUE_TEST_MODEL_MMI)
                {
                    ASSYEntity.getInstants().setMicTestResult(true);
                    MMITestProcessManager.getInstance().toNextTest();
                }else if(testModel == Constant.INTENT_VALUE_TEST_MODEL_PCBA){
                    ConnectManager.getInstance().setPCBAResult(Protocol.PCBA_TEST_ITEM_MIC,true,"007");
                }
                finish();
            }
        });
        Button tv_reset = findViewById(R.id.btn_result_reset);
        tv_reset.setVisibility(View.GONE);
        tv_reset.setOnClickListener(new View.OnClickListener()
        {
            @Override
            public void onClick(View v)
            {
                tv_pass.setEnabled(false);
            }
        });
        Button tv_fail = findViewById(R.id.btn_result_fail);
        tv_fail.setOnClickListener(new View.OnClickListener()
        {
            @Override
            public void onClick(View v)
            {
                mHandler.sendEmptyMessage(STOP_MIC_RECORD);
                if (testModel == Constant.INTENT_VALUE_TEST_MODEL_MMI)
                {    Log.e("TAG","MMI");
                    ASSYEntity.getInstants().setMicTestResult(false);
                    MMITestProcessManager.getInstance().testFail();
                }else if(testModel == Constant.INTENT_VALUE_TEST_MODEL_PCBA)
                {
                    Log.e("TAG","pcba");
                    ConnectManager.getInstance().setPCBAResult(Protocol.PCBA_TEST_ITEM_MIC,false,"007");
                }
                finish();
            }
        });

    }


    @SuppressLint("HandlerLeak")
    private Handler mHandler = new Handler()
    {
        public void handleMessage(Message msg)
        {

            switch (msg.what)
            {
                case MSG_START_TEST:
                    startMicRecord();
                    sendEmptyMessage(MSG_VOICE_CHANGE);
                    break;
                case MSG_VOICE_CHANGE:
                    if (mMediaRecorder != null)
                    {
                        int volume=mMediaRecorder.getMaxAmplitude();
                        LogUtil.i("MSG_VOICE_CHANGE volume: "+volume);
                        setDialogImage(volume);

                    }
                    sendEmptyMessageDelayed(MSG_VOICE_CHANGE, 200);
                    break;
                case STOP_MIC_RECORD:
                    removeCallbacksAndMessages(null);
                    stopMicRecord();
                    break;
                case 1:
                    tv_mic1.setVisibility(View.VISIBLE);
                    tv_mic1.setTextColor(Color.GREEN);
                    tv_mic1.setText("PASS");
                    flag++;
                    bt_mic2.setEnabled(true);
                    bt_mic1.setEnabled(false);
                    checkIsToNextItem();
                    break;
                case 2:
                    tv_mic2.setVisibility(View.VISIBLE);
                    tv_mic2.setTextColor(Color.GREEN);
                    tv_mic2.setText("PASS");
                    flag++;
//                    bt_mic3.setEnabled(true);
//                    tv_pass.setEnabled(true);
                    checkIsToNextItem();
                    break;
                case 3:
//                    tv_mic3.setVisibility(View.VISIBLE);
//                    tv_mic3.setTextColor(Color.GREEN);
//                    tv_mic3.setText("PASS");
                    flag++;
//                    bt_mic4.setEnabled(true);
                    checkIsToNextItem();
                    break;
                case 4:
//                    tv_mic4.setVisibility(View.VISIBLE);
//                    tv_mic4.setTextColor(Color.GREEN);
//                    tv_mic4.setText("PASS");
                    flag++;
                    Log.e("TAG","MIC测试"+flag);
                    tv_pass.setEnabled(true);
                    if(autoTestFlag){
                        checkIsToNextItem();
                    }
                    break;
                case 10:
                    if(msg.arg1==1){
                        tv_mic1.setVisibility(View.VISIBLE);
                        tv_mic1.setTextColor(Color.RED);
                        tv_mic1.setText("FAIl");
                    }else if(msg.arg1==2){
                        tv_mic2.setVisibility(View.VISIBLE);
                        tv_mic2.setTextColor(Color.RED);
                        tv_mic2.setText("FAIl");
                    }else if(msg.arg1==3){
//                        tv_mic3.setVisibility(View.VISIBLE);
//                        tv_mic3.setTextColor(Color.RED);
//                        tv_mic3.setText("FAIl");
                    }else if(msg.arg1==4){
//                        tv_mic4.setVisibility(View.VISIBLE);
//                        tv_mic4.setTextColor(Color.RED);
//                        tv_mic4.setText("FAIl");
                    }
                    goToBackResult();
                    break;
                default:
                    break;
            }

        }
    };



    //扬声器播放音乐  并用主麦录音
    private void startMicRecord()
    {
        LogUtil.i(TAG, "speakerAndRecord() AudioManager.MODE_NORMAL AudioSource.MIC");

        //进行主麦录音
        LogUtil.i(TAG, "inRecord()");
        try
        {
            if (Environment.getExternalStorageState().equals(
                    Environment.MEDIA_MOUNTED))
            {
                LogUtil.i(TAG, "Environment.getExternalStorageState():"
                        + Environment.getExternalStorageState());
            }
            record(MediaRecorder.AudioSource.MIC);
            isRecording = true;
        } catch (Exception e)
        {
            // TODO Auto-generated catch block
            LogUtil.i(TAG, "inRecord() Exception:" + e);
            loge(e);
        }
    }

    //停止扬声器播放和主麦录音
    private void stopMicRecord()
    {
        if (isRecording && mMediaRecorder != null)
        {
            mMediaRecorder.stop();
            mMediaRecorder.release();
            mMediaRecorder = null;
            LogUtil.i(TAG, "stopMicRecord() MediaRecorder  stop()  release()");
            isRecording = false;
        }
//        mHandler.sendEmptyMessageDelayed(MAINMICRECORDANDPLAY, END_TIME);
    }


    /**
     * 录音
     *
     * @param audioSource
     * @throws IllegalStateException
     * @throws IOException
     * @throws InterruptedException
     */
    private void record(int audioSource) throws IllegalStateException, IOException,
            InterruptedException
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
                }
            }
        });
        mMediaRecorder.setAudioChannels(2);
        mMediaRecorder.setOutputFile("sdcard/sparrow/testHeadset.aac");
        mAudioFilePath = "sdcard/sparrow/testHeadset.aac";
        LogUtil.i(TAG, "path :" + Environment.getExternalStorageDirectory().toString());
        File file = new File(mAudioFilePath);
        if (file.exists())
        {
            file.delete();
        }
        mMediaRecorder.prepare();
        mMediaRecorder.start();
    }


    private void loge(Object e)
    {
        if (e == null)
            return;
        Thread mThread = Thread.currentThread();
        StackTraceElement[] mStackTrace = mThread.getStackTrace();
        String mMethodName = mStackTrace[3].getMethodName();
        e = "[" + mMethodName + "] " + e;
        LogUtil.i(TAG, e + "");
    }


    @Override
    protected void onPause()
    {
        super.onPause();
        mHandler.removeCallbacksAndMessages(null);
        if (mAudioManager != null)
        {
            mAudioManager.setMode(AudioManager.MODE_NORMAL);
        }
        stopRecord();
        if (micControl != null) micControl.close();

        if (myTimerTask != null)
        {
            myTimerTask = null;
        }
    }

    @Override
    protected void onDestroy()
    {
        super.onDestroy();
    }

    public void stopRecord()
    {
        if (isRecording && mMediaRecorder != null)
        {
            mMediaRecorder.stop();
            mMediaRecorder.release();
            mMediaRecorder = null;
            Log.d(TAG, "------onStop----1-----");
        }
    }

    // 录音Dialog图片随声音大小切换
    public void setDialogImage(int voiceValue)
    {
        if (voiceValue < 200)
        {
            iv_audio.setImageResource(R.drawable.record_animate_01);
        } else if (voiceValue < 400)
        {
            iv_audio.setImageResource(R.drawable.record_animate_02);
        } else if (voiceValue < 800)
        {
            iv_audio.setImageResource(R.drawable.record_animate_03);
        } else if (voiceValue < 1600)
        {
            iv_audio.setImageResource(R.drawable.record_animate_04);
        } else if (voiceValue < 3200)
        {
            iv_audio.setImageResource(R.drawable.record_animate_05);
        } else if (voiceValue < 5000)
        {
            iv_audio.setImageResource(R.drawable.record_animate_06);
        } else if (voiceValue < 7000)
        {
            iv_audio.setImageResource(R.drawable.record_animate_07);
        } else if (voiceValue < 10000)
        {
            iv_audio.setImageResource(R.drawable.record_animate_08);
        } else if (voiceValue < 14000)
        {
            iv_audio.setImageResource(R.drawable.record_animate_09);
        } else if (voiceValue < 17000)
        {
            iv_audio.setImageResource(R.drawable.record_animate_10);
        } else if (voiceValue < 20000)
        {
            iv_audio.setImageResource(R.drawable.record_animate_11);
        } else if (voiceValue < 24000)
        {
            iv_audio.setImageResource(R.drawable.record_animate_12);
        } else if (voiceValue < 28000)
        {
            iv_audio.setImageResource(R.drawable.record_animate_13);
        } else
        {
            iv_audio.setImageResource(R.drawable.record_animate_14);
        }
    }


    public void checkVoice(final int micIndex)
    {

        timer = new Timer();
        myTimerTask = new TimerTask()
        {
            @Override
            public void run()
            {
                times++;
                Log.e("TAG", " 执行的次数为 " + times);
                if (times < 10)
                {
                    try{
                        if (mMediaRecorder!=null&&mMediaRecorder.getMaxAmplitude() > 700)
                        {
                            mHandler.sendEmptyMessage(micIndex);
                            cancel();

                        }
                    }catch (Exception e){
                        Log.d(TAG,"mMediaRecorder.getMaxAmplitude failed");
                    }
                } else if (times >= 10)
                {
                    Message msg = mHandler.obtainMessage();
                    msg.what = 10;
                    msg.arg1 = micIndex;
                    mHandler.sendMessage(msg);
                    cancel();
                }
            }
        };
        timer.schedule(myTimerTask, 500, 500);
    }

    @Override
    public void onClick(View v) {
        switch (v.getId()){
            case R.id.mic1:
                stopTimer();
                bt_mic1.setBackgroundColor(Color.BLUE);
                stopRecord();
                micControl.switchMic(false);
                startMicRecord();
                checkVoice(1);
                break;
            case R.id.mic2:
                stopTimer();
                bt_mic2.setBackgroundColor(Color.BLUE);
                stopRecord();
                micControl.switchMic(false);
                startMicRecord();
                checkVoice(2);
                break;
//            case R.id.mic3:
//                stopTimer();
////                bt_mic3.setBackgroundColor(Color.BLUE);
//                stopRecord();
//                micControl.switchMic(true);
//                startMicRecord();
//                checkVoice(3);
//                break;
//            case R.id.mic4:
//                stopTimer();
////                bt_mic4.setBackgroundColor(Color.BLUE);
//                stopRecord();
//                micControl.switchMic(true);
//                startMicRecord();
//                checkVoice(4);
//                break;
                default:
                    break;
        }

    }

    public void stopTimer(){
        times = 0;
        if(timer!=null){
            timer.cancel();
        }
        timer=null;
    }
    private void checkIsToNextItem() {
        if (flag>=2){
            mHandler.sendEmptyMessage(STOP_MIC_RECORD);
            if (testModel == Constant.INTENT_VALUE_TEST_MODEL_MMI)
            {
                ASSYEntity.getInstants().setMicTestResult(true);
                MMITestProcessManager.getInstance().toNextTest();
            }else if(testModel == Constant.INTENT_VALUE_TEST_MODEL_PCBA){

                ConnectManager.getInstance().setPCBAResult(Protocol.PCBA_TEST_ITEM_MIC,true,"007");
            }
            if(!Constant.TEST_TYPE_MMI_AUDO){
                tv_pass.setEnabled(true);
            }
            finish();
        }
    }
    private void goToBackResult() {
        mHandler.sendEmptyMessage(STOP_MIC_RECORD);
        if (testModel == Constant.INTENT_VALUE_TEST_MODEL_MMI)
        {    Log.e("TAG","MMI");
            ASSYEntity.getInstants().setMicTestResult(false);
            MMITestProcessManager.getInstance().testFail();
        }else if(testModel == Constant.INTENT_VALUE_TEST_MODEL_PCBA)
        {
            Log.e("TAG","pcba");
            ConnectManager.getInstance().setPCBAResult(Protocol.PCBA_TEST_ITEM_MIC,false,"007");
        }
        finish();
    }
}
