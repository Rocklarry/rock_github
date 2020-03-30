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
import android.content.SharedPreferences;

import com.a3nod.lenovo.sparrowfactory.tool.LogUtil;
import com.google.android.things.pio.I2cDevice;
import com.google.android.things.pio.PeripheralManager;

import java.io.IOException;

import static android.content.Context.MODE_PRIVATE;

/**
 * Driver for the BMA253 1.5g accelerometer.
 */
public class Bma253 implements AutoCloseable
{
    private static final String TAG = Bma253.class.getSimpleName();

    /**
     * I2C slave address of the BMA253.
     */
    public static final int I2C_ADDRESS = 0x18;
    public static final int MODE_STANDBY = 0; // i2c on, output off, low power
    public static final int MODE_ACTIVE = 1; // i2c on, output on
    static final float MAX_RANGE_G = 1.5f;
    static final float MAX_POWER_UA = 294.f; // at 120hz
    static final float MAX_FREQ_HZ = 120.f;
    static final float MIN_FREQ_HZ = 1.f;
    static final int CALIBRATE_TIME=20;


    public static final byte REG_BGW_CHIPID = 0x00;
    public static final byte REG_ACCD_X_LSB = 0x02;
    public static final byte REG_ACCD_X_MSB = 0x03;
    public static final byte REG_ACCD_Y_LSB = 0x04;
    public static final byte REG_ACCD_Y_MSB = 0x05;
    public static final byte REG_ACCD_Z_LSB = 0x06;
    public static final byte REG_ACCD_Z_MSB = 0x07;
    public static final byte REG_ACCD_TEMP = 0x08;
    public static final byte REG_INT_STATUS_0 = 0x09;
    public static final byte REG_INT_STATUS_1 = 0x0A;
    public static final byte REG_INT_STATUS_2 = 0x0B;
    public static final byte REG_INT_STATUS_3 = 0x0C;
    public static final byte REG_FIFO_STATUS = 0x0E;
    public static final byte REG_PMU_RANGE = 0x0F;
    public static final byte REG_PMU_BW = 0x10;
    public static final byte REG_PMU_LPW = 0x11;
    public static final byte REG_PMU_LOW_POWER = 0x12;
    public static final byte REG_ACCD_HBW = 0x13;
    public static final byte REG_BGW_SOFTRESET = 0x14;
    public static final byte REG_INT_EN_0 = 0x16;
    public static final byte REG_INT_EN_1 = 0x17;
    public static final byte REG_INT_EN_2 = 0x18;
    public static final byte REG_INT_MAP_0 = 0x19;
    public static final byte REG_INT_MAP_1 = 0x1A;
    public static final byte REG_INT_MAP_2 = 0x1B;
    public static final byte REG_INT_SRC = 0x1E;
    public static final byte REG_INT_OUT_CTRL = 0x20;
    public static final byte REG_INT_RST_LATCH = 0x21;
    public static final byte REG_INT_0 = 0x22;
    public static final byte REG_INT_1 = 0x23;
    public static final byte REG_INT_2 = 0x24;
    public static final byte REG_INT_3 = 0x25;
    public static final byte REG_INT_4 = 0x26;
    public static final byte REG_INT_5 = 0x27;
    public static final byte REG_INT_6 = 0x28;
    public static final byte REG_INT_7 = 0x29;
    public static final byte REG_INT_8 = 0x2A;
    public static final byte REG_INT_9 = 0x2B;
    public static final byte REG_INT_A = 0x2C;
    public static final byte REG_INT_B = 0x2D;
    public static final byte REG_INT_C = 0x2E;
    public static final byte REG_INT_D = 0x2F;
    public static final byte REG_FIFO_CONFIG_0 = 0x30;
    public static final byte REG_PMU_SELF_TEST = 0x32;
    public static final byte REG_TRIM_NVM_CTRL = 0x33;
    public static final byte REG_BGW_SPI3_WDT = 0x34;
    public static final byte REG_OFC_CTRL = 0x36;
    public static final byte REG_OFC_SETTING = 0x37;
    public static final byte REG_OFC_OFFSET_X = 0x38;
    public static final byte REG_OFC_OFFSET_Y = 0x39;
    public static final byte REG_OFC_OFFSET_Z = 0x3A;
    public static final byte REG_TRIM_GP0 = 0x3B;
    public static final byte REG_TRIM_GP1 = 0x3C;
    public static final byte REG_FIFO_CONFIG_1 = 0x3E;
    public static final byte REG_FIFO_DATA = 0x3F;

