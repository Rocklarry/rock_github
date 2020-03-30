package com.a3nod.lenovo.sparrowfactory.runin;

import android.app.Service;
import android.content.BroadcastReceiver;
import android.content.Context;
import android.content.Intent;
import android.content.IntentFilter;
import android.media.AudioManager;
import android.os.IBinder;
import android.support.v4.content.LocalBroadcastManager;
import android.text.TextUtils;
import android.widget.Toast;

import com.a3nod.lenovo.sparrowfactory.Constant;
import com.a3nod.lenovo.sparrowfactory.RebootActivity;
import com.a3nod.lenovo.sparrowfactory.runin.audio.AudioTestActivity;
import com.a3nod.lenovo.sparrowfactory.runin.camera.CameraTestActivity;
import com.a3nod.lenovo.sparrowfactory.runin.cpu.CPUTestActivity;
import com.a3nod.lenovo.sparrowfactory.runin.emmc.EMMCTestActivity;
import com.a3nod.lenovo.sparrowfactory.runin.lcd.LCDTestActivity;
import com.a3nod.lenovo.sparrowfactory.runin.memory.MemoryTestActivity;
import com.a3nod.lenovo.sparrowfactory.runin.reboot.RebootTestActivity;
import com.a3nod.lenovo.sparrowfactory.runin.report.ReportActivity;
import com.a3nod.lenovo.sparrowfactory.runin.threedimensional.ThreeDimensionalActivity;
import com.a3nod.lenovo.sparrowfactory.runin.twodimensional.TwoDimensionalActivity;
import com.a3nod.lenovo.sparrowfactory.runin.video.VideoTestActivity;
import com.a3nod.lenovo.sparrowfactory.tool.FileUtils;
import com.a3nod.lenovo.sparrowfactory.tool.LogUtil;
import com.a3nod.lenovo.sparrowfactory.tool.SPUtils;
import com.google.android.things.pio.Gpio;
import com.google.android.things.pio.GpioCallback;
import com.google.android.things.pio.PeripheralManager;

import java.io.IOException;
import java.util.ArrayList;
import java.util.List;
import java.util.Timer;
import java.util.TimerTask;

public class RuninService extends Service
{
    RuninConfig config;
    private Gpio gpio85;
    private Gpio gpio1019;
    AudioManager mAudioManager;

    public RuninService()
    {

    }

    public List<Class> lc;
    public List<Class> testItemList;

    LocalBroadcastManager lbm;
    TestServiceReceiver tsr;


    public volatile static long time;


    @Override
    public void onCreate()
    {
        super.onCreate();
        lbm = LocalBroadcastManager.getInstance(this);
        mAudioManager = (AudioManager) getSystemService(Context.AUDIO_SERVICE);
        config = RuninConfig.getInstance();
        initTestList();

        IntentFilter filter = new IntentFilter();
        filter.addAction(Constant.ACTION_NEXT_TEST);
        filter.addAction(Constant.INTENT_ACTION_STOP_RUNIN);
        tsr = new TestServiceReceiver();
        lbm.registerReceiver(tsr, filter);
        LogUtil.i("runin service oncreate ");
        startTiming();
        registerVolumeListener();
    }

    public void registerVolumeListener()
    {
        PeripheralManager service = PeripheralManager.getInstance();
        try
        {
            gpio85 = service.openGpio("GPIO_85");
            gpio85.setDirection(Gpio.DIRECTION_IN);
            gpio85.setEdgeTriggerType(Gpio.EDGE_BOTH);
            gpio85.registerGpioCallback(gpioCallback85);
            LogUtil.d("gpio85 value " + gpio85.getValue());

            gpio1019 = service.openGpio("GPIO_1019");
            gpio1019.setDirection(Gpio.DIRECTION_IN);
            gpio1019.setEdgeTriggerType(Gpio.EDGE_BOTH);
            gpio1019.registerGpioCallback(gpioCallback1019);
            LogUtil.d("gpio85 value " + gpio1019.getValue());
        } catch (IOException e)
        {
            e.printStackTrace();
        }

    }

