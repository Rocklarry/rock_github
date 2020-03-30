package com.a3nod.lenovo.sparrowfactory.runin.emmc;

import android.os.Bundle;
import android.os.Handler;
import android.os.Message;
import android.widget.TextView;

import com.a3nod.lenovo.sparrowfactory.Constant;
import com.a3nod.lenovo.sparrowfactory.CrashHandler;
import com.a3nod.lenovo.sparrowfactory.R;
import com.a3nod.lenovo.sparrowfactory.runin.RuninBaseActivity;
import com.a3nod.lenovo.sparrowfactory.runin.RuninConfig;
import com.a3nod.lenovo.sparrowfactory.tool.LogUtil;
import com.a3nod.lenovo.sparrowfactory.tool.SPUtils;
import com.a3nod.lenovo.sparrowfactory.tool.ThreadPoolUtils;

import java.lang.ref.WeakReference;

/* < 0067427 xuyinwen 20150811 begin */
/* 0067427 xuyinwen 20150811 end > */

public class EMMCTestActivity extends RuninBaseActivity
{

    public static final String TAG = "EMMCTestActivity";

    private static final int EMMC_TEST_START = 10;
    private static final int MSG_SHOW_TEXT = 21;
    EMMCHandler mEMMCHandler;
    TextView tv_status;
    long endTime;

    @Override
    protected void onCreate(Bundle savedInstanceState)
    {
        super.onCreate(savedInstanceState);

        CrashHandler.getInstance().setCurrentTag(TAG, CrashHandler.TYPE_RUNIN);
        setContentView(R.layout.activity_runin_emmc);
        testTime = config.getItemDuration(RuninConfig.RUNIN_EMMC_ID);
        endTime = System.currentTimeMillis() + testTime;

        mEMMCHandler = new EMMCHandler(this);
        LogUtil.i("test time : " + testTime);

        tv_status = findViewById(R.id.tv_emmc_status);

        if (isErrorRestart)
        {
            String errInfo = getIntent().getStringExtra(Constant.INTENT_KEY_ERROR_MSG);
            SPUtils.put(this, Constant.SP_KEY_EMMC_TEST_REMARK, errInfo);
            mTestSuccess = false;
            mEMMCHandler.sendEmptyMessage(MSG_TEST_FINISH);
        } else
        {
            tv_status.setText(R.string.running_emmc_test);
            mEMMCHandler.sendEmptyMessage(EMMC_TEST_START);
            mEMMCHandler.sendEmptyMessage(MSG_SHOW_TEXT);
        }

    }

    public void onResume()
    {
        super.onResume();
    }

    int i = 0;
    String[] txt = {"reading/writing EMMC.", "reading/writing EMMC..", "reading/writing EMMC..."};

    static class EMMCHandler extends Handler
    {
        WeakReference<EMMCTestActivity> wra;

        public EMMCHandler(EMMCTestActivity activity)
        {
            wra = new WeakReference<>(activity);
        }

        public void handleMessage(Message msg)
        {
            switch (msg.what)
            {
                case EMMC_TEST_START:
                    if (wra.get() != null)
                    {
                        boolean isSuccess = true;
                        if (msg.obj != null)
                        {
                            isSuccess = (Boolean) msg.obj;
                        }
                        LogUtil.i("isSuccess time : " + isSuccess);
                        wra.get().startTest(isSuccess);
                    }
                    break;
                case MSG_SHOW_TEXT:
                    if (wra.get() != null)
                    {
                        wra.get().setStatus();
                    }
                    sendEmptyMessageDelayed(MSG_SHOW_TEXT, 1000);
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

    public void setStatus()
    {
        i++;
        tv_status.setText(txt[i % 3]);
    }

    public void testFinish()
    {
        if (mTestSuccess)
        {
            config.saveTestResult(Constant.SP_KEY_EMMC_TEST_SUCCESS);
        } else
        {
            config.saveTestResult(Constant.SP_KEY_EMMC_TEST_FAIL);
        }
        if (isAutoRunin)
        {
            toNextText();
            LogUtil.d(" emmc 测试完成  结果 : " + (mTestSuccess ? "成功" : "失败"));
            LogUtil.d(" emmc 测试结束 前往音频测试  ");
            finish();
        } else
        {
            tv_status.setText("emmc test" + (mTestSuccess ? "success" : "false"));
            LogUtil.d(" emmc 测试完成  结果 : " + (mTestSuccess ? "成功" : "失败"));
            finish();
        }
    }

    private void startTest(boolean isSuccess)
    {
        mTestSuccess = isSuccess;
        if (interrupt)
        {
            return;
        }
        if (System.currentTimeMillis() < endTime)
        {
            if (mTestSuccess)
            {
                ThreadPoolUtils.executeSingleThread(new EMMCTestRunnable(mEMMCHandler));
            } else
            {
                mEMMCHandler.sendEmptyMessage(MSG_TEST_FINISH);
            }
        } else
        {
            mTestSuccess = true;
            mEMMCHandler.sendEmptyMessage(MSG_TEST_FINISH);
        }

    }


    @Override
    protected void onDestroy()
    {
        super.onDestroy();
        mEMMCHandler.removeCallbacksAndMessages(null);
    }

}
