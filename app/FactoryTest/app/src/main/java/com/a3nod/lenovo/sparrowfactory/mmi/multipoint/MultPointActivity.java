package com.a3nod.lenovo.sparrowfactory.mmi.multipoint;

import android.app.Activity;
import android.content.Intent;
import android.os.Bundle;
import android.os.Handler;
import android.os.Message;
import android.view.View;
import android.widget.Button;

import com.a3nod.lenovo.sparrowfactory.Constant;
import com.a3nod.lenovo.sparrowfactory.CrashHandler;
import com.a3nod.lenovo.sparrowfactory.R;
import com.a3nod.lenovo.sparrowfactory.mmi.MMITestProcessManager;
import com.a3nod.lenovo.sparrowfactory.mmi.result.ASSYEntity;
import com.a3nod.lenovo.sparrowfactory.mmi.result.ResultActivity;

/**
 * Created by Lenovo on 2017/11/4.
 */

public class MultPointActivity extends Activity
{
    public static final String TAG = "MMI_MULTI_POINT_TEST";
    MulipointView mulipointView;

    Handler handler = new Handler()
    {
        @Override
        public void handleMessage(Message msg)
        {
            switch (msg.what)
            {
                case 0:
                    pass();
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
        CrashHandler.getInstance().setCurrentTag(TAG, CrashHandler.TYPE_MMI);
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_mmi_multipoint);

        mulipointView = findViewById(R.id.mv_multipoint);
        initConfirmButton();

        mulipointView.setTestFinishCallback(new MulipointView.TestSuccessCallback()
        {
            @Override
            public void onTestFinish(boolean isSuccess)
            {
                if (isSuccess)
                {
                    Message message = Message.obtain();
                    message.what = 0;
                    handler.sendMessageDelayed(message, 2000);
                }
            }
        });

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
                pass();
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
                fail();
            }
        });

    }

    @Override
    protected void onPause()
    {
        super.onPause();
        handler.removeCallbacksAndMessages(null);
    }

    public void pass()
    {
        ASSYEntity.getInstants().setPointTestResult(true);
        MMITestProcessManager.getInstance().toNextTest();
        if (Constant.TEST_TYPE_MMI1){
            Intent intent = new Intent(this, ResultActivity.class);
            startActivity(intent);
        }
        finish();
    }

    public void fail()
    {
        ASSYEntity.getInstants().setPointTestResult(false);
        MMITestProcessManager.getInstance().testFail();
        finish();
    }
}