    private void unregisterGpioListener()
    {
        try
        {
            if (gpio85 != null)
            {
                gpio85.close();
                gpio85.unregisterGpioCallback(gpioCallback85);
            }
            if (gpio1019 != null)
            {
                gpio1019.close();
                gpio1019.unregisterGpioCallback(gpioCallback1019);
            }
        } catch (IOException e)
        {
            e.printStackTrace();
        }
    }

    private void initTestList()
    {
        testItemList = new ArrayList<>();
        testItemList.add(CPUTestActivity.class);
        testItemList.add(MemoryTestActivity.class);
        testItemList.add(EMMCTestActivity.class);
        testItemList.add(LCDTestActivity.class);
        testItemList.add(TwoDimensionalActivity.class);
        testItemList.add(ThreeDimensionalActivity.class);
        testItemList.add(AudioTestActivity.class);
        testItemList.add(VideoTestActivity.class);
        testItemList.add(CameraTestActivity.class);
        //        testItemList.add(SleepTestActivity.class);
        testItemList.add(RebootTestActivity.class);
        lc = testItemList;
    }

    private void getTestItem()
    {
        String str = (String) SPUtils.get(this, Constant.SP_KEY_RUNIN_TEST_ITEM, "0,1,2,3,4,5,6,7,8,9,");
        String[] items = str.split(",");
        lc = new ArrayList<>();
        for (int i = 0; i < items.length; i++)
        {
            if (!TextUtils.isEmpty(items[i]))
            {
                lc.add(testItemList.get(Integer.parseInt(items[i])));
            }
        }
    }

    Timer timer;
    long startTime = 0;
    long lastTime = 0;

    private void startTiming()
    {

        long tmp = System.currentTimeMillis();
        if (tmp < 365 * 24 * 60 * 60 * 10000L)
        {
            //如果获取当前开机时间<一年 那么 说明 这次开机是没有网络
            startTime = 0;
            SPUtils.put(this, Constant.SP_KEY_BOOT_TIME, startTime);
        } else
        {
            //如果获取当前开机时间>一年 那么 说明 这次开机有网络
            startTime = (long) SPUtils.get(this, Constant.SP_KEY_BOOT_TIME, 0L);
            lastTime = startTime;
        }
        LogUtil.i("aaaaaaaaaaaa start time : " + startTime);
        timer = new Timer();
        timer.schedule(new TimerTask()
        {
            @Override
            public void run()
            {
                long tmp = System.currentTimeMillis();
                long tmpTime = time;
                //                LogUtil.i("aaaaaaaaaaaa current time : " + tmp);
                if (tmp - lastTime < 365 * 24 * 60 * 60 * 10000L)
                {
                    tmpTime = tmp - startTime;
                } else
                {
                    startTime = tmp - tmpTime * 1000 + 1000;
                    SPUtils.put(RuninService.this, Constant.SP_KEY_BOOT_TIME, startTime);
                    tmpTime = tmp - startTime;
                }
                time = tmpTime / 1000;
                lastTime = tmp;
                LogUtil.i("aaaaaaaaaaaa service time : " + time + "  current systemtime : " + tmp + "start time : " + startTime);
            }
        }, 1000, 1000);
    }


