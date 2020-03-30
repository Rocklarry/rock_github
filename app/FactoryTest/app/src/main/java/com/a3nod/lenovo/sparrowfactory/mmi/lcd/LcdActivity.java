package com.a3nod.lenovo.sparrowfactory.mmi.lcd;

import android.app.Activity;
import android.graphics.Color;
import android.os.Bundle;
import android.util.Log;
import android.view.MotionEvent;
import android.view.View;
import android.view.Window;
import android.view.WindowManager;
import android.widget.Button;
import android.widget.ImageView;
import android.widget.LinearLayout;
import android.widget.RelativeLayout;
import android.widget.SeekBar;
import android.widget.TextView;

import com.a3nod.lenovo.sparrowfactory.CrashHandler;
import com.a3nod.lenovo.sparrowfactory.R;
import com.a3nod.lenovo.sparrowfactory.mmi.MMITestProcessManager;
import com.a3nod.lenovo.sparrowfactory.mmi.result.ASSYEntity;

/**
 * Created by jianzhou.peng on 2017/11/2.
 */

public class LcdActivity extends Activity
{
    public static final String TAG = "MMI_LCD_TEST";
    private RelativeLayout relativeLayout = null;
    final int[] color = new int[]{Color.WHITE,Color.BLACK, Color.RED, Color.GREEN,Color.BLACK,Color.BLUE};
    private int i = 0;
    private boolean flag = false;
    private LinearLayout ll_result;
    private TextView tv_hint;
    private ImageView iv_time;
    @Override
    protected void onCreate(Bundle savedInstanceState)
    {
        CrashHandler.getInstance().setCurrentTag(TAG,CrashHandler.TYPE_MMI);
        super.onCreate(savedInstanceState);
        setContentView(R.layout.lcd_activity);
        initview();
        initConfirmButton();
        initView();


    }

    public void initConfirmButton()
    {
        ll_result = findViewById(R.id.ll_result);
        ll_result.setVisibility(View.GONE);
        Button btn_pass = findViewById(R.id.btn_result_pass);
        btn_pass.setOnClickListener(new View.OnClickListener()
        {
            @Override
            public void onClick(View v)
            {
                ASSYEntity.getInstants().setLCDTestResult(true);
                MMITestProcessManager.getInstance().toNextTest();
                finish();
            }
        });
        Button btn_reset = findViewById(R.id.btn_result_reset);
        btn_reset.setOnClickListener(new View.OnClickListener()
        {
            @Override
            public void onClick(View v)
            {
                resetTest();
            }
        });
        Button btn_fail = findViewById(R.id.btn_result_fail);
        btn_fail.setOnClickListener(new View.OnClickListener()
        {
            @Override
            public void onClick(View v)
            {
                ASSYEntity.getInstants().setLCDTestResult(false);
                MMITestProcessManager.getInstance().testFail();
                finish();
            }
        });

    }

    private void initview()
    {
        tv_hint=findViewById(R.id.tv_lcd_hint);
        tv_hint.postDelayed(new Runnable()
        {
            @Override
            public void run()
            {
                tv_hint.setVisibility(View.GONE);
            }
        },2000);
        relativeLayout = findViewById(R.id.backguand);
        relativeLayout.setOnTouchListener(new View.OnTouchListener()
        {
            @Override
            public boolean onTouch(View v, MotionEvent event)
            {
                if (event.getAction() == MotionEvent.ACTION_DOWN)
                {
                    if (!flag)
                    {
                        relativeLayout.setBackgroundColor(color[i + 1]);
                    }
                    i++;
                    if (i == color.length - 1)
                    {
                        ll_result.setVisibility(View.VISIBLE);
                        flag = true;
                    }
                    if(i==color.length-2){
                        iv_time.setVisibility(View.VISIBLE);
                    }else{
                        iv_time.setVisibility(View.INVISIBLE);
                    }
                }
                return true;
            }
        });
    }

    private void resetTest()
    {
        flag = false;
        i = 0;
        relativeLayout.setBackgroundColor(color[0]);
        ll_result.setVisibility(View.GONE);
        iv_time.setVisibility(View.GONE);
    }
    public void initView()
    {
        iv_time = findViewById(R.id.iv_timeshow);
        iv_time.setVisibility(View.GONE);

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
