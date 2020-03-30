package com.a3nod.lenovo.sparrowfactory.mmi.sensor;

import android.app.Activity;
import android.content.Context;
import android.content.Intent;
import android.hardware.Sensor;
import android.hardware.SensorEvent;
import android.hardware.SensorEventListener;
import android.hardware.SensorManager;
import android.os.Bundle;
import android.util.Log;
import android.view.View;
import android.widget.Button;
import android.widget.TextView;

import com.a3nod.lenovo.sparrowfactory.Constant;
import com.a3nod.lenovo.sparrowfactory.CrashHandler;
import com.a3nod.lenovo.sparrowfactory.R;
import com.a3nod.lenovo.sparrowfactory.driver.ProximityService;
import com.a3nod.lenovo.sparrowfactory.mmi.MMITestProcessManager;
import com.a3nod.lenovo.sparrowfactory.mmi.result.ASSYEntity;
import com.a3nod.lenovo.sparrowfactory.pcba.PCBAHandler;

import java.text.DecimalFormat;

/**
 * Created by jianzhou.peng on 2017/11/8.
 */

public class LightSensorActivity extends Activity
{
    public static final String TAG = "MMI_L_SENSOR_TEST";

    private TextView tv_lux;
    PCBAHandler pcbaHandler;
    private double currentValue;
    private boolean isPass;
    private SensorManager mSensorManager;
    private EventListener mAmbientLightEventListener;
    private Button passBtn;
    private SensorManager.DynamicSensorCallback mDynamicSensorCallback = new SensorManager.DynamicSensorCallback()
    {
        @Override
        public void onDynamicSensorConnected(Sensor sensor)
        {
            if (sensor.getType() == Sensor.TYPE_LIGHT)
            {
                Log.i(TAG, "aaaaaaaaaaaaa Ambient light sensor connected");
                mAmbientLightEventListener = new EventListener("Ambient Light");
                mSensorManager.registerListener(mAmbientLightEventListener, sensor, SensorManager.SENSOR_DELAY_NORMAL);
            }
        }
    };

    @Override
    protected void onCreate(Bundle savedInstanceState)
    {
        CrashHandler.getInstance().setCurrentTag(TAG,CrashHandler.TYPE_MMI);
        super.onCreate(savedInstanceState);
        setContentView(R.layout.light_sensor_activity);
        startProximitySensorRequest();
        initView();
        Log.e("TAG", "进入L-Sensor界面");
        pcbaHandler=new PCBAHandler(this);
        initConfirmButton();
    }

    public void initConfirmButton()
    {
        passBtn= findViewById(R.id.btn_result_pass);
        passBtn.setEnabled(false);
        passBtn.setOnClickListener(new View.OnClickListener()
        {
            @Override
            public void onClick(View v)
            {
                ASSYEntity.getInstants().setLsensorTestResult(true);
                ASSYEntity.getInstants().LSensorValue=currentValue;
                Log.i(TAG,  " 1111sensor changed: " + currentValue);
                MMITestProcessManager.getInstance().toNextTest();
                finish();
            }
        });
        Button btn_reset = findViewById(R.id.btn_result_reset);
        btn_reset.setVisibility(View.GONE);
        btn_reset.setOnClickListener(new View.OnClickListener()
        {
            @Override
            public void onClick(View v)
            {
                reset();
            }
        });
        Button btn_fail = findViewById(R.id.btn_result_fail);
        btn_fail.setOnClickListener(new View.OnClickListener()
        {
            @Override
            public void onClick(View v)
            {
                ASSYEntity.getInstants().LSensorValue=currentValue;
                ASSYEntity.getInstants().setLsensorTestResult(false);
                MMITestProcessManager.getInstance().testFail();
                finish();
            }
        });

    }

    private void startProximitySensorRequest()
    {
        this.startService(new Intent(this, ProximityService.class));
        mSensorManager = (SensorManager) getSystemService(SENSOR_SERVICE);
        mSensorManager.registerDynamicSensorCallback(mDynamicSensorCallback);
    }

    private void stopProximitySensorRequest()
    {
        this.stopService(new Intent(this, ProximityService.class));
        mSensorManager.unregisterDynamicSensorCallback(mDynamicSensorCallback);
        mSensorManager.unregisterListener(mAmbientLightEventListener);
    }

    private class EventListener implements SensorEventListener
    {

        private final String name;

        EventListener(String name)
        {
            this.name = name;
        }

        @Override
        public void onSensorChanged(SensorEvent event)
        {
            Log.i(TAG, name + " sensor changed: " + event.values[0]);
            for (int i = 0; i < event.values.length; i++)
            {
                Log.i(TAG, name + " sensor changed: " + event.values[i]);
                Log.i(TAG, name + " sensor changed: " + Integer.toBinaryString(Float.floatToIntBits(event.values[i])));
            }
            DecimalFormat df = new DecimalFormat("##0.00");

            double brightness=event.values[0]*0.006;
            tv_lux.setText("values: " + df.format(event.values[0] * 0.006) + "LUX");
            if(brightness>4&&brightness<7){
                isPass=true;
                if (passBtn != null&& !Constant.TEST_TYPE_MMI_AUDO){
                    passBtn.setEnabled(true);
                }
            }
            if(isPass&&brightness>1.00f&& Constant.TEST_TYPE_MMI_AUDO){
                pcbaHandler.sendEmptyMessageDelayed(PCBAHandler.MSG_TEST_SUCCESS,PCBAHandler.DELAY);
            }
            currentValue=brightness;
//            tv_lux.setText("values：" +event.values[0]+ "LUX");
        }

        @Override
        public void onAccuracyChanged(Sensor sensor, int accuracy)
        {
            Log.i(TAG, name + "sensor accuracy changed: " + accuracy);
        }
    }


    private void initView()
    {
        tv_lux =findViewById(R.id.lux_text);
    }


    @Override
    protected void onDestroy()
    {
        stopProximitySensorRequest();
        super.onDestroy();
    }

    /**
     * 重新测试
     */
    private void reset()
    {
        tv_lux.setText("values: " + (float) 0);
    }
}
