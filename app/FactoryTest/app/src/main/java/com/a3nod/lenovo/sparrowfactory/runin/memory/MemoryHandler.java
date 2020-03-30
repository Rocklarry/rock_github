package com.a3nod.lenovo.sparrowfactory.runin.memory;

import android.os.Handler;
import android.os.Message;

import com.a3nod.lenovo.sparrowfactory.runin.RuninBaseActivity;

import java.lang.ref.WeakReference;

/**
 * Description
 * Created by aaa on 2018/1/23.
 */
class MemoryHandler extends Handler
{
    WeakReference<MemoryTestActivity> wra;

    public MemoryHandler(MemoryTestActivity activity)
    {
        wra = new WeakReference<>(activity);
    }

    @Override
    public void handleMessage(Message msg)
    {
        super.handleMessage(msg);
        switch (msg.what)
        {
            case RuninBaseActivity.MSG_START_TEST:
                if (wra.get() != null)
                {
                    wra.get().memoryTest(this);
                }
                break;
            case MemoryTestActivity.MSG_SHOW_TEXT:
                if (wra.get() != null)
                {
                    wra.get().setStatue();
                }
                sendEmptyMessageDelayed(MemoryTestActivity.MSG_SHOW_TEXT, 1000);
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
}
