/*   
 * Copyright (c) 2013-2020 Founder Ltd. All Rights Reserved.   
 *   
 * This software is the confidential and proprietary information of   
 * Founder. You shall not disclose such Confidential Information   
 * and shall use it only in accordance with the terms of the agreements   
 * you entered into with Founder.   
 *   
 */

package com.a3nod.lenovo.sparrowfactory;

import android.app.AlarmManager;
import android.app.PendingIntent;
import android.content.Context;
import android.content.Intent;
import android.util.Log;

import com.a3nod.lenovo.sparrowfactory.interact.ConnectManager;
import com.a3nod.lenovo.sparrowfactory.interact.Protocol;
import com.a3nod.lenovo.sparrowfactory.mmi.backlight.BackLightActivity;
import com.a3nod.lenovo.sparrowfactory.mmi.bluetooth.BluetoothActivity;
import com.a3nod.lenovo.sparrowfactory.mmi.button.ButtonActivity;
import com.a3nod.lenovo.sparrowfactory.mmi.camera.TestActivity;
import com.a3nod.lenovo.sparrowfactory.mmi.datacheck.DataCheckTestActivity;
import com.a3nod.lenovo.sparrowfactory.mmi.lcd.LcdActivity;
import com.a3nod.lenovo.sparrowfactory.mmi.multipoint.MultPointActivity;
import com.a3nod.lenovo.sparrowfactory.mmi.result.ASSYEntity;
import com.a3nod.lenovo.sparrowfactory.mmi.result.ResultActivity;
import com.a3nod.lenovo.sparrowfactory.mmi.sensor.LightSensorActivity;
import com.a3nod.lenovo.sparrowfactory.mmi.sensor.MyGVSensorActivity;
import com.a3nod.lenovo.sparrowfactory.mmi.sensor.PSensorActivity;
import com.a3nod.lenovo.sparrowfactory.mmi.speaker.SpeakerActivity;
import com.a3nod.lenovo.sparrowfactory.mmi.tp.TPTestActivity;
import com.a3nod.lenovo.sparrowfactory.mmi.wifi.WifiActivity;
import com.a3nod.lenovo.sparrowfactory.pcba.PCBADataCheckTestActivity;
import com.a3nod.lenovo.sparrowfactory.pcba.PCBAGSensorActivity;
import com.a3nod.lenovo.sparrowfactory.pcba.PCBALSensorActivity;
import com.a3nod.lenovo.sparrowfactory.pcba.PCBAMicTestActivity;
import com.a3nod.lenovo.sparrowfactory.pcba.PCBAPSensorActivity;
import com.a3nod.lenovo.sparrowfactory.pcba.PCBASpeakerTestActivity;
import com.a3nod.lenovo.sparrowfactory.pcba.PcbaButtonActivity;
import com.a3nod.lenovo.sparrowfactory.pcba.PcbaLcdActivity;
import com.a3nod.lenovo.sparrowfactory.pcba.PcbaTpActivity;
import com.a3nod.lenovo.sparrowfactory.runin.RuninService;
import com.a3nod.lenovo.sparrowfactory.tool.PcbaEntity;
import com.a3nod.lenovo.sparrowfactory.runin.audio.AudioTestActivity;
import com.a3nod.lenovo.sparrowfactory.runin.camera.CameraTestActivity;
import com.a3nod.lenovo.sparrowfactory.runin.cpu.CPUTestActivity;
import com.a3nod.lenovo.sparrowfactory.runin.emmc.EMMCTestActivity;
import com.a3nod.lenovo.sparrowfactory.runin.lcd.LCDTestActivity;
import com.a3nod.lenovo.sparrowfactory.runin.memory.MemoryTestActivity;
import com.a3nod.lenovo.sparrowfactory.runin.reboot.RebootTestActivity;
import com.a3nod.lenovo.sparrowfactory.runin.threedimensional.ThreeDimensionalActivity;
import com.a3nod.lenovo.sparrowfactory.runin.twodimensional.TwoDimensionalActivity;
import com.a3nod.lenovo.sparrowfactory.runin.video.VideoTestActivity;
import com.a3nod.lenovo.sparrowfactory.tool.LogUtil;
import com.a3nod.lenovo.sparrowfactory.tool.SPUtils;

import java.io.PrintWriter;
import java.io.StringWriter;
import java.io.Writer;
import java.lang.Thread.UncaughtExceptionHandler;

/**
 * 拦截crash异常信息
 *
 * @author aaa
 * @date 2015年5月15日
 */
