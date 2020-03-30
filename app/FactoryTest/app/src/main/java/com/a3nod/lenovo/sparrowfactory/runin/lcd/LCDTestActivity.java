package com.a3nod.lenovo.sparrowfactory.runin.lcd;

import android.graphics.Color;
import android.os.Bundle;
import android.os.Handler;
import android.os.Message;
import android.widget.LinearLayout;
import android.widget.Toast;

import com.a3nod.lenovo.sparrowfactory.Constant;
import com.a3nod.lenovo.sparrowfactory.CrashHandler;
import com.a3nod.lenovo.sparrowfactory.R;
import com.a3nod.lenovo.sparrowfactory.runin.RuninBaseActivity;
import com.a3nod.lenovo.sparrowfactory.runin.RuninConfig;
import com.a3nod.lenovo.sparrowfactory.tool.LogUtil;
import com.a3nod.lenovo.sparrowfactory.tool.SPUtils;

import java.lang.ref.WeakReference;


public class LCDTestActivity extends RuninBaseActivity
{

    public static final String TAG = "LCDTestActivity";

    LCDHandler handler;
    LinearLayout ll_bg;
    long endTime;

    private static final int MSG_START_ANIMATION = 10;

    int[] colors = {Color.RED, Color.GREEN, Color.BLUE, Color.WHITE, Color.BLACK};
    int index = 0;

    @Override
    protected void onCreate(Bundle savedInstanceState)
    {
        super.onCreate(savedInstanceState);
        CrashHandler.getInstance().setCurrentTag(TAG, CrashHandler.TYPE_RUNIN);
        setContentView(R.layout.activity_runin_lcd);
        ll_bg = findViewById(R.id.ll_runin_lcd);

        testTime = config.getItemDuration(RuninConfig.RUNIN_LCD_ID);
        endTime = System.currentTimeMillis() + testTime;

        LogUtil.i("test time : " + testTime);
        handler = new LCDHandler(this);

        if (isErrorRestart)
        {
            String errInfo = getIntent().getStringExtra(Constant.INTENT_KEY_ERROR_MSG);
            SPUtils.put(this, Constant.SP_KEY_LCD_TEST_REMARK, errInfo);
            mTestSuccess = false;
            handler.sendEmptyMessage(MSG_TEST_FINISH);
        } else
        {
            handler.sendEmptyMessage(MSG_START_ANIMATION);
//            throw new RuntimeException("测试崩溃重启");
        }
    }

    @Override
    protected void onResume()
    {
        super.onResume();
    }


    static class LCDHandler extends Handler
    {
        WeakReference<LCDTestActivity> wra;

        public LCDHandler(LCDTestActivity activity)
        {
            wra = new WeakReference<>(activity);
        }

        public void handleMessage(Message msg)
        {
            switch (msg.what)
            {
                case MSG_START_ANIMATION:
                    if (wra.get() != null)
                    {
                        wra.get().testLCD();
                    }
                    break;
                case MSG_TEST_FINISH:
                    if (wra.get() != null)
                    {
                        wra.get().testFinish();
                    }
                    break;
            }

            super.handleMessage(msg);
        }
    }

    public void testLCD()
    {
        if (System.currentTimeMillis() < endTime)
        {
            ll_bg.setBackgroundColor(colors[index]);
            index = (index + 1) % colors.length;
            handler.sendEmptyMessageDelayed(MSG_START_ANIMATION, 2000);
        } else
        {
            mTestSuccess = true;
            handler.sendEmptyMessage(MSG_TEST_FINISH);
        }
    }

    private void testFinish()
    {
        if (mTestSuccess)
        {
            config.saveTestResult(Constant.SP_KEY_LCD_TEST_SUCCESS);
        } else
        {
            config.saveTestResult(Constant.SP_KEY_LCD_TEST_FAIL);
        }
        if (isAutoRunin)
        {
            toNextText();
            finish();
            LogUtil.d(" lcd 测试结束 前往相机测试  ");
        } else
        {
            finish();
//            tv_status.setText("lcd test"+(mTestSuccess? "success" : "fail"));
            Toast.makeText(this, (mTestSuccess ? "success" : "fail"), Toast.LENGTH_SHORT).show();
            LogUtil.d(" lcd 测试完成  结果 : " + (mTestSuccess ? "成功" : "失败"));
        }
    }

    @Override
    protected void onPause()
    {
        super.onPause();
        handler.removeCallbacksAndMessages(null);
    }
}
