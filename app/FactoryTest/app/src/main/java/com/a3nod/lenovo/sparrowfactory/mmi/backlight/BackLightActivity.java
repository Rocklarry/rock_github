package com.a3nod.lenovo.sparrowfactory.mmi.backlight;

import android.app.Activity;
import android.content.Context;
import android.os.Bundle;
import android.os.Handler;
import android.os.Message;
import android.provider.Settings;
import android.util.Log;
import android.view.View;
import android.view.Window;
import android.view.WindowManager;
import android.widget.Button;
import android.widget.SeekBar;
import android.widget.TextView;

import com.a3nod.lenovo.sparrowfactory.Constant;
import com.a3nod.lenovo.sparrowfactory.CrashHandler;
import com.a3nod.lenovo.sparrowfactory.R;
import com.a3nod.lenovo.sparrowfactory.mmi.MMITestProcessManager;
import com.a3nod.lenovo.sparrowfactory.mmi.result.ASSYEntity;
import com.a3nod.lenovo.sparrowfactory.tool.SystemInfoTools;

import java.util.Timer;
import java.util.TimerTask;

/**
 * Created by jianzhou.peng on 2017/11/15.
 */

public class BackLightActivity extends Activity
{
    public static final String TAG="MMI_BACKLIGHT";
    private Handler handler = null;
    private int [] value = new int[]{0,255,0,255,0,255};
    private int index = 0;
    private Timer myTimer = null;
    private TimerTask timerTask = null;
    private Boolean autoTestFlag = false;
    private Button btn_pass,btn_fail;
    @Override
    protected void onCreate(Bundle savedInstanceState)
    {
        if(getIntent().getExtras()!=null){
            if (Constant.INTENT_VALUE_TEST_MODEL_MMI==(int)getIntent().getExtras().get(Constant.INTENT_KEY_TEST_MODEL)){
                autoTestFlag = true;
            }
        }
        CrashHandler.getInstance().setCurrentTag(TAG,CrashHandler.TYPE_MMI);
        super.onCreate(savedInstanceState);
        setContentView(R.layout.backlight_activity);
        handler = new Handler(){
            @Override
            public void handleMessage(Message msg) {
                switch (msg.what){
                    case 0:
                        setWindowBrightness(value[index]);
                        index ++;
                        if(index==6){
                            btn_fail.setEnabled(true);
                            btn_pass.setEnabled(true);
                        }
                        break;
                        default:
                            break;
                }
            }
        };

        initConfirmButton();
      //  initView();

    }

    public static String getMac(Context context) {
        return android.provider.Settings.Secure.getString(context.getContentResolver(), "wifi_address");
    }

    @Override
    protected void onResume()
    {
        super.onResume();
        startSwitchValue();
    }

    @Override
    protected void onPause()
    {
        super.onPause();
    }

    private void startSwitchValue() {
         myTimer = new Timer();
         timerTask = new TimerTask() {
            @Override
            public void run() {
                if(index==5){
                    myTimer.cancel();
                }
                handler.sendEmptyMessage(0);
            }
        };

        myTimer.schedule(timerTask,1000,1200);
    }

    private int getSystemBrightness()
    {
        int systemBrightness = 0;
        SystemInfoTools.getSystemProperty(Settings.System.SCREEN_BRIGHTNESS);
        try
        {
            systemBrightness = Settings.System.getInt(getContentResolver(), Settings.System.SCREEN_BRIGHTNESS);
        } catch (Settings.SettingNotFoundException e)
        {
            e.printStackTrace();
        }
        return systemBrightness;
    }

   private void initConfirmButton()
    {
        btn_pass = findViewById(R.id.btn_result_pass);
        btn_pass.setEnabled(false);
        btn_pass.setOnClickListener(new View.OnClickListener()
        {
            @Override
            public void onClick(View v)
            {
                ASSYEntity.getInstants().setBacklightTestResult(true);
                MMITestProcessManager.getInstance().toNextTest();
                finish();
            }
        });
        Button btn_reset = findViewById(R.id.btn_result_reset);
        btn_reset.setVisibility(View.GONE);
        btn_fail = findViewById(R.id.btn_result_fail);
        if(autoTestFlag){
            btn_fail.setEnabled(false);
        }
        btn_fail.setOnClickListener(new View.OnClickListener()
        {
            @Override
            public void onClick(View v)
            {
                ASSYEntity.getInstants().setBacklightTestResult(false);
                MMITestProcessManager.getInstance().testFail();
                finish();
            }
        });
    }


    private void setWindowBrightness(int brightness)
    {
        Window window = getWindow();
        WindowManager.LayoutParams lp = window.getAttributes();
        lp.screenBrightness = brightness / 255.0f;
        Log.e("TAG", "lp.screenBrightness" + lp.screenBrightness);
        window.setAttributes(lp);
    }


}
