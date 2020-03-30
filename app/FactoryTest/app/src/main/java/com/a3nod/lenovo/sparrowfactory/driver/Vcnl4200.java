package com.a3nod.lenovo.sparrowfactory.driver;

import android.util.Log;

import com.a3nod.lenovo.sparrowfactory.tool.LogUtil;
import com.google.android.things.pio.I2cDevice;
import com.google.android.things.pio.PeripheralManager;

import java.io.IOException;

public class Vcnl4200
{
    // Register addresses from the datasheet.
    // _L indicates Low byte only, otherwise the whole word
    public static final int REG_ALS_CONF_L = 0x00; // ALS integration time, persistence, interrupt, and function enable / disable
    public static final int REG_ALS_THDH = 0x01; // ALS high interrupt threshold
    public static final int REG_ALS_THDL = 0x02; // ALS low interrupt threshold
    public static final int REG_PS_CONF1_L = 0x03; // PS duty ratio, integration time, persistence, and PS enable / disable
    public static final int REG_PS_CONF1_CONF2 = 0x03; // CONF1 and PS_HD, PS interrupt trigger method
    public static final int REG_PS_CONF3_L = 0x04; // PS multi pulse, active force mode, enable sunlight cancellation
    public static final int REG_PS_CONF3_MS = 0x04; // CONF3 and PS mode selection, sunlight capability, sunlight protection mode
    public static final int REG_PS_CANC = 0x05; // PS cancellation level setting
    public static final int REG_PS_THDL = 0x06; // PS low interrupt threshold setting
    public static final int REG_PS_THDH = 0x07; // PS high interrupt threshold setting
    public static final int REG_PS_DATA = 0x08; // Proximity Sensor data
    public static final int REG_ALS_DATA = 0x09; // Ambient Light Sensor data
    public static final int REG_WHITE_DATA = 0x0A; // White data
    public static final int REG_INT_FLAG = 0x0D; // ALS, PS interrupt flags
    public static final int REG_ID = 0x0E; // Device ID
    // Ambient Light Sensor Integration Time in Milliseconds
    public static final byte ALS_IT_MS_50 = 0;
    public static final byte ALS_IT_MS_100 = 64;
    public static final byte ALS_IT_MS_200 = (byte) 128;
    public static final byte ALS_IT_MS_400 = (byte) 192;
    public static final byte ALS_SD_POWER_ON = 0x00;
    public static final byte ALS_SD_SHUTDOWN = 0x01;
    public static final byte CONF1_PS_DUTY_160 = 0x00;
    public static final byte CONF1_PS_DUTY_320 = 0x40;
    public static final byte CONF1_PS_DUTY_640 = (byte) 0x80;
    public static final byte CONF1_PS_DUTY_1280 = (byte) 0xC0;
    public static final byte CONF1_PS_IT_1T = 0x00;
    public static final byte CONF1_PS_IT_15T = 0x02;
    public static final byte CONF1_PS_IT_2T = 0x04;
    public static final byte CONF1_PS_IT_4T = 0x06;
    public static final byte CONF1_PS_IT_8T = 0x08;
    public static final byte CONF1_PS_IT_9T = 0x0A;
    public static final byte CONF1_PS_SD_POWER_ON = 0x00;
    public static final byte CONF1_PS_SD_SHUT_DOWN = 0x01;
    public static final byte CONF2_PS_HD_12_BIT = 0x00;
    public static final byte CONF2_PS_HD_16_BIT = 0x08;
    // Proximity multi-pulse numbers
    public static final byte CONF3_PS_MPS_1 = 0x00;
    public static final byte CONF3_PS_MPS_2 = 0x20;
    public static final byte CONF3_PS_MPS_4 = 0x40;
    public static final byte CONF3_PS_MPS_8 = 0x60;
    // Proximity sensor smart persistence
    public static final byte CONF3_PS_SMART_PERS_DISABLE = 0x00;
    public static final byte CONF3_PS_SMART_PERS_ENABLE = 0x10;
    public static final byte MS_LED_I_MA_50 = 0x00;
    public static final byte MS_LED_I_MA_75 = 0x01;
    public static final byte MS_LED_I_MA_100 = 0x02;
    public static final byte MS_LED_I_MA_120 = 0x03;
    public static final byte MS_LED_I_MA_140 = 0x04;
    public static final byte MS_LED_I_MA_160 = 0x05;
    public static final byte MS_LED_I_MA_180 = 0x06;
    public static final byte MS_LED_I_MA_200 = 0x07;
    public static final byte INT_FLAG_PS_UPFLAG = (byte) 0x80; // PS code saturation flag
    public static final byte INT_FLAG_PS_SPFLAG = 0x40; // PS enter sunlight protection flag
    public static final byte INT_FLAG_ALS_IF_L = 0x20; // ALS crossing low THD INT trigger event
    public static final byte INT_FLAG_ALS_IF_H = 0x10; // ALS crossing high THD INT trigger event
    public static final byte INT_FLAG_PS_IF_CLOSE = 0x40; // PS rise above PS_THDH INT trigger event
    public static final byte INT_FLAG_PS_IF_AWAY = 0x20; // PS drop below PS_THDL INT trigger event
    private static final String TAG = "Vcnl4200";
    private static final int I2C_ADDRESS = 0x51;
    private static final int CHIP_ID = 0x1058;
    private I2cDevice mDevice;
    private int mChipId;
    private CalibrateCallback callback;
    private static final int CALIBRATE_TIME=20;

