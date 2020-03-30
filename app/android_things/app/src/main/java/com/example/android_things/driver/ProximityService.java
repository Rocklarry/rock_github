package com.example.android_things.driver;

import android.app.Service;
import android.content.Intent;
import android.os.Binder;
import android.os.IBinder;
import android.util.Log;
import com.google.android.things.pio.PeripheralManager;
import java.io.IOException;
import java.util.List;

public class ProximityService extends Service {
    private static final String TAG = ProximityService.class.getSimpleName();
    private final IBinder binder = new MyBinder();
    private Vcnl4200SensorDriver proximitySensorDriver;

    public class MyBinder extends Binder {
        public ProximityService getService() {
            return ProximityService.this;
        }
    }

    public void onCreate(){
        PeripheralManager manager = PeripheralManager.getInstance();
        List<String> i2cBusList = manager.getI2cBusList();
        if (i2cBusList.isEmpty()) {
            Log.e(TAG, "No I2C bus available on this device.");
            return;
        } else {
            Log.i(TAG, "List of available devices: " + i2cBusList);
        }
        setupProximitySensor("I2C2");
    }

    public void onDestroy() {
        super.onDestroy();
        destroyProximitySensor();
    }

    public IBinder onBind(Intent intent) {
        Log.i(TAG, "onBind.");
        return binder;
    }

    public int onStartCommand(Intent intent, int flags, int startId) {
        return START_NOT_STICKY;
    }

    private void setupProximitySensor(String i2cBus) {
        try {
            proximitySensorDriver = new Vcnl4200SensorDriver(i2cBus);
            proximitySensorDriver.registerProximitySensor();     // 将传感器注册到系统
            proximitySensorDriver.registerLightSensor();
            Log.i(TAG, "Proximity and ambient light drivers registered");
        } catch (IOException e) {
            Log.e(TAG, "Error configuring sensor", e);
        }
    }

    private void destroyProximitySensor() {
        if (proximitySensorDriver != null) {
            proximitySensorDriver.unregisterProximitySensor();
            proximitySensorDriver.unregisterLightSensor();
            try {
                proximitySensorDriver.close();
            } catch (IOException e) {
                Log.e(TAG, "Error closing sensor", e);
            } finally {
                proximitySensorDriver = null;
            }
        }
    }
    /*public void calibrate(CalibrateCallback callback){
        proximitySensorDriver.calibrate(callback);
    }*/
}
