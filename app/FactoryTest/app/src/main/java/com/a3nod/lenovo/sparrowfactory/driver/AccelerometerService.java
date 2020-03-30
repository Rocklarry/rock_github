package com.a3nod.lenovo.sparrowfactory.driver;

import android.app.Service;
import android.content.Context;
import android.content.Intent;
import android.os.Binder;
import android.os.IBinder;
import android.util.Log;


import com.google.android.things.pio.PeripheralManager;

import java.io.IOException;
import java.util.List;

/**
 * To use this service, start it from your component (like an activity):
 * <pre>{@code
 * this.startService(new Intent(this, AccellerometerService.class))
 * }</pre>
 */
public class AccelerometerService extends Service
{
    private static final String TAG = AccelerometerService.class.getSimpleName();

    private Bma253AccelerometerDriver accellerometerSensorDriver;

    @Override
    public void onCreate()
    {
        PeripheralManager manager = PeripheralManager.getInstance();
        List<String> i2cBusList = manager.getI2cBusList();
        if (i2cBusList.isEmpty())
        {
            Log.e(TAG, "No I2C bus available on this device.");
            return;
        } else
        {
            Log.i(TAG, "List of available devices: " + i2cBusList);
        }

//        setupAccellerometerSensor(i2cBusList.get(0));
        setupAccellerometerSensor("I2C2", getApplicationContext());
    }

    public void calibrate(CalibrateCallback callback)
    {
        if(accellerometerSensorDriver!=null){
            accellerometerSensorDriver.calibrate(callback);
        }
    }

    @Override
    public void onDestroy()
    {
        super.onDestroy();
        destroyAccellerometerSensor();
    }

    @Override
    public IBinder onBind(Intent intent)
    {
        Log.i(TAG, "onBind.");
        return new MyBinder(this);
    }

    public static class MyBinder extends Binder
    {
        private AccelerometerService service;
        public MyBinder(AccelerometerService service){
            this.service=service;
        }
        public AccelerometerService getService(){
            return service;
        }
    }

    @Override
    public int onStartCommand(Intent intent, int flags, int startId)
    {
        return START_NOT_STICKY;
    }

    private void setupAccellerometerSensor(String i2cBus, Context context)
    {
        try
        {
            accellerometerSensorDriver = new Bma253AccelerometerDriver(i2cBus, context);
            accellerometerSensorDriver.register();
            Log.i(TAG, "Accelerometer driver registered");
        } catch (IOException e)
        {
            Log.e(TAG, String.format("Error configuring accelerometer sensor: %s", e.getMessage()), e);
        }
    }

    private void destroyAccellerometerSensor()
    {
        if (accellerometerSensorDriver != null)
        {
            accellerometerSensorDriver.unregister();
            try
            {
                accellerometerSensorDriver.close();
            } catch (Exception e)
            {
                Log.e(TAG, "Error closing sensor", e);
            } finally
            {
                accellerometerSensorDriver = null;
            }
        }
    }

}