    @Override
    public int onStartCommand(Intent intent, int flags, int startId)
    {
        LogUtil.i("runin serviceonStartCommand  ");
        if (intent == null)
        {
            //崩溃重启
            LogUtil.i("runin service error restart ");
            if ((Boolean) SPUtils.get(this, Constant.SP_KEY_IS_AUTORUNIN, false))
            {
                getTestItem();
            }
        } else
        {
            if (intent.getBooleanExtra(Constant.INTENT_KEY_START_AUTO_RUNIN, false))
            {
                config.clearReport();

                long duration = intent.getLongExtra(Constant.INTENT_KEY_RUNIN_DURATION, 0L);
                String test_item = intent.getStringExtra(Constant.INTENT_KEY_RUNIN_TEST_ITEM);

                long currentTime = time * 1000L;
                setAutoRuninConfig(duration, currentTime);
                LogUtil.i(" RuninService.class.duration: " + config.getRunin_duration());
                LogUtil.i(" RuninService.class.test_item: " + test_item);

                SPUtils.put(this, Constant.SP_KEY_IS_AUTORUNIN, true);
                SPUtils.put(this, Constant.SP_KEY_RUNIN_DURATION, duration);
                SPUtils.put(this, Constant.SP_KEY_RUNIN_CURRENT_TIME, 0);
                SPUtils.put(this, Constant.SP_KEY_RUNIN_TEST_ITEM, test_item);
                SPUtils.put(this, Constant.SP_KEY_RUNIN_TEST_START_TIME, currentTime);
                SPUtils.put(this, Constant.SP_KEY_RUNIN_TEST_END_TIME, currentTime);
                getTestItem();
                //这里是点击按钮启动的服务
                config.setTestStatus(0);
                startTest(true, 0);
            } else
            {
                boolean isAutoRunin=(Boolean) SPUtils.get(this, Constant.SP_KEY_IS_AUTORUNIN, false);
                boolean isError=(Boolean) SPUtils.get(this, Constant.SP_KEY_RUNIN_ERROR, false);

                LogUtil.i("runin service start from boot start  isAutorunin : "+isAutoRunin+ " is Error : "+isError);
                getTestItem();
                //这是从启动广播进来的
                if (isAutoRunin&&!isError)
                {
                    Intent rebootIntent = new Intent(this, RebootTestActivity.class);
                    rebootIntent.putExtra(Constant.INTENT_KEY_IS_AUTO, true);
                    rebootIntent.putExtra(Constant.INTENT_KEY_REBOOT_SUCCESS, true);
                    startActivity(rebootIntent);
                }
                LogUtil.i("current reboot time" + (int) SPUtils.get(this, Constant.SP_KEY_REBOOT_CURRENT_TIME, -1));
                if ((int) SPUtils.get(this, Constant.SP_KEY_REBOOT_CURRENT_TIME, -1) >= 0)
                {
                    Intent rebootIntent = new Intent(this, RebootActivity.class);
                    startActivity(rebootIntent);
                }
            }
        }

        return   START_NOT_STICKY ;
//        return super.onStartCommand(intent, flags, startId);
    }

    public void setAutoRuninConfig(long duration, long startTime)
    {
        config.setRunin_duration(duration + startTime);

        config.setItemDuration(RuninConfig.RUNIN_CPU_ID, 0);
        config.setItemDuration(RuninConfig.RUNIN_MEMORY_ID, 0);
        config.setItemDuration(RuninConfig.RUNIN_EMMC_ID, 0);

        config.setItemDuration(RuninConfig.RUNIN_LCD_ID, 0);
        config.setItemDuration(RuninConfig.RUNIN_2D_ID, 0);
        config.setItemDuration(RuninConfig.RUNIN_3D_ID, 0);

        config.setItemDuration(RuninConfig.RUNIN_AUDIO_ID, 1);
        config.setItemDuration(RuninConfig.RUNIN_VIDEO_ID, 0);
        config.setItemDuration(RuninConfig.RUNIN_CAMERA_ID, 0);

        config.setItemDuration(RuninConfig.RUNIN_REBOOT_ID, 1);
    }

    public void startTest(boolean isAuto, int testId)
    {
        LogUtil.i("start test activity: " + lc.get(testId));
        Intent intent = new Intent(this, lc.get(testId));
        intent.putExtra(Constant.INTENT_KEY_IS_AUTO, isAuto);
        startActivity(intent);
    }