    public static final byte PMU_BW_7_81HZ = 0b1000;
    public static final byte PMU_BW_15_63HZ = 0b1001;
    public static final byte PMU_BW_31_25HZ = 0b1010;
    public static final byte PMU_BW_62_5HZ = 0b1011;
    public static final byte PMU_BW_125HZ = 0b1100;
    public static final byte PMU_BW_250HZ = 0b1101;
    public static final byte PMU_BW_500HZ = 0b1110;
    public static final byte PMU_BW_1000HZ = 0b1111;


    public static final byte PMU_RANGE_2G = 0b0011;
    public static final byte PMU_RANGE_4G = 0b0101;
    public static final byte PMU_RANGE_8G = 0b1000;
    public static final byte PMU_RANGE_16G = 0b1100;

    private static final float[] RANGE_TO_G_TABLE;

    static
    {
        RANGE_TO_G_TABLE = new float[16];
        RANGE_TO_G_TABLE[PMU_RANGE_2G] = 0.98f;
        RANGE_TO_G_TABLE[PMU_RANGE_4G] = 1.95f;
        RANGE_TO_G_TABLE[PMU_RANGE_8G] = 3.91f;
        RANGE_TO_G_TABLE[PMU_RANGE_16G] = 7.81f;
    }

    public static final byte CHIPID = (byte) 0xFA;
    private byte mPmuRange = PMU_RANGE_2G;
    private float gFactor = 0;
    private I2cDevice mDevice;

    private int[] Cailvalues = new int[3];
    private Context mContext;
    int range = 2;

    /**
     * Create a new BMA253 driver connected to the given I2C bus.
     *
     * @param bus
     * @throws IOException
     */
    public Bma253(String bus, Context context) throws IOException
    {
        PeripheralManager pioService = PeripheralManager.getInstance();
        I2cDevice device = pioService.openI2cDevice(bus, I2C_ADDRESS);
        try
        {
            this.mContext = context;
            connect(device);
        } catch (IOException | RuntimeException e)
        {
            try
            {
                close();
            } catch (IOException | RuntimeException ignored)
            {
            }
            throw e;
        }
    }

    /**
     * Create a new BMA253 driver connected to the given I2C device.
     *
     * @param device
     * @throws IOException
     */
    /*package*/ Bma253(I2cDevice device) throws IOException
    {
        connect(device);
    }

    private void connect(I2cDevice device) throws IOException
    {
        if (mDevice != null)
        {
            throw new IllegalStateException("device already connected");
        }
        mDevice = device;
        init();
    }

    /**
     * Close the driver and the underlying device.
     */
    @Override
    public void close() throws IOException
    {
        if (mDevice != null)
        {
            try
            {
                mDevice.close();
            } finally
            {
                mDevice = null;
            }
        }
    }

    public void init() throws IOException
    {
        byte chipid = mDevice.readRegByte(REG_BGW_CHIPID);
        if (chipid != CHIPID)
        {
            throw new IOException(String.format("wrong chip id. Expected %02X, found %02X", CHIPID, chipid));
        }
        mDevice.writeRegByte(REG_PMU_BW, PMU_BW_62_5HZ);
        mDevice.writeRegByte(REG_PMU_RANGE, mPmuRange);
        gFactor = RANGE_TO_G_TABLE[mPmuRange];

        switch (mPmuRange)
        {
            case PMU_RANGE_2G:
                range = 2;
                break;
            case PMU_RANGE_4G:
                range = 4;
                break;
            case PMU_RANGE_8G:
                range = 8;
                break;
            case PMU_RANGE_16G:
                range = 16;
                break;
        }
        SharedPreferences sharedPreferences = mContext.getSharedPreferences("Gsensor", MODE_PRIVATE);
        Cailvalues[0] = sharedPreferences.getInt("GX", 0);
        Cailvalues[1] = sharedPreferences.getInt("GY", 0);
        Cailvalues[2] = sharedPreferences.getInt("GZ", 32768 / range);
        LogUtil.i("init Cailvalues[0]: " + (-Cailvalues[0]) / 32768f * gFactor * range);
        LogUtil.i("init Cailvalues[1]: " + (-Cailvalues[1]) / 32768f * gFactor * range);
        LogUtil.i("init Cailvalues[2]: " + (Cailvalues[2]) / 32768f * gFactor * range);
    }

