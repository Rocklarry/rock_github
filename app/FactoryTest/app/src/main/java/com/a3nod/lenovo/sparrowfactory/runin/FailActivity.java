package com.a3nod.lenovo.sparrowfactory.runin;

import android.app.Activity;
import android.content.Intent;
import android.os.Bundle;
import android.view.KeyEvent;
import android.view.MotionEvent;
import android.widget.TextView;
import android.widget.Toast;

import com.a3nod.lenovo.sparrowfactory.Constant;
import com.a3nod.lenovo.sparrowfactory.R;
import com.a3nod.lenovo.sparrowfactory.runin.audio.AudioTestActivity;
import com.a3nod.lenovo.sparrowfactory.runin.camera.CameraTestActivity;
import com.a3nod.lenovo.sparrowfactory.runin.cpu.CPUTestActivity;
import com.a3nod.lenovo.sparrowfactory.runin.emmc.EMMCTestActivity;
import com.a3nod.lenovo.sparrowfactory.runin.lcd.LCDTestActivity;
import com.a3nod.lenovo.sparrowfactory.runin.memory.MemoryTestActivity;
import com.a3nod.lenovo.sparrowfactory.runin.reboot.RebootTestActivity;
import com.a3nod.lenovo.sparrowfactory.runin.report.ReportActivity;
import com.a3nod.lenovo.sparrowfactory.runin.threedimensional.ThreeDimensionalActivity;
import com.a3nod.lenovo.sparrowfactory.tool.LogUtil;
import com.a3nod.lenovo.sparrowfactory.tool.SPUtils;
import com.a3nod.lenovo.sparrowfactory.runin.twodimensional.TwoDimensionalActivity;
import com.a3nod.lenovo.sparrowfactory.runin.video.VideoTestActivity;

public class FailActivity extends Activity
{
    TextView tv_fail;

    @Override
    protected void onCreate(Bundle savedInstanceState)
    {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_fail);
        Intent intent = getIntent();
        Class failItem =(Class) intent.getSerializableExtra("failed_item");
        LogUtil.i("failed item : "+ failItem);
        tv_fail = findViewById(R.id.tv_test_fail);
        tv_fail.setText(getFailTestName(failItem) + " TEST FAIL");
    }

    public String getFailTestName(Class cls)
    {
        String name = "";
        if(CPUTestActivity.class.equals(cls))
        {
            name = "CPU";
        }else if(MemoryTestActivity.class.equals(cls))
        {
            name = "MEMORY";
        }else if(EMMCTestActivity.class.equals(cls))
        {
            name = "EMMC";
        }else if(LCDTestActivity.class.equals(cls))
        {
            name = "LCD";
        }else if(TwoDimensionalActivity.class.equals(cls))
        {
            name = "2D";
        }else if(ThreeDimensionalActivity.class.equals(cls))
        {
            name = "3D";
        }else if(AudioTestActivity.class.equals(cls))
        {
            name = "AUDIO";
        }else if(VideoTestActivity.class.equals(cls))
        {
            name = "VIDEO";
        }else if(CameraTestActivity.class.equals(cls))
        {
            name = "CAMERA";
        }else if(RebootTestActivity.class.equals(cls))
        {
            name = "REBOOT";
        }else
        {
            name="UNKNOWN";
        }
        return name;
    }

    private long lastMillis = 0;

    @Override
    public boolean onTouchEvent(MotionEvent event)
    {

        LogUtil.i("touch");
        if (event.getAction() == KeyEvent.ACTION_DOWN)
        {
            long currentMillis = System.currentTimeMillis();
            if (currentMillis - lastMillis > 2000)
            {
                lastMillis = currentMillis;
                Toast.makeText(this, getString(R.string.double_click_exit), Toast.LENGTH_SHORT).show();
            } else
            {
                Intent intent = new Intent(this, ReportActivity.class);
                startActivity(intent);
                finish();
            }
        }
        return false;
    }

}
