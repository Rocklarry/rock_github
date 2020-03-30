package com.a3nod.lenovo.sparrowfactory.runin.twodimensional;

import android.content.pm.ActivityInfo;
import android.os.Bundle;
import android.os.Handler;
import android.os.Message;
import android.widget.Toast;

import com.a3nod.lenovo.sparrowfactory.Constant;
import com.a3nod.lenovo.sparrowfactory.CrashHandler;
import com.a3nod.lenovo.sparrowfactory.runin.RuninBaseActivity;
import com.a3nod.lenovo.sparrowfactory.runin.RuninConfig;
import com.a3nod.lenovo.sparrowfactory.tool.LogUtil;
import com.a3nod.lenovo.sparrowfactory.tool.SPUtils;

import java.lang.ref.WeakReference;


//import android.media.AudioSystem;
//import android.os.SystemProperties;

/* < 0077092 xuyinwen 20151121 begin */
/* 0077092 xuyinwen 20151121 end > */

public class TwoDimensionalActivity extends RuninBaseActivity
{

    public static final String TAG = "TwoDimensionalActivity";

    TwoDimensionalView mview;
    TwoDimensionalHandler handler;

    @Override
    protected void onCreate(Bundle savedInstanceState)
    {
        super.onCreate(savedInstanceState);
        setRequestedOrientation(ActivityInfo.SCREEN_ORIENTATION_PORTRAIT);

        CrashHandler.getInstance().setCurrentTag(TAG, CrashHandler.TYPE_RUNIN);
        testTime = config.getItemDuration(RuninConfig.RUNIN_2D_ID);
        LogUtil.i("test time : " + testTime);

        handler = new TwoDimensionalHandler(this);

        if (isErrorRestart)
        {
            String errInfo = getIntent().getStringExtra(Constant.INTENT_KEY_ERROR_MSG);
            SPUtils.put(this, Constant.SP_KEY_2D_TEST_REMARK, errInfo);
            mTestSuccess = false;
            handler.sendEmptyMessage(MSG_TEST_FINISH);
        } else
        {
            mTestSuccess = true;
            mview = new TwoDimensionalView(this);
            mview.requestFocus();
            mview.setFocusableInTouchMode(true);
            setContentView(mview);
            handler.sendEmptyMessageDelayed(MSG_TEST_FINISH, testTime);
        }
    }

    static class TwoDimensionalHandler extends Handler
    {
        WeakReference<TwoDimensionalActivity> wra;

        public TwoDimensionalHandler(TwoDimensionalActivity activity)
        {
            wra = new WeakReference<>(activity);
        }

        @Override
        public void handleMessage(Message msg)
        {
            super.handleMessage(msg);
            switch (msg.what)
            {
                case MSG_START_TEST:

                    break;
                case MSG_TEST_FINISH:
                    if (wra.get() != null)
                    {
                        wra.get().testFinish();
                    }
                    break;
            }
        }
    }

    public void testFinish()
    {
        if (mTestSuccess)
        {
            config.saveTestResult(Constant.SP_KEY_2D_TEST_SUCCESS);
        } else
        {
            config.saveTestResult(Constant.SP_KEY_2D_TEST_FAIL);
        }
        if (isAutoRunin)
        {
            // to  next test
            toNextText();
            finish();
        } else
        {
            finish();
            Toast.makeText(this, "2D test " + (mTestSuccess ? "success" : "fail"), Toast.LENGTH_LONG).show();
            //test finish , exit?
        }

    }

    @Override
    public void onResume()
    {
        super.onResume();
        mview.onResume();
    }

    @Override
    public void onPause()
    {
        super.onPause();
        mview.onPause();
        handler.removeCallbacksAndMessages(null);
    }

}
