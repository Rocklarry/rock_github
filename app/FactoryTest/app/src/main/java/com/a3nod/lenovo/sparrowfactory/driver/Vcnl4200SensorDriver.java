package com.a3nod.lenovo.sparrowfactory.driver;

/*
 * Copyright 2016 Google Inc.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

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

public class Vcnl4200SensorDriver implements AutoCloseable {
    private static final String TAG = "Vcnl4200SensorDriver";

    // DRIVER parameters
    // documented at https://source.android.com/devices/sensors/hal-interface.html#sensor_t
    private static final String DRIVER_VENDOR = "Google";
    private static final String DRIVER_NAME_PREFIX = "VCNL 4200 ";

    private Vcnl4200 mDevice;

    private ProximitySensorDriver mProximitySensorDriver;
    private AmbientLightSensorDriver mAmbientLightSensorDriver;

    /**
     * Create a new framework sensor driver connected on the given bus.
     * The driver emits {@link Sensor} with pressure and temperature data when
     * registered.
     *
     * @param bus I2C bus the sensor is connected to.
     * @throws IOException
     * @see #registerProximitySensor()
     * @see #registerAmbientLightSensor()
     */
    public Vcnl4200SensorDriver(String bus) throws IOException {
        mDevice = new Vcnl4200(bus);
    }

    /**
     * Close the driver and the underlying device.
     *
     * @throws IOException
     */
    @Override
    public void close() throws IOException {
        unregisterProximitySensor();
        unregisterAmbientLightSensor();
        if (mDevice != null) {
            try {
                mDevice.close();
            } finally {
                mDevice = null;
            }
        }
    }

    /**
     * Register a {@link UserSensor} that pipes temperature readings into the Android SensorManager.
     *
     * @see #unregisterProximitySensor()
     */
    public void registerProximitySensor() {
        if (mDevice == null) {
            throw new IllegalStateException("cannot register closed driver");
        }

        if (mProximitySensorDriver == null) {
            mProximitySensorDriver = new ProximitySensorDriver();
            UserDriverManager.getInstance().registerSensor(mProximitySensorDriver.getUserSensor());
        }
    }

    /**
     * Register a {@link UserSensor} that pipes ambient light readings into the Android SensorManager.
     *
     * @see #unregisterAmbientLightSensor()
     */
    public void registerAmbientLightSensor() {
        if (mDevice == null) {
            throw new IllegalStateException("cannot register closed driver");
        }

        if (mAmbientLightSensorDriver == null) {
            mAmbientLightSensorDriver = new AmbientLightSensorDriver();
            UserDriverManager.getInstance().registerSensor(mAmbientLightSensorDriver.getUserSensor());
        }
    }

    /**
     * Unregister the proximity {@link UserSensor}.
     */
    public void unregisterProximitySensor() {
        if (mProximitySensorDriver != null) {
            UserDriverManager.getInstance().unregisterSensor(mProximitySensorDriver.getUserSensor());
            mProximitySensorDriver = null;
        }
    }

    /**
     * Unregister the proximity {@link UserSensor}.
     */
    public void unregisterAmbientLightSensor() {
        if (mAmbientLightSensorDriver != null) {
            UserDriverManager.getInstance().unregisterSensor(mAmbientLightSensorDriver.getUserSensor());
            mAmbientLightSensorDriver = null;
        }
    }

    public void calibrate(CalibrateCallback callback)
    {
        mDevice.startCalibrate(callback);
    }

    private class ProximitySensorDriver implements UserSensorDriver {
        // DRIVER parameters
        // documented at https://source.android.com/devices/sensors/hal-interface.html#sensor_t
        private final String DRIVER_NAME = "Proximity Sensor";
        private final float DRIVER_MAX_RANGE = 15000f;
        private final float DRIVER_RESOLUTION = 20f;
        private final int DRIVER_VERSION = 1;
        private final String DRIVER_REQUIRED_PERMISSION = "";
        private final int DRIVER_MIN_DELAY_US = (int)TimeUnit.SECONDS.toMicros(1);
        private final int DRIVER_MAX_DELAY_US = (int)TimeUnit.SECONDS.toMicros(2);
//        private final int DRIVER_MIN_DELAY_US = (int)TimeUnit.MILLISECONDS.toMicros(200);
//        private final int DRIVER_MAX_DELAY_US = (int)TimeUnit.MILLISECONDS.toMicros(500);

        private UserSensor mUserSensor;

        private UserSensor getUserSensor() {
            if (mUserSensor == null) {
                mUserSensor = new UserSensor.Builder()
                        .setType(Sensor.TYPE_PROXIMITY)
                        .setName(DRIVER_NAME_PREFIX + DRIVER_NAME)
                        .setVendor(DRIVER_VENDOR)
                        .setVersion(DRIVER_VERSION)
//                        .setMaxRange(DRIVER_MAX_RANGE)
                        .setMinDelay(DRIVER_MIN_DELAY_US)
//                        .setRequiredPermission(DRIVER_REQUIRED_PERMISSION)
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

    private class AmbientLightSensorDriver implements UserSensorDriver {
        // DRIVER parameters
        // documented at https://source.android.com/devices/sensors/hal-interface.html#sensor_t
        private final String DRIVER_NAME = "Ambient Light Sensor";
        private final float DRIVER_MAX_RANGE = 15000f;
        private final float DRIVER_RESOLUTION = 400f;
        private final int DRIVER_VERSION = 1;
        private final String DRIVER_REQUIRED_PERMISSION = "";
//        private final int DRIVER_MIN_DELAY_US = (int)TimeUnit.SECONDS.toMicros(5);
//        private final int DRIVER_MAX_DELAY_US = (int)TimeUnit.SECONDS.toMicros(10);
        private final int DRIVER_MIN_DELAY_US = (int)TimeUnit.SECONDS.toMicros(1);
        private final int DRIVER_MAX_DELAY_US = (int)TimeUnit.SECONDS.toMicros(2);

        private UserSensor mUserSensor;

        private UserSensor getUserSensor() {
            if (mUserSensor == null) {
                mUserSensor = new UserSensor.Builder()
                        .setType(Sensor.TYPE_LIGHT)
                        .setName(DRIVER_NAME_PREFIX + DRIVER_NAME)
                        .setVendor(DRIVER_VENDOR)
                        .setVersion(DRIVER_VERSION)
                        .setMaxRange(DRIVER_MAX_RANGE)
                        .setMinDelay(DRIVER_MIN_DELAY_US)
//                        .setRequiredPermission(DRIVER_REQUIRED_PERMISSION)
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
            Log.v(TAG, "Reading ambient sensor " + mDevice.readAmbient());
            return new UserSensorReading(new float[]{mDevice.readAmbient()}, SensorManager.SENSOR_STATUS_ACCURACY_MEDIUM);
        }

        @Override
        public void setEnabled(boolean enabled) throws IOException {
            Log.i(TAG, "Ambient light sensor setEnabled: " + enabled);
            mDevice.enableAmbientLightSensor(enabled);
        }
    }

}
