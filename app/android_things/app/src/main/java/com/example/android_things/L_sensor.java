package com.example.android_things;

import android.app.Activity;
import android.content.Intent;
import android.hardware.Sensor;
import android.hardware.SensorEvent;
import android.hardware.SensorEventListener;
import android.hardware.SensorManager;
import android.os.Bundle;
import android.view.MotionEvent;
import android.widget.TextView;
import android.widget.Toast;

import com.example.android_things.driver.ProximityService;

public class L_sensor extends Activity {


    private SensorManager mSensorManager;
    private EventListener mLightEventListener;

    private TextView ls_tx;

    private SensorManager.DynamicSensorCallback dynamicSensorCallback = new SensorManager.DynamicSensorCallback() {
        @Override
        public void onDynamicSensorConnected(Sensor sensor) {
            super.onDynamicSensorConnected(sensor);

            if(sensor.getType() == Sensor.TYPE_LIGHT){
                mLightEventListener = new EventListener("Light");
                mSensorManager.registerListener(mLightEventListener, sensor, SensorManager.SENSOR_DELAY_NORMAL);
            }
        }
    };



    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_l_sensor);

        ls_tx = findViewById(R.id.ls);

        /*this.startService(new Intent(this, ProximityService.class));
        mSensorManager = (SensorManager) getSystemService(SENSOR_SERVICE);
        mSensorManager.registerDynamicSensorCallback(dynamicSensorCallback);*/
        startProximitySensorRequest();

    }

    private void startProximitySensorRequest()
    {
        this.startService(new Intent(this, ProximityService.class));
        mSensorManager = (SensorManager) getSystemService(SENSOR_SERVICE);
        mSensorManager.registerDynamicSensorCallback(dynamicSensorCallback);
    }

    private void stopProximitySensorRequest()
    {
        this.stopService(new Intent(this, ProximityService.class));
        mSensorManager.unregisterDynamicSensorCallback(dynamicSensorCallback);
        mSensorManager.unregisterListener(mLightEventListener);
    }


    protected void onDestroy()
    {
        stopProximitySensorRequest();
        super.onDestroy();
    }


    public class EventListener implements SensorEventListener {

        private final String name;
        public EventListener(String light) {
            this.name = light;
        }

        @Override
        public void onSensorChanged(SensorEvent event) {
            if (event.sensor.getType() == Sensor.TYPE_LIGHT) {
                double ls_value=event.values[0];
                ls_tx.setText("name:"+name+"  values£º" +ls_value);

            }
        }

        @Override
        public void onAccuracyChanged(Sensor sensor, int accuracy) {

        }
    }


    private long exitTime = 0;
    public boolean dispatchTouchEvent(MotionEvent ev) {
        if (MotionEvent.ACTION_DOWN == ev.getAction()) {
            if ((System.currentTimeMillis() - exitTime) > 2000) {
                Toast.makeText(getApplicationContext(), "Press exit again", Toast.LENGTH_SHORT).show();
                exitTime = System.currentTimeMillis();
            } else {
                finish();
                System.exit(0);
            }
        }
        return super.dispatchTouchEvent(ev);
    }
}
