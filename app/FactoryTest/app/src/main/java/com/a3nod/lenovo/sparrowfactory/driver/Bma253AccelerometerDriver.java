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

package com.a3nod.lenovo.sparrowfactory.driver;

import android.content.Context;
import android.hardware.Sensor;
import android.hardware.SensorManager;

import com.google.android.things.userdriver.UserDriverManager;
import com.google.android.things.userdriver.sensor.UserSensor;
import com.google.android.things.userdriver.sensor.UserSensorDriver;
import com.google.android.things.userdriver.sensor.UserSensorReading;

import java.io.IOException;
import java.util.UUID;
import java.util.concurrent.TimeUnit;

public class Bma253AccelerometerDriver implements AutoCloseable {
    private static final String TAG = Bma253AccelerometerDriver.class.getSimpleName();
    private static final String DRIVER_NAME = "BMA220";
    private static final String DRIVER_VENDOR = "Bosch";
    private static final float DRIVER_MAX_RANGE = Bma253.MAX_RANGE_G * SensorManager.GRAVITY_EARTH;
    private static final float DRIVER_RESOLUTION = DRIVER_MAX_RANGE / 32.f; // 6bit signed
    private static final float DRIVER_POWER = Bma253.MAX_POWER_UA / 1000.f;
    private static final int DRIVER_MIN_DELAY_US = Math.round(1000000.f/Bma253.MAX_FREQ_HZ);
    private static final int DRIVER_MAX_DELAY_US = Math.round(1000000.f/Bma253.MIN_FREQ_HZ);
    private static final int DRIVER_VERSION = 1;
    private static final String DRIVER_REQUIRED_PERMISSION = "";
    private Bma253 mDevice;
    private UserSensor mAccelerometerSensor;
    private UserSensor mTemperatureSensor;

    /**
     * Create a new framework accelerometer driver connected to the given I2C bus.
     * The driver emits {@link Sensor} with acceleration data when registered.
     * @param bus
     * @throws IOException
     * @see #register()
     */
    public Bma253AccelerometerDriver(String bus, Context context) throws IOException {
        mDevice = new Bma253(bus,context);
    }

    public void calibrate(CalibrateCallback callback)
    {
        if(mDevice!=null)
        {
            mDevice.startCalibrate(callback);
        }
    }
    /**
     * Close the driver and the underlying device.
     * @throws IOException
     */
    @Override
    public void close() throws IOException {
        unregister();
        if (mDevice != null) {
            try {
                mDevice.close();
            } finally {
                mDevice = null;
            }
        }
    }

    /**
     * Register the driver in the framework.
     * @see #unregister()
     */
    public void register() {
        if (mDevice == null) {
            throw new IllegalStateException("cannot registered closed driver");
        }
        UserDriverManager manager = UserDriverManager.getInstance();
        if (mAccelerometerSensor == null) {
            mAccelerometerSensor = buildAccelerometer(mDevice);
            manager.registerSensor(mAccelerometerSensor);
        }
        if (mTemperatureSensor == null) {
            mTemperatureSensor = buildTemperatureSensor(mDevice);
            manager.registerSensor(mTemperatureSensor);
        }
    }

    /**
     * Unregister the driver from the framework.
     */
    public void unregister() {
        UserDriverManager manager = UserDriverManager.getInstance();
        if (mAccelerometerSensor != null) {
            manager.unregisterSensor(mAccelerometerSensor);
            mAccelerometerSensor = null;
        }
        if (mTemperatureSensor != null) {
            manager.unregisterSensor(mTemperatureSensor);
            mTemperatureSensor = null;
        }
    }

    static UserSensor buildAccelerometer(final Bma253 bma253) {
        return new UserSensor.Builder()
                .setType(Sensor.TYPE_ACCELEROMETER)
                .setName(DRIVER_NAME)
                .setVendor(DRIVER_VENDOR)
                .setVersion(DRIVER_VERSION)
                .setMaxRange(DRIVER_MAX_RANGE)
                .setResolution(DRIVER_RESOLUTION)
                .setPower(DRIVER_POWER)
                .setMinDelay(DRIVER_MIN_DELAY_US)
//                .setRequiredPermission(DRIVER_REQUIRED_PERMISSION)
                .setMaxDelay(DRIVER_MAX_DELAY_US)
                .setUuid(UUID.randomUUID())
                .setDriver(new UserSensorDriver() {
                    @Override
                    public UserSensorReading read() throws IOException {
                        float[] sample = bma253.readSample();
                        for (int i=0; i<sample.length; i++) {
                            sample[i] = sample[i] * SensorManager.GRAVITY_EARTH;
                        }
                        return new UserSensorReading(
                                sample,
                                SensorManager.SENSOR_STATUS_ACCURACY_HIGH); // 120Hz
                    }

                    @Override
                    public void setEnabled(boolean enabled) throws IOException {
                    }
                })
                .build();
    }

    static UserSensor buildTemperatureSensor(final Bma253 bma253) {
        return new UserSensor.Builder()
                .setType(Sensor.TYPE_AMBIENT_TEMPERATURE)
                .setName(DRIVER_NAME)
                .setVendor(DRIVER_VENDOR)
                .setVersion(DRIVER_VERSION)
                .setMaxRange(80)
                .setResolution(DRIVER_RESOLUTION)
                .setPower(DRIVER_POWER)
                .setMinDelay((int)TimeUnit.SECONDS.toMicros(5))
//                .setRequiredPermission(DRIVER_REQUIRED_PERMISSION)
                .setMaxDelay((int)TimeUnit.SECONDS.toMicros(10))
                .setUuid(UUID.randomUUID())
                .setDriver(new UserSensorDriver() {
                    @Override
                    public UserSensorReading read() throws IOException {
                        return new UserSensorReading(
                                bma253.readTemperature(),
                                SensorManager.SENSOR_STATUS_ACCURACY_HIGH);
                    }

                    @Override
                    public void setEnabled(boolean enabled) throws IOException {
                    }
                })
                .build();
    }
}