    public void viewReport()
    {
        Intent intent = new Intent(this, ReportActivity.class);
        startActivity(intent);
    }

    @Override
    public IBinder onBind(Intent intent)
    {
        throw new UnsupportedOperationException("Not yet implemented");
    }

    @Override
    public void onDestroy()
    {
        super.onDestroy();
        unregisterGpioListener();
        timer.cancel();
        lbm.unregisterReceiver(tsr);
    }

    class TestServiceReceiver extends BroadcastReceiver
    {

        @Override
        public void onReceive(Context context, Intent intent)
        {
            String action = intent.getAction();
            long runinStartTime = (Long) SPUtils.get(RuninService.this, Constant.SP_KEY_RUNIN_TEST_START_TIME, 0L);
            long runinSettingTime = (Long) SPUtils.get(RuninService.this, Constant.SP_KEY_RUNIN_DURATION, 0L);
            if (action.equals(Constant.ACTION_NEXT_TEST))
            {
                boolean isRunin = (boolean) SPUtils.get(context, Constant.SP_KEY_IS_AUTORUNIN, false);


                int currentTestId = 0;
                boolean isSuccess = intent.getBooleanExtra(Constant.INTENT_KEY_RUNIN_TEST_RESULT, false);
                LogUtil.i("receive broadcast isRunin: " + isRunin + "is Success : " + isSuccess + " currentTimeMillis : " + System.currentTimeMillis() + " :  setting time" + config.getRunin_duration());
                if (!isRunin)
                {
                    return;
                }
                SPUtils.put(context,Constant.SP_KEY_RUNIN_ERROR,false);
                if (time * 1000 < config.getRunin_duration())
                {
                    if (isSuccess)
                    {
                        LogUtil.i("receive broadcast former TestId : " + currentTestId);
                        currentTestId = config.getTestStatus();
                        currentTestId = (++currentTestId) % lc.size();
                        config.setTestStatus(currentTestId);
                        LogUtil.i("receive broadcast next TestId : " + currentTestId);
                        startTest(true, currentTestId);

                        FileUtils.createReportFile(config.getReport(), Constant.RUNIN_STATUS_RUNNING, runinSettingTime, runinStartTime, -1);
                    } else
                    {
                        currentTestId = config.getTestStatus();
                        Intent toErrorPage = new Intent(context, FailActivity.class);
                        toErrorPage.putExtra("failed_item", lc.get(currentTestId));
                        LogUtil.i("failed item : " + lc.get(currentTestId)+ "  currentTestId : "+currentTestId);
                        startActivity(toErrorPage);

                        SPUtils.put(RuninService.this, Constant.SP_KEY_IS_AUTORUNIN, false);
                        SPUtils.put(RuninService.this, Constant.SP_KEY_RUNIN_TEST_END_TIME, time * 1000L);
                        FileUtils.createReportFile(config.getReport(), getFailItemID(lc.get(currentTestId)), runinSettingTime, runinStartTime, time * 1000L);
                    }
                } else
                {
                    LogUtil.i("receive broadcast test finish to report ");
                    SPUtils.put(RuninService.this, Constant.SP_KEY_IS_AUTORUNIN, false);
                    SPUtils.put(RuninService.this, Constant.SP_KEY_RUNIN_TEST_END_TIME, time * 1000L);
                    FileUtils.createReportFile(config.getReport(), Constant.RUNIN_STATUS_SUCCESS, runinSettingTime, runinStartTime, time * 1000L);
                    viewReport();
                }
            } else if (Constant.INTENT_ACTION_STOP_RUNIN.equals(action))
            {
                boolean isRunin = (boolean) SPUtils.get(context, Constant.SP_KEY_IS_AUTORUNIN, false);
                LogUtil.i("receive broadcast INTENT_ACTION_STOP_RUNIN isRunin: " + isRunin);
                if (isRunin)
                {
                    SPUtils.put(RuninService.this, Constant.SP_KEY_IS_AUTORUNIN, false);
                    SPUtils.put(RuninService.this, Constant.SP_KEY_RUNIN_TEST_END_TIME, time * 1000L);
                    FileUtils.createReportFile(config.getReport(), Constant.RUNIN_STATUS_CANCEL, runinSettingTime, runinStartTime, time * 1000L);
                }
            }
        }
    }

