package com.a3nod.lenovo.sparrowfactory.pcba;

import android.content.Context;
import android.graphics.Color;
import android.os.Bundle;
import android.os.Handler;
import android.os.Message;
import android.os.PowerManager;
import android.util.Log;
import android.view.View;
import android.widget.Button;
import android.widget.LinearLayout;
import android.widget.RelativeLayout;

import com.a3nod.lenovo.sparrowfactory.CrashHandler;
import com.a3nod.lenovo.sparrowfactory.R;
import com.a3nod.lenovo.sparrowfactory.interact.ConnectManager;
import com.a3nod.lenovo.sparrowfactory.interact.Protocol;
import com.a3nod.lenovo.sparrowfactory.tool.LogUtil;

import java.util.Timer;
import java.util.TimerTask;

/**
 * Created by jianzhou.peng on 2017/11/22.
 */

public class PcbaLcdActivity extends PCBABaseActivity
{
    public static final String TAG = "pcba_lcd_test";
    private RelativeLayout relativeLayout;
    private int[] color = new int[]{Color.WHITE, Color.BLACK, Color.RED, Color.GREEN, Color.BLUE};
    private volatile int index = 0;
    private Timer timer;
    LinearLayout ll_result;

    @Override
    protected void onCreate(Bundle savedInstanceState)
    {
        CrashHandler.getInstance().setCurrentTag(TAG, CrashHandler.TYPE_PCBA);
        super.onCreate(savedInstanceState);
        setContentView(R.layout.pcba_lcd_activity);
        relativeLayout = findViewById(R.id.backguand);
        ll_result = findViewById(R.id.ll_result);
        ll_result.setVisibility(View.GONE);
        PowerManager powerManager = (PowerManager) this
                .getSystemService(Context.POWER_SERVICE);
        boolean ifOpen = powerManager.isScreenOn();
        Log.e("TAG", "ifOPen" + ifOpen);
        initConfirmButton();
    }

    private Handler myHandler = new Handler()
    {
        @Override
        public void handleMessage(Message msg)
        {
            switch (msg.what)
            {
                case 0:
                    LogUtil.i("color change:" + index);
                    if(index< color.length){
                        relativeLayout.setBackgroundColor(color[index]);
                    }
                    index++;
                    break;
                case 1:
                    ll_result.setVisibility(View.VISIBLE);
//                    ConnectManager.getInstance().setPCBAResult(Protocol.PCBA_TEST_ITEM_LCD, false, "007");
//                    PcbaEntity.getInstants().mLcdReuslt = "pass";
//                    TestProcessManager.getInstance().toNextItem();
//                    finish();
                    break;
                default:
                    break;

            }
            super.handleMessage(msg);
        }
    };

    @Override
    protected void onResume()
    {
        super.onResume();
        startSwitchValue();
    }

    private void startSwitchValue()
    {
        timer = new Timer();
        TimerTask timerTask = new TimerTask()
        {
            @Override
            public void run()
            {
                LogUtil.i("start lcd ");
                myHandler.sendEmptyMessage(0);
                if (index == color.length - 1)
                {
                    myHandler.sendEmptyMessage(1);
                    timer.cancel();
                }
            }
        };
        timer.schedule(timerTask, 100, 1300);
        LogUtil.i("schedule lcd ");
    }

    /**
     * 重新测试
     */
    private void reset()
    {
        //if (index >= 3)
      //  {
            index = 0;
            startSwitchValue();
            relativeLayout.setBackgroundColor(color[0]);
            ll_result.setVisibility(View.GONE);
      //  }

//        throw new RuntimeException("抛个异常玩");
    }

    public void initConfirmButton()
    {

        Button tv_pass = findViewById(R.id.btn_result_pass);
        tv_pass.setOnClickListener(new View.OnClickListener()
        {
            @Override
            public void onClick(View v)
            {
                ConnectManager.getInstance().setPCBAResult(Protocol.PCBA_TEST_ITEM_LCD,true,"007");
                finish();
            }
        });
        Button tv_reset = findViewById(R.id.btn_result_reset);
        tv_reset.setOnClickListener(new View.OnClickListener()
        {
            @Override
            public void onClick(View v)
            {
                reset();
            }
        });
        Button tv_fail = findViewById(R.id.btn_result_fail);
        tv_fail.setOnClickListener(new View.OnClickListener()
        {
            @Override
            public void onClick(View v)
            {
                ConnectManager.getInstance().setPCBAResult(Protocol.PCBA_TEST_ITEM_LCD,false,"007");
                finish();
            }
        });
    }
}
