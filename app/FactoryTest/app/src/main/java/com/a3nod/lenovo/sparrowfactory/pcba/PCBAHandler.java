package com.a3nod.lenovo.sparrowfactory.pcba;

import android.app.Activity;
import android.os.Handler;
import android.os.Message;
import android.widget.Toast;

import com.a3nod.lenovo.sparrowfactory.R;

import java.lang.ref.WeakReference;

/**
 * Description
 * Created by aaa on 2018/1/31.
 */
public class PCBAHandler extends Handler
{
    WeakReference<Activity> wra;
    public static final int MSG_TEST_SUCCESS = 0;
    public static final int MSG_TEST_FAIL= 1;
    public static final int DELAY = 1000;
    public PCBAHandler(Activity activity)
    {
        wra = new WeakReference<>(activity);
    }

    @Override
    public void handleMessage(Message msg)
    {
        super.handleMessage(msg);
        switch (msg.what)
        {
            case MSG_TEST_SUCCESS:
                if (wra!=null&&wra.get() != null)
                {
                    Toast.makeText(wra.get(),"test success",Toast.LENGTH_SHORT).show();
                    wra.get().findViewById(R.id.btn_result_pass).performClick();
                    wra=null;
                }
                break;
            case  MSG_TEST_FAIL:
                if (wra!=null&&wra.get() != null)
                {
                    Toast.makeText(wra.get(),"test fail",Toast.LENGTH_SHORT).show();
                    wra.get().findViewById(R.id.btn_result_fail).performClick();
                    wra=null;
                }
                break;
        }
    }
}
