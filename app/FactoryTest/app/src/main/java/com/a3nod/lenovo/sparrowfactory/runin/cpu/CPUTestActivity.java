package com.a3nod.lenovo.sparrowfactory.runin.cpu;

import android.os.Bundle;
import android.widget.TextView;

import com.a3nod.lenovo.sparrowfactory.CrashHandler;
import com.a3nod.lenovo.sparrowfactory.R;
import com.a3nod.lenovo.sparrowfactory.runin.RuninBaseActivity;
import com.a3nod.lenovo.sparrowfactory.runin.RuninConfig;
import com.a3nod.lenovo.sparrowfactory.Constant;
import com.a3nod.lenovo.sparrowfactory.tool.LogUtil;
import com.a3nod.lenovo.sparrowfactory.tool.SPUtils;
import com.a3nod.lenovo.sparrowfactory.tool.ThreadPoolUtils;


public class CPUTestActivity extends RuninBaseActivity
{
    public static final String TAG = "CPUTestActivity";
    public CPUHandler handler;
    public static final int MSG_ALGORITHM_LOOG = 3;

    public static final int TASK_INT_TEST = 11;
    public static final int TASK_FLOAT_TEST = 12;
    public static final int TASK_PI_TEST = 13;
    public static final int TASK_SORT_TEST = 14;
    public static final int TASK_FIVE_TEST = 16;
    public static final int TASK_SIX_TEST = 17;

    public static final int MSG_SHOW_TEXT=21;
    private TextView tv_status;

    private long endTime;

    @Override
    protected void onCreate(Bundle savedInstanceState)
    {
        super.onCreate(savedInstanceState);

        CrashHandler.getInstance().setCurrentTag(TAG, CrashHandler.TYPE_RUNIN);
        setContentView(R.layout.activity_runin_cpu);

        tv_status = findViewById(R.id.tv_cpu_status);



        testTime = config.getItemDuration(RuninConfig.RUNIN_CPU_ID);
        LogUtil.i("test time : "+testTime);

        endTime = System.currentTimeMillis() + testTime;

        handler = new CPUHandler(this);
        if (isErrorRestart)
        {
            String errInfo=getIntent().getStringExtra(Constant.INTENT_KEY_ERROR_MSG);
            SPUtils.put(this, Constant.SP_KEY_CPU_TEST_REMARK, errInfo);
            mTestSuccess = false;
            handler.sendEmptyMessage(MSG_TEST_FINISH);
        } else
        {
            handler.sendEmptyMessage(MSG_START_TEST);
            handler.sendEmptyMessageDelayed(MSG_SHOW_TEXT,1000);
        }

    }

    public void setShowText(String str){
        tv_status.setText(str);
    }

    public void doTest(int arg){
        LogUtil.d("arg=" +arg +( handler ==null ?"handler is null" : "handler is not null"));
        if (interrupt)
        {
            return;
        }
        if (System.currentTimeMillis() < endTime)
        {
            ThreadPoolUtils.executeMultiThread(new AlgorithmTask(arg,handler));
        } else
        {
            if(!mTestSuccess)
            {
                mTestSuccess = true;
                handler.sendEmptyMessage(MSG_TEST_FINISH);
            }
        }
    }


    public void testFinish()
    {
        LogUtil.i("aaaaaaa cpu testfinish ");
        if(mTestSuccess){
            config.saveTestResult(Constant.SP_KEY_CPU_TEST_SUCCESS);
        }else
        {
            config.saveTestResult(Constant.SP_KEY_CPU_TEST_FAIL);
        }
        if (isAutoRunin)
        {
            LogUtil.i("aaaaaaa cpu isAutoRunin ");
            // to  next test
            toNextText();
            finish();
        } else
        {
            LogUtil.i("aaaaaaa cpu not AutoRunin");
            //test finish , exit?
            tv_status.setText("cpu test" + (mTestSuccess ? "success" : "fail"));
        }
    }

    @Override
    protected void onPause()
    {
        super.onPause();
        handler.removeCallbacksAndMessages(null);
    }
}
