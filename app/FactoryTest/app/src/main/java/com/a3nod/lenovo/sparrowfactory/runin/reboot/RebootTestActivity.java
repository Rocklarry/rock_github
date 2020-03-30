package com.a3nod.lenovo.sparrowfactory.runin.reboot;

import android.annotation.SuppressLint;
import android.content.Intent;
import android.os.Bundle;
import android.os.Handler;
import android.widget.TextView;

import com.a3nod.lenovo.sparrowfactory.R;
import com.a3nod.lenovo.sparrowfactory.CrashHandler;
import com.a3nod.lenovo.sparrowfactory.runin.RuninBaseActivity;
import com.a3nod.lenovo.sparrowfactory.runin.RuninConfig;
import com.a3nod.lenovo.sparrowfactory.Constant;
import com.a3nod.lenovo.sparrowfactory.tool.LogUtil;
import com.a3nod.lenovo.sparrowfactory.tool.SPUtils;
import com.google.android.things.device.DeviceManager;

import java.lang.ref.WeakReference;


public class RebootTestActivity extends RuninBaseActivity
{

    public static final String TAG = "RebootTestActivity";

    private static final int MSG_TEST_START = 1;

    private TextView tv_rebotCount;
    RebootHandler handler;
    @Override
    protected void onCreate(Bundle savedInstanceState)
    {
        super.onCreate(savedInstanceState);
        CrashHandler.getInstance().setCurrentTag(TAG, CrashHandler.TYPE_RUNIN);
        setContentView(R.layout.activity_runin_reboot);

        tv_rebotCount = findViewById(R.id.tv_reboot_count);

        testTime = config.getItemDuration(RuninConfig.RUNIN_REBOOT_ID);
        LogUtil.i("test time : " + testTime);

        handler=new RebootHandler(this);

        boolean isRebootSuccess=getIntent().getBooleanExtra(Constant.INTENT_KEY_REBOOT_SUCCESS,false);

        if (isErrorRestart)
        {
            String errInfo=getIntent().getStringExtra(Constant.INTENT_KEY_ERROR_MSG);
            SPUtils.put(this, Constant.SP_KEY_REBOOT_TEST_REMARK, errInfo);
            mTestSuccess = false;
            LogUtil.i("reboot error restart : ");
            tv_rebotCount.setText("reboot Error Restart ");
            handler.sendEmptyMessageDelayed(MSG_TEST_FINISH,5000);
        } else
        {
            if(isRebootSuccess){
                mTestSuccess=true;
                handler.sendEmptyMessageDelayed(MSG_TEST_FINISH,5000);
                tv_rebotCount.setText("reboot success ");
            }else{
                LogUtil.i("reboot start : ");
                handler.sendEmptyMessageDelayed(MSG_TEST_START, 5 * 1000);

                tv_rebotCount.setText("targe reboot times : " + testTime + "\n"
                        + " reboot in 5 s"
                );
            }
        }

    }

    @SuppressLint("HandlerLeak")
    static class RebootHandler extends Handler
    {
        WeakReference<RebootTestActivity> wra;
        public RebootHandler(RebootTestActivity activity){
            wra=new WeakReference<>(activity);
        }
        public void handleMessage(android.os.Message msg)
        {
            if(wra.get()==null){
                return;
            }
            switch (msg.what)
            {
                case MSG_TEST_START:
                    wra.get().reboot();
                    break;
                case MSG_TEST_FINISH:
                    wra.get().testFinish();
                    break;
            }

        }
    }

    private void reboot()
    {


        LogUtil.i(TAG, "saveReBootCount Reboot");

//        Intent intent = new Intent(Intent.ACTION_REBOOT);
//        intent.setAction(Intent.ACTION_REBOOT);
//        intent.putExtra("nowait", 1);
//        intent.putExtra("interval", 1);
//        intent.putExtra("window", 0);
//        sendBroadcast(intent);
        DeviceManager manager = DeviceManager.getInstance();
        manager.reboot();


//            PowerManager pManager = (PowerManager) getSystemService(Context.POWER_SERVICE);
//            pManager.reboot("");
//            String cmd = "su  reboot";
//            try {
//                Runtime.getRuntime().exec(cmd);
//            } catch (Exception e){
//                Toast.makeText(getApplicationContext(), "Error! Fail to reboot.", Toast.LENGTH_SHORT).show();
//            }

//            DeviceManager.reboot();

        LogUtil.i(TAG, "First test loop, reboot test");

    }

    public void testFinish()
    {

        if (mTestSuccess)
        {
            config.saveTestResult(Constant.SP_KEY_REBOOT_TEST_SUCCESS);
        } else
        {
            config.saveTestResult(Constant.SP_KEY_REBOOT_TEST_FAIL);
        }
        if (isAutoRunin)
        {
            int currentTimes = (Integer) SPUtils.get(this, Constant.SP_KEY_RUNIN_CURRENT_TIME, 0);
            currentTimes++;
            SPUtils.put(this, Constant.SP_KEY_RUNIN_CURRENT_TIME, currentTimes);
            toNextText();
            finish();
        } else
        {
            //显示
            tv_rebotCount.setText("reboot test " + (mTestSuccess ? "success" : "fail"));
            LogUtil.d(" reboot 测试结束 完成 成功");
        }
    }

    @Override
    protected void onDestroy()
    {
        super.onDestroy();
        handler.removeCallbacksAndMessages(null);
    }
}
