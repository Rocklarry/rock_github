package com.a3nod.lenovo.sparrowfactory.runin.cpu;

import android.os.Handler;
import android.os.Message;

import com.a3nod.lenovo.sparrowfactory.R;
import com.a3nod.lenovo.sparrowfactory.runin.RuninBaseActivity;

import java.lang.ref.WeakReference;
import java.util.Random;

/**
 * Description
 * Created by aaa on 2018/1/23.
 */
class CPUHandler extends Handler
{
    WeakReference<CPUTestActivity> wra;
    Random random;
    String[] text;

    public CPUHandler(CPUTestActivity activity)
    {
        this.wra = new WeakReference<>(activity);
        random = new Random();
        text = new String[]{activity.getString(R.string.cpu_int_test),
                activity.getString(R.string.cpu_float_test),
                activity.getString(R.string.cpu_pi_test),
                activity.getString(R.string.cpu_sort_test),
                activity.getString(R.string.cpu_arithmetic_test),
                activity.getString(R.string.cpu_six_test)};
    }

    @Override
    public void handleMessage(Message msg)
    {
        super.handleMessage(msg);
        switch (msg.what)
        {
            case RuninBaseActivity.MSG_START_TEST:
                startTest();
                break;
            case CPUTestActivity.MSG_SHOW_TEXT:
                if (wra.get() != null)
                {
                    wra.get().setShowText(text[random.nextInt(6)]);
                }
                sendEmptyMessageDelayed(CPUTestActivity.MSG_SHOW_TEXT, 100);
                break;
            case CPUTestActivity.MSG_ALGORITHM_LOOG:
                if (wra.get() != null)
                {
                    wra.get().doTest(msg.arg1);
                }
                break;

            case RuninBaseActivity.MSG_TEST_FINISH:
                removeCallbacksAndMessages(null);
                if (wra.get() != null)
                {
                    wra.get().testFinish();
                }
                break;
        }
    }

    void startTest()
    {
        obtainMessage(CPUTestActivity.MSG_ALGORITHM_LOOG, CPUTestActivity.TASK_INT_TEST, 0).sendToTarget();
        obtainMessage(CPUTestActivity.MSG_ALGORITHM_LOOG, CPUTestActivity.TASK_FLOAT_TEST, 0).sendToTarget();
        obtainMessage(CPUTestActivity.MSG_ALGORITHM_LOOG, CPUTestActivity.TASK_PI_TEST, 0).sendToTarget();
        obtainMessage(CPUTestActivity.MSG_ALGORITHM_LOOG, CPUTestActivity.TASK_SORT_TEST, 0).sendToTarget();
        obtainMessage(CPUTestActivity.MSG_ALGORITHM_LOOG, CPUTestActivity.TASK_FIVE_TEST, 0).sendToTarget();
        obtainMessage(CPUTestActivity.MSG_ALGORITHM_LOOG, CPUTestActivity.TASK_SIX_TEST, 0).sendToTarget();
    }
}
