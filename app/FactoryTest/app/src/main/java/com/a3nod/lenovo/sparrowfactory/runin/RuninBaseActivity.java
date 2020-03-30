package com.a3nod.lenovo.sparrowfactory.runin;


import android.app.Activity;
import android.content.Context;
import android.content.Intent;
import android.os.Bundle;
import android.support.v4.content.LocalBroadcastManager;
import android.view.KeyEvent;
import android.view.MotionEvent;
import android.view.Window;
import android.widget.Toast;

import com.a3nod.lenovo.sparrowfactory.Constant;
import com.a3nod.lenovo.sparrowfactory.R;
import com.a3nod.lenovo.sparrowfactory.interact.ConnectManager;
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

import org.json.JSONException;
import org.json.JSONObject;

import java.util.List;


public class RuninBaseActivity extends Activity
{

    public static final int MSG_TEST_FINISH = 0;
    public static final int MSG_START_TEST = 1;
    protected boolean isAutoRunin = false;
    protected int testTime = 0;
    public boolean mTestSuccess = false;
    protected volatile boolean interrupt = false;
    protected LocalBroadcastManager lbm;
    protected RuninConfig config;
    protected long startTime = 0;
    protected boolean isErrorRestart = false;

    @Override
    protected void onCreate(Bundle savedInstanceState)
    {
        super.onCreate(savedInstanceState);
        requestWindowFeature(Window.FEATURE_NO_TITLE);
//        SparrowApplication.getRefWatcher().watch(this);
        config = RuninConfig.getInstance();

        lbm = LocalBroadcastManager.getInstance(this);

        startTime = System.currentTimeMillis();
        //获取测试类型 单项测试还是自动测试
        Intent intent = getIntent();
        isAutoRunin = intent.getBooleanExtra(Constant.INTENT_KEY_IS_AUTO, Constant.RUNIN_TYPE_SINGLE);
        isErrorRestart = intent.getBooleanExtra(Constant.INTENT_KEY_ERROR_RESTART, false);
        String errorMsg=intent.getStringExtra(Constant.INTENT_KEY_ERROR_MSG);
        LogUtil.i("isErrorRestart : "+ isErrorRestart+"    isAutoRunin : "+ isAutoRunin);
        LogUtil.i("errorMsg : "+ errorMsg);
    }


    public void isMonkeyRunning(String tag, String s, Context context)
    {
        //如果在跑monkey, 则finish掉activity；
/*        if(ActivityManager.isUserAMonkey()){
         LogRuningTest.printInfo(tag, "---isMonkeyRunning", context);
         Activity activity =(Activity)context;
         activity.finish();
         LogRuningTest.printInfo(tag, "---finish", context);
        }*/
    }

    @Override
    protected void onPause()
    {
        super.onPause();
//        System.gc();
    }

    protected void toNextText()
    {
        Intent intent = new Intent(Constant.ACTION_NEXT_TEST);
        intent.putExtra(Constant.INTENT_KEY_RUNIN_TEST_RESULT, mTestSuccess);
        lbm.sendBroadcast(intent);
    }

//    @Override
//    public boolean onKeyDown(int keyCode, KeyEvent event)
//    {
//        switch (keyCode)
//        {
//            case KeyEvent.KEYCODE_BACK:
//                LogRuningTest.printInfo("RuninBaseActivity", "User click KEYCODE_BACK. Ignore it.", this);
////                return false;
//                interrupt = true;
//                break;
//            default:
//                break;
//        }
//        return super.onKeyDown(keyCode, event);
//    }

    private long lastMillis = 0;

    @Override
    public boolean onTouchEvent(MotionEvent event)
    {
        if (event.getAction() == KeyEvent.ACTION_DOWN)
        {
            LogUtil.i("baseactivity  touch ==== "+ event.getX()+" : "+event.getY());
            long currentMillis = System.currentTimeMillis();
            if (currentMillis - lastMillis > 2000)
            {
                lastMillis = currentMillis;
                Toast.makeText(this, getString(R.string.double_click_exit), Toast.LENGTH_SHORT).show();
            } else
            {
                LogUtil.i("baseactivity  double click exit");
                interrupt = true;
//                sendMessageToPC(-2);
                sendRuninCancelBroadcast();
                finish();
            }
        }
        return false;
    }

    public void sendRuninCancelBroadcast(){
        Intent stopRuninIntent = new Intent(Constant.INTENT_ACTION_STOP_RUNIN);
        lbm.sendBroadcast(stopRuninIntent);
    }


    public void sendMessageToPC(int failedItem){
        SPUtils.put(RuninBaseActivity.this, Constant.SP_KEY_IS_AUTORUNIN, false);
        SPUtils.put(RuninBaseActivity.this, Constant.SP_KEY_RUNIN_TEST_END_TIME, RuninService.time*1000L);


        long startTime = (Long) SPUtils.get(RuninBaseActivity.this, Constant.SP_KEY_RUNIN_TEST_START_TIME, 0L);
        long endTime = (Long) SPUtils.get(RuninBaseActivity.this, Constant.SP_KEY_RUNIN_TEST_END_TIME, 0L);
        long duration = (Long) SPUtils.get(RuninBaseActivity.this, Constant.SP_KEY_RUNIN_DURATION, 0L);

        List<TestItem> lti = config.getReport();
        String report=createReportString(lti);

        ConnectManager.getInstance().setRuninResult(failedItem,duration,endTime-startTime,report);
    }

    public String createReportString(List<TestItem> lti)
    {
        JSONObject root = new JSONObject();
        JSONObject item;
        for (TestItem ti : lti)
        {
            try
            {
                item = new JSONObject();
                item.put("success", ti.getSuccess());
                item.put("fail", ti.getFail());
                root.put(ti.getName(), item);
            } catch (JSONException e)
            {
                e.printStackTrace();
            }
        }

        return root.toString();
    }

}
