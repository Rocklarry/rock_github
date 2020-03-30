package com.a3nod.lenovo.sparrowfactory.driver;

import android.hardware.Sensor;
import android.util.Log;

import com.google.android.things.userdriver.UserDriverManager;
import com.google.android.things.userdriver.sensor.UserSensor;
import com.google.android.things.userdriver.sensor.UserSensorDriver;
import com.google.android.things.userdriver.sensor.UserSensorReading;

import java.io.IOException;
import java.util.UUID;
import java.util.concurrent.TimeUnit;

/**
 * This class  is used to register or unregister bh1745 RGB sensors.
 * In this class,the RGBSensorDriver internal class implements the UserSensorDriver class.
 */
public class Bh1745SensorDriver implements AutoCloseable {

    private static final String TAG = "Bh1745SensorDriver";

    private static final String DRIVER_VENDOR = "3nod";
    private static final String DRIVER_NAME_PREFIX = "Bh 1745";

    private Bh1745 mDevice;

    private RGBSensorDriver mRGBSensorDriver;

    private static final int TYPE_RGB_SENSOR =0x10001;// the custom rgb sensor type

    private static final String STRING_RGB_SENEOR = "com.3nod.rgb_sensor";
    /**
     * Create a new framework sensor driver connected on the given bus.
     * The driver emits with brightness and color temperature data when registered.
     * @param bus I2C bus the sensor is connected to.
     * @see #registerRGBSensor()
     */
    public Bh1745SensorDriver(String bus) throws IOException {
        mDevice = new Bh1745(bus);
    }

    /**
     * Close the driver and the underlying device.
     */
    @Override
    public void close() throws IOException {
        unregisterRGBSensor();
        if (mDevice != null) {
            try {
                mDevice.close();
            } finally {
                mDevice = null;
            }
        }
    }

    /**
     * Register a {@link UserSensor} that color temperature and brightness readings into the Android SensorManager.
     *
     * @see #unregisterRGBSensor()
     */
    public void registerRGBSensor() {
        if (mDevice == null) {
            throw new IllegalStateException("cannot register closed driver");
        }
        Log.d(TAG,"--------registerRGBSensor()------");
        if (mRGBSensorDriver == null) {
            mRGBSensorDriver = new RGBSensorDriver();
            UserDriverManager.getInstance().registerSensor(mRGBSensorDriver.getUserSensor());
            Log.d(TAG,"--------222------");
        }
    }

    /**
     * Unregister the rgbsensor {@link UserSensor}.
     */
    public void unregisterRGBSensor() {
        if (mRGBSensorDriver != null) {
            UserDriverManager.getInstance().unregisterSensor(mRGBSensorDriver.getUserSensor());
            mRGBSensorDriver = null;
        }
    }

    private class RGBSensorDriver implements UserSensorDriver {
        private final String DRIVER_NAME = "RGB Sensor";
        private final int DRIVER_VERSION = 1;
        private final int DRIVER_MIN_DELAY_US = (int) TimeUnit.MILLISECONDS.toMicros(160);
        private final int DRIVER_MAX_DELAY_US = (int) TimeUnit.MILLISECONDS.toMicros(5120);

        private UserSensor mUserSensor;

        private UserSensor getUserSensor() {
            if (mUserSensor == null) {
                mUserSensor = new UserSensor.Builder()
                        .setCustomType(TYPE_RGB_SENSOR,STRING_RGB_SENEOR,Sensor.REPORTING_MODE_CONTINUOUS)
                        .setName(DRIVER_NAME_PREFIX + DRIVER_NAME)
                        .setVendor(DRIVER_VENDOR)
                        .setVersion(DRIVER_VERSION)
                        .setMinDelay(DRIVER_MIN_DELAY_US)
                        //.setResolution(DRIVER_RESOLUTION)
                        .setMaxDelay(DRIVER_MAX_DELAY_US)
                        .setUuid(UUID.randomUUID())
                        .setDriver(this)
                        .build();
            }
            return mUserSensor;
        }

        @Override
        public UserSensorReading read() throws IOException {
            float lightValue = 0.0f;
            float colorTempValue = 0.0f;
            int red = 0;
            int green= 0;
            int blue = 0;
            if (mDevice.isVaildData()){
                red = mDevice.readRedValue();
                green = mDevice.readGreenValue();
                blue = mDevice.readBlueValue();
                int clear = mDevice.readClearValue();
                lightValue = mDevice.getLightValue(red,green,clear);
                colorTempValue = mDevice.getColorTemperature(red,green,blue,clear);
                Log.d(TAG," red ="+red+",green ="+green+",blue ="+blue+" ,lightValue ="+lightValue +",colorValue ="+colorTempValue);
            }else{
                Log.d(TAG,"RGBSensor data is not vaild");
            }
            return new UserSensorReading(new float[]{red,green,blue,lightValue,colorTempValue});
        }

        @Override
        public void setEnabled(boolean enabled) throws IOException {
            Log.i(TAG, "RGB sensor setEnabled: " + enabled);
            mDevice.enableRGBSensor(enabled);
        }
    }

}
