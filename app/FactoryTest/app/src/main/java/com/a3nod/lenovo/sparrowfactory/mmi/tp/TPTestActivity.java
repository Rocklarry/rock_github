package com.a3nod.lenovo.sparrowfactory.mmi.tp;

import android.app.Activity;
import android.os.Bundle;
import android.util.Log;
import android.view.KeyEvent;
import android.view.MotionEvent;
import android.widget.Toast;

import com.a3nod.lenovo.sparrowfactory.CrashHandler;
import com.a3nod.lenovo.sparrowfactory.R;
import com.a3nod.lenovo.sparrowfactory.mmi.MMITestProcessManager;
import com.a3nod.lenovo.sparrowfactory.mmi.result.ASSYEntity;

/**
 * Created by Lenovo on 2017/11/4.
 */

public class TPTestActivity extends Activity
{
    //    private PaletteView mPaletteView;
    public static final String TAG = "MMI_TP_TEST";
    private TPTestView testView;

    @Override
    protected void onCreate(Bundle savedInstanceState)
    {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_tp);
        CrashHandler.getInstance().setCurrentTag(TAG,CrashHandler.TYPE_MMI);
        Log.v("msg", "I am here TP");
/*        mPaletteView =findViewById(R.id.palette);
        mPaletteView.setMode(PaletteView.Mode.DRAW);
        new Thread(new Runnable() {
            @Override
            public void run() {
                try {
                    Thread.sleep(10000);
                    finish();
                } catch (InterruptedException e) {
                    e.printStackTrace();
                }
            }
        }).start();*/
        testView = findViewById(R.id.tp_test);
        testView.setTestFinishCallback(new TPTestView.TestSuccessCallback()
        {
            @Override
            public void onTestFinish(boolean isSuccess)
            {
                if (isSuccess)
                {
                    Toast.makeText(TPTestActivity.this, "tp test success", Toast.LENGTH_SHORT).show();
                    ASSYEntity.getInstants().setTPTestResult(true);
                    MMITestProcessManager.getInstance().toNextTest();
                } else
                {
                    Toast.makeText(TPTestActivity.this, "tp test fail", Toast.LENGTH_SHORT).show();
                    ASSYEntity.getInstants().setTPTestResult(false);
                    MMITestProcessManager.getInstance().testFail();
                }
                finish();
            }
        });
    }

    @Override
    protected void onDestroy() {
        super.onDestroy();
        if(testView!=null){
            testView.cancelTimerTask();
        }

    }
}
