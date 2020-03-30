package com.a3nod.lenovo.sparrowfactory.pcba;

import android.os.Bundle;
import android.os.Handler;
import android.os.Message;
import android.util.Log;
import android.view.MotionEvent;
import android.view.View;
import android.widget.Button;
import android.widget.TextView;

import com.a3nod.lenovo.sparrowfactory.CrashHandler;
import com.a3nod.lenovo.sparrowfactory.R;
import com.a3nod.lenovo.sparrowfactory.interact.ConnectManager;
import com.a3nod.lenovo.sparrowfactory.interact.Protocol;


/**
 * Created by jianzhou.peng on 2017/12/2.
 */

public class PcbaTpActivity extends PCBABaseActivity
{
    public static final String TAG = "pcba_tp_test";
    public static final int MSG_TEST_SUCCESS = 1;
    public static final int MSG_TP_TOUCHED =0;
    private TextView tv_pass;
    private boolean isFirst=true;
    private Handler myHandler = new Handler()
    {
        @Override
        public void handleMessage(Message msg)
        {
            switch (msg.what)
            {
                case MSG_TP_TOUCHED:
                    tv_pass.setVisibility(View.VISIBLE);
                    sendEmptyMessageDelayed(MSG_TEST_SUCCESS,1000);
                    break;
                case MSG_TEST_SUCCESS:
                    myHandler.removeCallbacksAndMessages(null);
                    findViewById(R.id.btn_result_pass).performClick();
                    break;
                default:
                    break;

            }
            super.handleMessage(msg);
        }
    };

    @Override
    protected void onCreate(Bundle savedInstanceState)
    {
        CrashHandler.getInstance().setCurrentTag(TAG, CrashHandler.TYPE_PCBA);
        super.onCreate(savedInstanceState);
        setContentView(R.layout.pcba_tp_activity);
        tv_pass = findViewById(R.id.tv_tp_pass);
        initConfirmButton();
    }

    @Override
    protected void onPause()
    {
        super.onPause();
        myHandler.removeCallbacksAndMessages(null);
    }

    @Override
    public boolean onTouchEvent(MotionEvent event)
    {
        switch (event.getAction())
        {
            case MotionEvent.ACTION_DOWN:
                float result = event.getX();
                if (result != -1&&isFirst)
                {
                    isFirst=false;
                    myHandler.sendEmptyMessage(MSG_TP_TOUCHED);
                }
                Log.e("TAG", "获取的值" + result);
                break;
            default:
                break;
        }
        return super.onTouchEvent(event);
    }

    public void initConfirmButton()
    {

        Button btn_pass = findViewById(R.id.btn_result_pass);
        btn_pass.setEnabled(false);
        btn_pass.setOnClickListener(new View.OnClickListener()
        {
            @Override
            public void onClick(View v)
            {
                ConnectManager.getInstance().setPCBAResult(Protocol.PCBA_TEST_ITEM_TP, true, "007");
                finish();
            }
        });

        Button btn_reset = findViewById(R.id.btn_result_reset);
        btn_reset.setVisibility(View.GONE);

        Button btn_fail = findViewById(R.id.btn_result_fail);
        btn_fail.setOnClickListener(new View.OnClickListener()
        {
            @Override
            public void onClick(View v)
            {
                ConnectManager.getInstance().setPCBAResult(Protocol.PCBA_TEST_ITEM_TP, false, "007");
                finish();
            }
        });
    }


}
