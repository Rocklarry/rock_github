package com.a3nod.lenovo.sparrowfactory.mmi;

import android.content.Context;
import android.content.Intent;
import android.util.Log;

import com.a3nod.lenovo.sparrowfactory.Constant;
import com.a3nod.lenovo.sparrowfactory.mmi.result.ASSYEntity;
import com.a3nod.lenovo.sparrowfactory.mmi.result.ResultActivity;

/**
 * Created by jianzhou.peng on 2018/1/8.
 */

public class MMITestProcessManager
{

    private static volatile MMITestProcessManager instance = null;
    private Context mContext = null;
    private static String[] action2;
    private static String[] action1;
    private static String[] action;
    private int currentItem = 0;
    private boolean isTesting = false;

    private MMITestProcessManager()
    {
    }

    public static MMITestProcessManager getInstance()
    {
        if (instance == null)
        {
            synchronized (MMITestProcessManager.class)
            {
                if (instance == null)
                {
                    instance = new MMITestProcessManager();
                    action1 = new String[]{
                          //  "gv_sensor.action",
                            "p_sensor.action",
                            "l_sensor.action",
                            "backlight.action",
                            "lcd.action",
                            "hello.TPTestActivity",
                            "hello.MultPointActivity"
                    };
                    action2 = new String[]{
                      /*      "hello.testActivity",  */
                            "backlight.action",
                            "button.action",
                            "hello.SpeakerActivity",
                            "pcba_mic",
                 /*            "gv_sensor.action",  */
                            "p_sensor.action",
                            "l_sensor.action",
                            "rgb.sensor.action",
                            "hello.BluetoothActivity",
                            "wifi.action",
                            "lcd.action",
                            "hello.TPTestActivity",
                            "hello.MultPointActivity",
                            "datacheck.action"
                    };
                }
            }
        }
        return instance;
    }

    /**
     * @param context
     */
    public void setApplicationContext(Context context, boolean autoTestFlag)
    {
        mContext = context;
        isTesting = autoTestFlag;
    }

    public void resetCurrentTimes()
    {
        currentItem = 0;
        ASSYEntity.getInstants().reset();
    }

    public void testFail()
    {
        if (isTesting)
        {
            isTesting = false;
            Intent intent = new Intent(mContext, ResultActivity.class);
            mContext.startActivity(intent);
        }
    }

    /**
     * 当前的测试项是否通过
     */
    public void toNextTest()
    {
        Log.e("TAG", "action.length" + action2.length);
        if (!isTesting)
        {
            return;
        }
        Log.e("TAG", " currentItem " + currentItem);
        if (Constant.TEST_TYPE_MMI1)
        {
            action = action1;
        } else if (Constant.TEST_TYPE_MMI2)
        {
            action = action2;
        }
        if (currentItem <= action.length - 1)
        {
            Intent intent = new Intent(action[currentItem]);
            intent.putExtra(Constant.INTENT_KEY_TEST_MODEL, Constant.INTENT_VALUE_TEST_MODEL_MMI);
            currentItem++;
            mContext.startActivity(intent);
        }
        if (currentItem == action.length)
        {
            testFinish();
        }
    }

    public boolean isTesting()
    {
        return isTesting;
    }

    public void testFinish()
    {
        isTesting = false;
    }
}
