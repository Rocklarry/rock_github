package com.example.android_things;

import android.app.Activity;
import android.content.ComponentName;
import android.content.Context;
import android.content.Intent;
import android.content.ServiceConnection;
import android.hardware.Sensor;
import android.hardware.SensorEvent;
import android.hardware.SensorEventListener;
import android.hardware.SensorManager;
import android.os.Bundle;
import android.os.IBinder;
import android.view.MotionEvent;
import android.widget.TextView;


import com.example.android_things.driver.ProximityService;

public class P_sensor extends Activity {

    private SensorManager mSensorManager;
    private EventListener mProximityEventListener;

    private float p_sensor_value;
    private TextView ps_tx;

    private SensorManager.DynamicSensorCallback dynamicSensorCallback = new SensorManager.DynamicSensorCallback() {
        @Override
        public void onDynamicSensorConnected(Sensor sensor) {
            super.onDynamicSensorConnected(sensor);
            if(sensor.getType() == Sensor.TYPE_PROXIMITY){
                mProximityEventListener = new EventListener("Proximity");
                mSensorManager.registerListener(mProximityEventListener,sensor,SensorManager.SENSOR_DELAY_NORMAL);
            }
        }
    };

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_p_sensor);

        ps_tx = findViewById(R.id.ps);
    }

    @Override
    protected void onResume()
    {
        super.onResume();
        startProximitySensorRequest();
    }

    @Override
    protected void onPause()
    {
        super.onPause();
        stopProximitySensorRequest();
    }

    private void startProximitySensorRequest()
    {
        bindService(new Intent(this, ProximityService.class), connection, Context.BIND_AUTO_CREATE);
        mSensorManager = (SensorManager) getSystemService(SENSOR_SERVICE);
        mSensorManager.registerDynamicSensorCallback(dynamicSensorCallback);
    }

    private void stopProximitySensorRequest()
    {
        unbindService(connection);
        mSensorManager.unregisterDynamicSensorCallback(dynamicSensorCallback);
        mSensorManager.unregisterListener(mProximityEventListener);
    }

    private ServiceConnection connection = new ServiceConnection()
    {
        @Override
        public void onServiceDisconnected(ComponentName name)
        {
        }

        @Override
        public void onServiceConnected(ComponentName name, IBinder service)
        {
        }
    };

    private class EventListener implements SensorEventListener{
        private final String name;
        EventListener(String name){
            this.name = name;
        }

        public void onSensorChanged(SensorEvent event){
            p_sensor_value = event.values[0];
            ps_tx.setText("name:"+name+"  value:"+p_sensor_value);
        }
        public void onAccuracyChanged(Sensor sensor, int accuracy){

        }
    }


    private int count = 0;
    private long firClick = 0;
    private long secClick = 0;
    private final int interval = 1500;

    public boolean dispatchTouchEvent(MotionEvent ev) {
        if (MotionEvent.ACTION_DOWN == ev.getAction()) {
            count++;
            if (1 == count) {
                firClick = System.currentTimeMillis();
            } else if (2 == count) {
                secClick = System.currentTimeMillis();
                if (secClick - firClick < interval) {
                    count = 0;
                    firClick = 0;
                    System.exit(0);
                } else {
                    firClick = secClick;
                    count = 1;
                }
                secClick = 0;
            }
        }
        return super.dispatchTouchEvent(ev);
    }
}