    /**
     * Create a new VCNL 4200 sensor driver connected on the given bus.
     *
     * @param bus I2C bus the sensor is connected to.
     * @throws IOException
     */
    public Vcnl4200(String bus) throws IOException
    {
        PeripheralManager managerService = PeripheralManager.getInstance();
        I2cDevice device = managerService.openI2cDevice(bus, I2C_ADDRESS);
        try
        {
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
     * Create a new VCNL 4200 sensor driver connected to the given I2c device.
     *
     * @param device I2C device of the sensor.
     * @throws IOException
     */
    /*package*/  Vcnl4200(I2cDevice device) throws IOException
    {
        connect(device);
    }

    public void close() throws IOException
    {
        mDevice.close();
    }

    public void connect(I2cDevice device) throws IOException
    {
        mDevice = device;

        mChipId = mDevice.readRegWord(REG_ID) & 0xffff;
        // Sanity check
        if (mChipId != CHIP_ID)
        {
            Log.e(TAG, String.format("Wrong chip ID: %X", mChipId));
            throw new IOException("VCNL 4200 sensor not found");
        }
    }

    public void enableProximitySensor(boolean enabled) throws IOException
    {
        if (enabled) {
            mDevice.writeRegWord(REG_PS_CONF1_CONF2,
                    regWord(CONF1_PS_DUTY_1280 | CONF1_PS_IT_9T | CONF1_PS_SD_POWER_ON,
                            CONF2_PS_HD_16_BIT));
            mDevice.writeRegWord(REG_PS_CONF3_MS, regWord(CONF3_PS_MPS_8, MS_LED_I_MA_200));
        } else {
            mDevice.writeRegByte(REG_PS_CONF1_L, CONF1_PS_SD_SHUT_DOWN);
        }
    }

    public void enableAmbientLightSensor(boolean enabled) throws IOException
    {
        if (enabled)
        {
            mDevice.writeRegByte(REG_ALS_CONF_L, (byte) (ALS_IT_MS_200 | ALS_SD_POWER_ON));
        } else
        {
            mDevice.writeRegByte(REG_ALS_CONF_L, ALS_SD_SHUTDOWN);
        }
    }

    private void configureProximitySensor() throws IOException
    {
        mDevice.writeRegWord(REG_PS_CONF1_CONF2, regWord(CONF1_PS_DUTY_320 | CONF1_PS_IT_2T, CONF2_PS_HD_16_BIT));
        mDevice.writeRegWord(REG_PS_CONF3_MS, regWord(CONF3_PS_MPS_4, MS_LED_I_MA_200));
    }

    private void configureAmbientLightSensor() throws IOException
    {
        mDevice.writeRegByte(REG_ALS_CONF_L, ALS_IT_MS_200);
    }


    public short readProximity() throws IOException
    {
        short distance = mDevice.readRegWord(REG_PS_DATA);
        if (isCalibrate)
        {
            if (calibrateTimes>=CALIBRATE_TIME)
            {
                isCalibrate=false;
                calibrateTimes=0;
                if (callback != null)
                {
                    callback.onCalibrateFinish();
                }
                mDevice.writeRegByte(REG_PS_CANC, (byte)(calibrateValue/CALIBRATE_TIME));
            }else{
                calibrateValue+=distance;
            }
            calibrateTimes++;
        }
        LogUtil.i("distance : " + distance);
        return distance;
    }

    public short readAmbient() throws IOException
    {
        short brightness = mDevice.readRegWord(REG_ALS_DATA);
        LogUtil.i("brightness : " + brightness);
        return brightness;
    }

    private short regWord(int low, int high)
    {
        return (short) (((high & 0xff) << 8) | (low & 0xff));
    }

    boolean isCalibrate = false;
    public int calibrateTimes = 0;
    private short calibrateValue=0;
    public void startCalibrate(CalibrateCallback callback)
    {
        this.callback = callback;
        calibrateValue=0;
        calibrateTimes = 0;
        isCalibrate = true;
    }
}