    /**
     * Read an accelerometer sample.
     *
     * @return acceleration over xyz axis in G.
     * @throws IOException
     * @throws IllegalStateException
     */
    public float[] readSample() throws IOException, IllegalStateException
    {
        if (mDevice == null)
        {
            throw new IllegalStateException("device not connected");
        }

        short x, y, z;
        x = mDevice.readRegWord(REG_ACCD_X_LSB);
        y = mDevice.readRegWord(REG_ACCD_Y_LSB);
        z = mDevice.readRegWord(REG_ACCD_Z_LSB);
        LogUtil.i("calibrateTimes: " + calibrateTimes);
        LogUtil.i("x  : " + (-x) / 32768d * gFactor * range);
        LogUtil.i("y  : " + (-y) / 32768d * gFactor * range);
        LogUtil.i("z  : " + (z) / 32768d * gFactor * range);
        if (isCalibrate)
        {
            if (calibrateTimes >= CALIBRATE_TIME)
            {
                isCalibrate = false;
                calibrateTimes = 0;
                if (callback != null)
                {
                    callback.onCalibrateFinish();
                }
                Cailvalues[0] = Cailvalues[0] / CALIBRATE_TIME;
                Cailvalues[1] = Cailvalues[1] / CALIBRATE_TIME;
                Cailvalues[2] = Cailvalues[2] / CALIBRATE_TIME;
                SharedPreferences sharedPreferences = mContext.getSharedPreferences("Gsensor", MODE_PRIVATE);
                SharedPreferences.Editor editor = sharedPreferences.edit();
                editor.putInt("GX", Cailvalues[0]);
                editor.putInt("GY", Cailvalues[1]);
                editor.putInt("GZ", Cailvalues[2]);
                editor.commit();
            } else
            {
                Cailvalues[0] += x;
                Cailvalues[1] += y;
                Cailvalues[2] += z;
            }
            calibrateTimes++;

            float fx = -x / 32768f * range;
            float fy = -y / 32768f * range;
            float fz = z / 32768f * range;
            return new float[]{fx, fy, fz};
        } else
        {
            float fx = -(x - Cailvalues[0]) / 32768f * range;
            float fy = -(y - Cailvalues[1]) / 32768f * range;
            float fz = (z + 32768 / range - Cailvalues[2]) / 32768f * range /*/gFactor*/;
            LogUtil.i("aaaaaaaaaaaaaaa zzzzzzz :" + (32768 / range - Cailvalues[2]) / 32768f * range);
            return new float[]{fx, fy, fz};
        }
    }

    boolean isCalibrate = false;
    public volatile int calibrateTimes = 0;
    public CalibrateCallback callback;

    public void startCalibrate(CalibrateCallback callback)
    {
        this.callback = callback;
        isCalibrate = true;
        calibrateTimes = 0;
        Cailvalues[0] = 0;
        Cailvalues[1] = 0;
        Cailvalues[2] = 0;
    }

    /**
     * Read the temperature.
     *x
     * @return temperature in Celcius.
     * @throws IOException
     * @throws IllegalStateException
     */
    public float[] readTemperature() throws IOException, IllegalStateException
    {
        if (mDevice == null)
        {
            throw new IllegalStateException("device not connected");
        }
        float temp = mDevice.readRegWord(REG_ACCD_TEMP) * 0.5f + 23;
        return new float[]{temp};
    }

    public float readX() throws IOException, IllegalStateException
    {
        if (mDevice == null)
        {
            throw new IllegalStateException("device not connected");
        }
        float temp = mDevice.readRegWord(REG_ACCD_X_LSB);
        return temp;
    }

    public float readY() throws IOException, IllegalStateException
    {
        if (mDevice == null)
        {
            throw new IllegalStateException("device not connected");
        }
        float temp = mDevice.readRegWord(REG_ACCD_Y_LSB);
        return temp;
    }

    public float readZ() throws IOException, IllegalStateException
    {
        if (mDevice == null)
        {
            throw new IllegalStateException("device not connected");
        }
        float temp = mDevice.readRegWord(REG_ACCD_Z_LSB);
        return temp;
    }
}