public class CrashHandler implements UncaughtExceptionHandler
{
    private static final String TAG = "CrashHandler";
    private static final String DIR = "crash";
    private String currentTag = "";

    UncaughtExceptionHandler defaultUEH;
    private static Context context;
    public static final int TYPE_PCBA = 0;
    public static final int TYPE_RUNIN = 2;
    public static final int TYPE_MMI = 1;
    public static final int TYPE_OTHER = 10;
    private static int TYPE = TYPE_PCBA;

    private static CrashHandler handler = new CrashHandler();

    public static CrashHandler getInstance()
    {
        return handler;
    }

    public static void init(Context ctx)
    {
        if (context == null)
        {
            context = ctx.getApplicationContext();
        }
    }

    private CrashHandler()
    {
        defaultUEH = Thread.getDefaultUncaughtExceptionHandler();
        //将异常处理设置成这个类
        Thread.setDefaultUncaughtExceptionHandler(this);
    }

    public void setCurrentTag(String tag, int type)
    {
        currentTag = tag;
        TYPE = type;
    }

    @Override
    public void uncaughtException(final Thread thread, final Throwable ex)
    {
        final Writer result = new StringWriter();
        final PrintWriter printWriter = new PrintWriter(result);

        ex.printStackTrace(printWriter);

        String stacktrace = result.toString();

        printWriter.close();

        Log.e(TAG, stacktrace);

        if (TYPE == TYPE_RUNIN)
        {
            Class cls = getActivityFromTag(currentTag);
            LogUtil.i(" goto " + cls);
            Intent intent = new Intent(context, cls);
            intent.putExtra(Constant.INTENT_KEY_ERROR_RESTART, true);
            intent.putExtra(Constant.INTENT_KEY_IS_AUTO, (boolean) SPUtils.get(context, Constant.SP_KEY_IS_AUTORUNIN, false));
            intent.putExtra(Constant.INTENT_KEY_ERROR_MSG, stacktrace);

//            SPUtils.put(context, Constant.SP_KEY_IS_AUTORUNIN, false);
            boolean isAutoRunin=(Boolean)SPUtils.get(context,Constant.SP_KEY_IS_AUTORUNIN,false);
            SPUtils.put(context,Constant.SP_KEY_RUNIN_ERROR,true);
            boolean isError=(Boolean)SPUtils.get(context,Constant.SP_KEY_RUNIN_ERROR,false);
            LogUtil.i("crash handle isAutoRunin : "+isAutoRunin+ "  is error : "+isError);


            PendingIntent restartIntent = PendingIntent.getActivity(context, 0, intent, PendingIntent.FLAG_CANCEL_CURRENT);
            AlarmManager mgr = (AlarmManager) context.getSystemService(Context.ALARM_SERVICE);
            mgr.set(AlarmManager.RTC, System.currentTimeMillis() + 2000, restartIntent);
            new Thread(new Runnable()
            {
                @Override
                public void run()
                {
                    android.os.Process.killProcess(android.os.Process.myPid());
                }
            }).start();
//            defaultUEH.uncaughtException(thread, ex);
        } else if (TYPE == TYPE_PCBA)
        {

            switch (currentTag)
            {
                case PcbaLcdActivity.TAG:
                    ConnectManager.getInstance().setPCBAResult(Protocol.PCBA_TEST_ITEM_LCD, false, "");
                    break;
                case PcbaTpActivity.TAG:
                    ConnectManager.getInstance().setPCBAResult(Protocol.PCBA_TEST_ITEM_TP, false, "");
                    break;
                case TestActivity.TAG:
                    ConnectManager.getInstance().setPCBAResult(Protocol.PCBA_TEST_ITEM_CAMERA, false, "");
                    break;
                case PCBAMicTestActivity.TAG:
                    ConnectManager.getInstance().setPCBAResult(Protocol.PCBA_TEST_ITEM_MIC, false, "");
                    break;
                case PCBASpeakerTestActivity.TAG:
                    ConnectManager.getInstance().setPCBAResult(Protocol.PCBA_TEST_ITEM_SPEAKER, false, "");
                    break;
                case PCBAGSensorActivity.TAG:
                    ConnectManager.getInstance().setPCBAResult(Protocol.PCBA_TEST_ITEM_GSENSOR, false, "");
                    break;
                case PCBAPSensorActivity.TAG:
                    ConnectManager.getInstance().setPCBAResult(Protocol.PCBA_TEST_ITEM_PSENSOR, false, "");
                    break;
                case PCBALSensorActivity.TAG:
                    ConnectManager.getInstance().setPCBAResult(Protocol.PCBA_TEST_ITEM_LSENSOR, false, "");
                    break;
                case PcbaButtonActivity.TAG:
                    ConnectManager.getInstance().setPCBAResult(Protocol.PCBA_TEST_ITEM_KEY, false, "");
                    break;
                case BluetoothActivity.TAG:
                    ConnectManager.getInstance().setPCBAResult(Protocol.PCBA_TEST_ITEM_BLUETOOTH, false, "");
                    break;
                case WifiActivity.TAG:
                    ConnectManager.getInstance().setPCBAResult(Protocol.PCBA_TEST_ITEM_WIFI, false, "");
                    break;
                case PCBADataCheckTestActivity.TAG:
                    PcbaEntity.getInstants().setDataCheckResult(null);
                    ConnectManager.getInstance().setPCBAResult(Protocol.PCBA_TEST_ITEM_DATACHECK, false, "");
                    break;
            }
            defaultUEH.uncaughtException(thread, ex);
        } else if (TYPE == TYPE_MMI)
        {
            switch (currentTag)
            {
                case TestActivity.TAG:
                    ASSYEntity.getInstants().setCameraTestResult(false);
                    break;
                case BackLightActivity.TAG:
                    ASSYEntity.getInstants().setBacklightTestResult(false);
                    break;
                case ButtonActivity.TAG:
                    ASSYEntity.getInstants().setButtonsTestResult(false);
                    break;
                case SpeakerActivity.TAG:
                    ASSYEntity.getInstants().setSpeakerTestResult(false);
                    break;
                case PCBAMicTestActivity.TAG:
                    ASSYEntity.getInstants().setMicTestResult(false);
                    break;
                case MyGVSensorActivity.TAG:
                    ASSYEntity.getInstants().setGsensorTestResult(false);
                    break;
                case PSensorActivity.TAG:
                    ASSYEntity.getInstants().setPsensorTestResult(false);
                    break;
                case LightSensorActivity.TAG:
                    ASSYEntity.getInstants().setLsensorTestResult(false);
                    break;
                case BluetoothActivity.TAG:
                    ASSYEntity.getInstants().setBTTestResult(false);
                    break;
                case WifiActivity.TAG:
                    ASSYEntity.getInstants().setWifiTestResult(false);
                    break;
                case LcdActivity.TAG:
                    ASSYEntity.getInstants().setLCDTestResult(false);
                    break;
                case TPTestActivity.TAG:
                    ASSYEntity.getInstants().setTPTestResult(false);
                    break;
                case MultPointActivity.TAG:
                    ASSYEntity.getInstants().setPointTestResult(false);
                    break;
                case DataCheckTestActivity.TAG:

                    break;
                case ResultActivity.TAG:
                    break;
            }
            ASSYEntity.getInstants().save();

            Intent intent = new Intent(context, ResultActivity.class);

            PendingIntent restartIntent = PendingIntent.getActivity(context, 0, intent, PendingIntent.FLAG_CANCEL_CURRENT);
            AlarmManager mgr = (AlarmManager) context.getSystemService(Context.ALARM_SERVICE);
            mgr.set(AlarmManager.RTC, System.currentTimeMillis() + 3000, restartIntent);

            new Thread(new Runnable()
            {
                @Override
                public void run()
                {
                    android.os.Process.killProcess(android.os.Process.myPid());
                }
            }).start();
        } else
        {
            defaultUEH.uncaughtException(thread, ex);
        }
    }

    public Class getActivityFromTag(String tag)
    {
        Class cls = null;
        switch (tag)
        {
            case AudioTestActivity.TAG:
                cls = AudioTestActivity.class;
                break;
            case CameraTestActivity.TAG:
                cls = CameraTestActivity.class;
                break;
            case CPUTestActivity.TAG:
                cls = CPUTestActivity.class;
                break;
            case EMMCTestActivity.TAG:
                cls = EMMCTestActivity.class;
                break;
            case LCDTestActivity.TAG:
                cls = LCDTestActivity.class;
                break;
            case MemoryTestActivity.TAG:
                cls = MemoryTestActivity.class;
                break;
            case RebootTestActivity.TAG:
                cls = RebootTestActivity.class;
                break;
            case ThreeDimensionalActivity.TAG:
                cls = ThreeDimensionalActivity.class;
                break;
            case TwoDimensionalActivity.TAG:
                cls = TwoDimensionalActivity.class;
                break;
            case VideoTestActivity.TAG:
                cls = VideoTestActivity.class;
                break;
        }
        return cls;
    }
}