    public int getFailItemID(Class cls)
    {
        int item = -1;
        if (CPUTestActivity.class.equals(cls))
        {
            item = 0;
        } else if (MemoryTestActivity.class.equals(cls))
        {
            item = 1;
        } else if (EMMCTestActivity.class.equals(cls))
        {
            item = 2;
        } else if (LCDTestActivity.class.equals(cls))
        {
            item = 3;
        } else if (TwoDimensionalActivity.class.equals(cls))
        {
            item = 4;
        } else if (ThreeDimensionalActivity.class.equals(cls))
        {
            item = 5;
        } else if (AudioTestActivity.class.equals(cls))
        {
            item = 6;
        } else if (VideoTestActivity.class.equals(cls))
        {
            item = 7;
        } else if (CameraTestActivity.class.equals(cls))
        {
            item = 8;
        } else if (RebootTestActivity.class.equals(cls))
        {
            item = 9;
        } else
        {
            item = -1;
        }
        return item;
    }


    GpioCallback gpioCallback85 = new GpioCallback()
    {
        @Override
        public boolean onGpioEdge(Gpio gpio)
        {
            try
            {
                boolean keyDown = gpio.getValue();
                int currentVolume = mAudioManager.getStreamVolume(AudioManager.STREAM_MUSIC);
                if (!keyDown)
                {
                    LogUtil.d("gpioCallback85 volumeUP keydown  currentVolume : " + currentVolume);
                } else
                {
                    mAudioManager.setStreamVolume(AudioManager.STREAM_MUSIC, currentVolume + 5, 0);
                    LogUtil.d("gpioCallback85 volumeUP keyup");
                    currentVolume = mAudioManager.getStreamVolume(AudioManager.STREAM_MUSIC);
                    sendVolumeBroadcast(true, currentVolume, AudioManager.STREAM_MUSIC);
                }
            } catch (IOException e)
            {
                e.printStackTrace();
            }
            return true;
        }
    };
    GpioCallback gpioCallback1019 = new GpioCallback()
    {
        @Override
        public boolean onGpioEdge(Gpio gpio)
        {
            try
            {
                boolean keyDown = gpio.getValue();
                int currentVolume = mAudioManager.getStreamVolume(AudioManager.STREAM_MUSIC);
                if (keyDown)
                {
                    LogUtil.d("gpioCallback1019 volumeDOWN keydown : " + currentVolume);
                } else
                {
                    mAudioManager.setStreamVolume(AudioManager.STREAM_MUSIC, currentVolume - 5, 0);
                    LogUtil.d("gpioCallback1019 volumeDOWN keyup");
                    currentVolume = mAudioManager.getStreamVolume(AudioManager.STREAM_MUSIC);
                    sendVolumeBroadcast(false, currentVolume, AudioManager.STREAM_MUSIC);
                }
            } catch (IOException e)
            {
                e.printStackTrace();
            }
            return true;
        }
    };

    public void sendVolumeBroadcast(boolean isUP, int current, int type)
    {
        Toast.makeText(RuninService.this, "volume " + (isUP
                                                       ? "up :"
                                                       : "down : ") + current, Toast.LENGTH_SHORT).show();
        Intent intent = new Intent(isUP ? Constant.ACTION_VOLUME_UP : Constant.ACTION_VOLUME_DOWN);
        intent.putExtra(Constant.INTENT_KEY_VOLUME_TYPE, type);
        intent.putExtra(Constant.INTENT_KEY_CURRENT_VOLUME, current);
        lbm.sendBroadcast(intent);
    }

}
