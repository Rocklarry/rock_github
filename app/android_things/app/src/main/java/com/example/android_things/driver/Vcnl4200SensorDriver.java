package com.example.android_things.driver;

import android.hardware.Sensor;
import android.hardware.SensorManager;
import android.util.Log;

import com.google.android.things.userdriver.UserDriverManager;
import com.google.android.things.userdriver.sensor.UserSensor;
import com.google.android.things.userdriver.sensor.UserSensorDriver;
import com.google.android.things.userdriver.sensor.UserSensorReading;

import java.io.IOException;
import java.util.UUID;
import java.util.concurrent.TimeUnit;

class Vcnl4200SensorDriver implements AutoCloseable{
    private static final String TAG = "Vcnl4200SensorDriver";
    private static final String DRIVER_VENDOR = "Google";
    private static final String DRIVER_NAME_PREFIX = "VCNL 4200 ";

    private Vcnl4200 mDevice;
    private ProximitySensorDriver mProximitySensorDriver;
    private LightSensorDriver mLightSensorDriver;

    public Vcnl4200SensorDriver(String bus) throws IOException {
        mDevice = new Vcnl4200(bus);
    }

    @Override
    public void close() throws IOException {
        unregisterProximitySensor();
        unregisterLightSensor();

        if (mDevice != null) {
            try {
                mDevice.close();
            } finally {
                mDevice = null;
            }
        }
    }

    /////////////////////////////////   P - SENSOR          /////////////////////////////////////////
    public void registerProximitySensor() {
        if (mDevice == null) {
            throw new IllegalStateException("cannot register closed driver");
        }
        if (mProximitySensorDriver == null) {
            mProximitySensorDriver = new ProximitySensorDriver();
            UserDriverManager.getInstance().registerSensor(mProximitySensorDriver.getUserSensor());
        }
    }

    public void unregisterProximitySensor() {
        if (mProximitySensorDriver != null) {
            UserDriverManager.getInstance().unregisterSensor(mProximitySensorDriver.getUserSensor());
            mProximitySensorDriver = null;
        }
    }

/*
    public void calibrate(CalibrateCallback callback)
    {
        mDevice.startCalibrate(callback);
    }*/

    private class ProximitySensorDriver implements UserSensorDriver {

        private final String DRIVER_NAME = "Proximity Sensor";
        private final float DRIVER_MAX_RANGE = 15000f;
        private final float DRIVER_RESOLUTION = 20f;
        private final int DRIVER_VERSION = 1;
        private final int DRIVER_MIN_DELAY_US = (int)TimeUnit.MILLISECONDS.toMicros(1);
        private final int DRIVER_MAX_DELAY_US = (int)TimeUnit.MILLISECONDS.toMicros(5);

        private UserSensor mUserSensor;

        private UserSensor getUserSensor() {
            if (mUserSensor == null) {
                mUserSensor = new UserSensor.Builder()
                        .setType(Sensor.TYPE_PROXIMITY)
                        .setName(DRIVER_NAME_PREFIX + DRIVER_NAME)
                        .setVendor(DRIVER_VENDOR)
                        .setVersion(DRIVER_VERSION)
                        .setMaxRange(DRIVER_MAX_RANGE)
                        .setMinDelay(DRIVER_MIN_DELAY_US)
                        .setResolution(DRIVER_RESOLUTION)
                        .setMaxDelay(DRIVER_MAX_DELAY_US)
                        .setUuid(UUID.randomUUID())
                        .setDriver(this)
                        .build();
            }
            return mUserSensor;
        }

        @Override
        public UserSensorReading read() throws IOException {
            Log.v(TAG, "Reading proximity sensor " + mDevice.readProximity());
            return new UserSensorReading(new float[]{mDevice.readProximity()}, SensorManager.SENSOR_STATUS_ACCURACY_MEDIUM);
        }

        @Override
        public void setEnabled(boolean enabled) throws IOException {
            Log.i(TAG, "Proximity sensor setEnabled: " + enabled);
            mDevice.enableProximitySensor(enabled);
        }
    }

    /////////////////////////////////   L - SENSOR          /////////////////////////////////////////

    public void registerLightSensor(){
        if(mLightSensorDriver == null){
            mLightSensorDriver = new LightSensorDriver();
            UserDriverManager.getInstance().registerSensor(mLightSensorDriver.getUserSensor());
        }
    }
    public void  unregisterLightSensor(){
        if(mLightSensorDriver != null){
            UserDriverManager.getInstance().unregisterSensor(mLightSensorDriver.getUserSensor());
            mLightSensorDriver = null;
        }

    }

    private class LightSensorDriver implements UserSensorDriver {
        private final String DRIVER_NAME = "Light Sensor";
        private final float DRIVER_MAX_RANGE = 15000f;
        private final float DRIVER_RESOLUTION = 400f;
        private final int DRIVER_VERSION = 1;
        private final int DRIVER_MIN_DELAY_US = (int)TimeUnit.SECONDS.toMicros(1);
        private final int DRIVER_MAX_DELAY_US = (int)TimeUnit.SECONDS.toMicros(2);

        private UserSensor mUserSensor;
        private UserSensor getUserSensor(){
            if(mUserSensor == null){
                mUserSensor = new UserSensor.Builder()
                        .setType(Sensor.TYPE_LIGHT)
                        .setName(DRIVER_NAME_PREFIX + DRIVER_NAME)
                        .setVendor(DRIVER_VENDOR)
                        .setVersion(DRIVER_VERSION)
                        .setMaxRange(DRIVER_MAX_RANGE)
                        .setMinDelay(DRIVER_MIN_DELAY_US)
                        .setResolution(DRIVER_RESOLUTION)
                        .setMaxDelay(DRIVER_MAX_DELAY_US)
                        .setUuid(UUID.randomUUID())
                        .setDriver(this)
                        .build();
            }
            return mUserSensor;
        }

        public UserSensorReading read() throws IOException {
            return new UserSensorReading( new float[]{mDevice.readLight()},SensorManager.SENSOR_STATUS_ACCURACY_MEDIUM);
        }

        public void setEnabled(boolean enable)throws IOException{
            mDevice.enableLightSensor(enable);
        }
    }

}
