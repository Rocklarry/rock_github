package com.a3nod.lenovo.sparrowfactory.runin.threedimensional;

import android.os.Bundle;
import android.os.Handler;
import android.os.Message;
import android.view.View;
import android.widget.Toast;

import com.a3nod.lenovo.sparrowfactory.Constant;
import com.a3nod.lenovo.sparrowfactory.CrashHandler;
import com.a3nod.lenovo.sparrowfactory.runin.RuninBaseActivity;
import com.a3nod.lenovo.sparrowfactory.runin.RuninConfig;
import com.a3nod.lenovo.sparrowfactory.tool.LogUtil;
import com.a3nod.lenovo.sparrowfactory.tool.SPUtils;

import java.lang.ref.WeakReference;


//import android.media.AudioSystem;
//import android.os.SystemProperties;

/* < 0077092 xuyinwen 20151121 begin */
/* 0077092 xuyinwen 20151121 end > */

public class ThreeDimensionalActivity extends RuninBaseActivity
{

    public static final String TAG = "ThreeDimensionalActivity";

    ThreeDimensionalHandler handler;

    @Override
    protected void onCreate(Bundle savedInstanceState)
    {
        // TODO Auto-generated method stub
        super.onCreate(savedInstanceState);

        CrashHandler.getInstance().setCurrentTag(TAG, CrashHandler.TYPE_RUNIN);
        testTime = config.getItemDuration(RuninConfig.RUNIN_3D_ID);
        LogUtil.i("test time : " + testTime);
        handler = new ThreeDimensionalHandler(this);

        LogUtil.i(TAG, "START_OPGL_ANIMATION");
        if (isErrorRestart)
        {
            String errInfo = getIntent().getStringExtra(Constant.INTENT_KEY_ERROR_MSG);
            SPUtils.put(this, Constant.SP_KEY_3D_TEST_REMARK, errInfo);
            mTestSuccess = false;
            handler.sendEmptyMessage(MSG_TEST_FINISH);
        } else
        {
            View mClubeGLView = new GLTutorialCube(this);
            setContentView(mClubeGLView);
            mTestSuccess = true;
            handler.sendEmptyMessageDelayed(MSG_TEST_FINISH, testTime);
        }

    }

    @Override
    protected void onResume()
    {
        super.onResume();
    }

    static class ThreeDimensionalHandler extends Handler
    {
        WeakReference<ThreeDimensionalActivity> wra;

        public ThreeDimensionalHandler(ThreeDimensionalActivity activity)
        {
            wra = new WeakReference<>(activity);
        }

        public void handleMessage(Message msg)
        {
            switch (msg.what)
            {
                case MSG_TEST_FINISH:
                    if (wra.get() != null)
                    {
                        wra.get().testFinish();
                    }
                    break;
            }
        }
    }

    public void testFinish()
    {
        //if interrupted by user
        if (mTestSuccess)
        {
            config.saveTestResult(Constant.SP_KEY_3D_TEST_SUCCESS);
        } else
        {
            config.saveTestResult(Constant.SP_KEY_3D_TEST_FAIL);
        }
        if (isAutoRunin)
        {
            // to  next test
            toNextText();
            finish();
        } else
        {
            finish();
            Toast.makeText(this, "3D test " + (mTestSuccess ? "success" : "fail"), Toast.LENGTH_LONG).show();
            // single 3d test finish -->success
        }
    }


    @Override
    protected void onDestroy()
    {
        super.onDestroy();
        handler.removeCallbacksAndMessages(null);
    }

}
