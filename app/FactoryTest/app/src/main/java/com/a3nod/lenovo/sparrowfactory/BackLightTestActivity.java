package com.a3nod.lenovo.sparrowfactory;

import android.app.Activity;
import android.support.v7.app.AppCompatActivity;
import android.os.Bundle;
import android.util.DisplayMetrics;
import android.view.KeyEvent;
import android.view.MotionEvent;
import android.view.View;
import android.widget.CheckBox;
import android.widget.CompoundButton;
import android.widget.SeekBar;
import android.widget.TextView;
import android.widget.Toast;

import com.a3nod.lenovo.sparrowfactory.mmi.backlight.BackLightActivity;
import com.a3nod.lenovo.sparrowfactory.tool.BrightnessTools;
import com.a3nod.lenovo.sparrowfactory.tool.LogUtil;

public class BackLightTestActivity extends Activity
{
    private SeekBar sb_brightness;
    private TextView tv_brightness;
    private CheckBox cb_lock_brightness;
    private boolean is8inch=true;

    @Override
    protected void onCreate(Bundle savedInstanceState)
    {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_back_light_test);
        tv_brightness=findViewById(R.id.tv_brightness);
        sb_brightness=findViewById(R.id.sb_brightness);
        cb_lock_brightness=findViewById(R.id.cb_lock_brightness);
        is8inch=is8InchScreen();

        int brightness=BrightnessTools.getScreenBrightness(this);
        LogUtil.i("oncreate  current brightness"+brightness);

        tv_brightness.setText(brightness+"");
        sb_brightness.setProgress(brightness);
        sb_brightness.setOnSeekBarChangeListener(new SeekBar.OnSeekBarChangeListener()
        {
            @Override
            public void onProgressChanged(SeekBar seekBar, int progress, boolean fromUser)
            {
                if(is8inch&&cb_lock_brightness.isChecked()&&progress>190){
                        sb_brightness.setProgress(190);
                }else{
                    setScreenBrightness(progress);
                    tv_brightness.setText(""+progress);
                }
            }

            @Override
            public void onStartTrackingTouch(SeekBar seekBar)
            {

            }

            @Override
            public void onStopTrackingTouch(SeekBar seekBar)
            {

            }
        });
        cb_lock_brightness.setOnCheckedChangeListener(new CompoundButton.OnCheckedChangeListener()
        {
            @Override
            public void onCheckedChanged(CompoundButton buttonView, boolean isChecked)
            {
                cb_lock_brightness.setText(isChecked?"locked to 190":"unlock");
            }
        });
        cb_lock_brightness.setChecked(true);
        cb_lock_brightness.setVisibility(is8inch? View.VISIBLE:View.INVISIBLE);
    }

    public boolean is8InchScreen(){
        DisplayMetrics dm = new DisplayMetrics();
        getWindowManager().getDefaultDisplay().getMetrics(dm);//display = getWindowManager().getDefaultDisplay();display.getMetrics(dm)（把屏幕尺寸信息赋值给DisplayMetrics dm）;
        int width = dm.widthPixels;
        LogUtil.i("screen  width : " + width + " height : " + dm.heightPixels);
        if(width == 800){
            return true;
        }else{
            return false;
        }
    }
    private void setScreenBrightness(int progress)
    {
//        BrightnessTools.stopAutoBrightness(this);
        BrightnessTools.saveBrightness(getContentResolver(),progress);
        int brightness=BrightnessTools.getScreenBrightness(this);
        LogUtil.i(" current brightness : "+ brightness);
    }

    private long lastMillis = 0;
    @Override
    public boolean onTouchEvent(MotionEvent event) {
        if (event.getAction() == KeyEvent.ACTION_DOWN) {
            long currentMillis = System.currentTimeMillis();
            if (currentMillis - lastMillis > 800) {
                lastMillis = currentMillis;
                Toast.makeText(this, getString(R.string.double_click_exit), Toast.LENGTH_SHORT).show();
            } else {
                finish();
            }
        }
        return false;
    }
}
