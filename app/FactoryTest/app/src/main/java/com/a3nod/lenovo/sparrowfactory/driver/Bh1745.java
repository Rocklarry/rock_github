package com.a3nod.lenovo.sparrowfactory.driver;

import android.util.Log;

import com.google.android.things.pio.I2cDevice;
import com.google.android.things.pio.PeripheralManager;

import java.io.BufferedReader;
import java.io.File;
import java.io.FileNotFoundException;
import java.io.FileReader;
import java.io.IOException;

/**
 * A class for describing bh1745 RGB sensor
 * In this class, read red, green and blue numerical methods are provided,
 *  and the method of activating RGB sensors is also provided.
 *  IsVaildData () is used to check whether the sensor values are updated or not.
 */
public class Bh1745 implements AutoCloseable{

    private static final String TAG = "Bh1745";
    private static final int I2C_ADDRESS = 0x38;
    private I2cDevice mDevice;

    public static final int RED_DATA_LSB = 0x50; //Low byte of RED
    public static final int RED_DATA_MSB = 0x51; //High byte of RED
    public static final int GREEN_DATA_LSB = 0x52; //Low byte of GREEN
    public static final int GREEN_DATA_MSB = 0x53; //High byte of GREEN
    public static final int BLUE_DATA_LSB = 0x54; //Low byte of BLUE
    public static final int BLUE_DATA_MSB = 0x55; //High byte of BLUE



    public static final int MODE_CONTROL1 = 0x41; //Function setting
    public static final int MODE_CONTROL2 = 0x42; //Function setting

    public static final int MANUFACTURER_ID = 0x92; //Manufacturer ID
    public static final int MANUFACTURER_ID_DEFAULT = 0xE0; //Manufacturer ID default calue

    private static final int JUDGE_LENS_BLACK = 178;
    private static final int[] RED_LENS_BLACK = {161,103};
    private static final int[] GREEN_LENS_BLACK ={480,405};
    private static final int JUDGE_FIXED_COEF = 1000;
    private static final int CUT_UNIT = 1000;
    private static final float TRANS =16.0f;
    private static final int[] again = {1,2,16};
    private static final int[] atime = {160, 320, 640, 1280, 2560, 5120};

    private static final double RATIO_JUDGEC= 0.178;
    private static final double[] beff={2.879, 5.787, 1.0};
    private static final double[] color={-4.508, 5.091, -3.146,  5.242};
    private static final int[]    cct_eff= {20741, 1366, 16442, 1271};
    private static final int CLOLOR_TEMPRATURE_MAX = 10000;

    private static  final String[] FACTORY_PROPERTY_PATH ={"/factory_iot/device.prop","/mnt/product/factory/device.prop"};
    private static final String FACTORY_PROPERTY_LIGHTCOEF_KEY = "light_coef";
    private static float LIGHT_COEFFICIENT_VALUE = 1.0f;
    private static final String FACTORY_PROPERTY_TEMPCOEF_KEY = "temp_coef";
    private static float TEMP_COEFFICIENT_VALUE = 1.0f;

    private static byte timeBuffer;
    private static byte control2Data;

    /**
     * Create a new Bh1745 RGBsensor driver connected on the given bus.
     *
     * @param bus I2C bus the sensor is connected to.
     */
    public Bh1745(String bus) throws IOException {
        PeripheralManager managerService = PeripheralManager.getInstance();
        I2cDevice device = managerService.openI2cDevice(bus, I2C_ADDRESS);
        getFactoryProperties();
        try {
            connect(device);
        } catch (IOException | RuntimeException e) {
            try {
                close();
            } catch (IOException | RuntimeException ignored) {
            }
            throw e;
        }
    }

    private void getFactoryProperties()
    {
        for (String path:FACTORY_PROPERTY_PATH){
            File file = new File(path);
            if (file.exists()){
                Log.d("Bh1745","----file exit-----");
                try {
                    FileReader reader = new FileReader(file);
                    BufferedReader buffReader= new BufferedReader(reader);
                    String temp = null;
                    while((temp = buffReader.readLine())!=null){
                        Log.d("Bh1745","---------temp ="+temp);
                        String[] values = temp.split("=");
                        if (values[0].trim().equals(FACTORY_PROPERTY_LIGHTCOEF_KEY)){
                            LIGHT_COEFFICIENT_VALUE = Float.parseFloat(values[1]);
                            Log.d("Bh1745","---------RATIO_JUDGEC ="+LIGHT_COEFFICIENT_VALUE);
                        }
                        if (values[0].trim().equals(FACTORY_PROPERTY_TEMPCOEF_KEY)){
                            TEMP_COEFFICIENT_VALUE = Float.parseFloat(values[1]);
                            Log.d("Bh1745","---------RATIO_JUDGEC ="+TEMP_COEFFICIENT_VALUE);
                        }
                    }
                    buffReader.close();
                    reader.close();
                }catch (FileNotFoundException e){
                    e.printStackTrace();
                }catch (IOException e){
                    e.printStackTrace();
                }
            }
        }
    }

    /**
     * Create a new Bh1745 RGBsensor driver connected to the given I2c device.
     *
     * @param device I2C device of the sensor.
     */
    /*package*/  Bh1745(I2cDevice device) throws IOException {
        connect(device);
    }

    public void close() throws IOException {
        mDevice.close();
    }

    public void connect(I2cDevice device) throws IOException {
        mDevice = device;

        int mChipId = mDevice.readRegWord(MANUFACTURER_ID) & 0x0ff;
        // Sanity check
        if (mChipId != MANUFACTURER_ID_DEFAULT) {
            Log.e(TAG, String.format("Wrong Manufacturer ID: %X", mChipId));
            throw new IOException("Bh1745 RGBsensor not found");
        }

    }

    public void enableRGBSensor(boolean enabled) throws IOException {
        short registersRgb[][] = {
                {0x41,0x00},
                {0x42,0x12}
        };
        byte[] buf = new byte[2];
        if (enabled) {
            for (int index = 0; index < registersRgb.length; index++) {
                buf[0] = (byte) registersRgb[index][0];
                buf[1] = (byte) registersRgb[index][1];
                mDevice.write(buf, 2);
            }
        } else {
            for (int index = 0; index < registersRgb.length; index++) {
                buf[0] = (byte) registersRgb[index][0];
                buf[1] = (byte) registersRgb[index][1];
                mDevice.write(buf, 2);
            }
        }
        timeBuffer = mDevice.readRegByte(MODE_CONTROL1);
    }

    public int readBlueValue() throws IOException {
        byte blueLow = mDevice.readRegByte(BLUE_DATA_LSB);
        byte blueHigh = mDevice.readRegByte(BLUE_DATA_MSB);
        int blueValue = ((blueHigh & 0x0ff)<< 8 )| (blueLow & 0x0ff);
        return  blueValue;
    }

    public int readRedValue() throws IOException {
        byte redLow = mDevice.readRegByte(RED_DATA_LSB);
        byte redHigh = mDevice.readRegByte(RED_DATA_MSB);
        int redValue = ((redHigh & 0x0ff)<<8) | (redLow& 0x0ff);
        return redValue;
    }

    public int readGreenValue() throws IOException {
        byte greenLow = mDevice.readRegByte(GREEN_DATA_LSB);
        byte greenHigh = mDevice.readRegByte(GREEN_DATA_MSB);
        int greenValue = ((greenHigh & 0x0ff )<< 8) | (greenLow & 0x0ff);
        return greenValue;
    }

    public int readClearValue()  throws IOException {
        byte clearLow =   mDevice.readRegByte(0x56);
        byte clearHigh =   mDevice.readRegByte(0x57);
        int clearValue = ((clearHigh & 0x0ff )<< 8) | (clearLow & 0x0ff);
        return clearValue;
    }

    /**
     * calculate brightness value
     * @param red
     * @param green
     * @param clear
     * @return
     * @throws IOException
     */
    public int getLightValue(int red,int green,int clear)throws IOException {
        int lux = 0;
        int gain = again[(control2Data&0x03)];
        long time = atime[(timeBuffer&0x07)];
        if (green<1){
            lux = 0;
        }else{
            int luxTemp = 0;
            if ((clear*JUDGE_FIXED_COEF)<(green*JUDGE_LENS_BLACK)){
                luxTemp = RED_LENS_BLACK[0]*red + GREEN_LENS_BLACK[0]*green;
            }else{
                luxTemp = RED_LENS_BLACK[1]*red + GREEN_LENS_BLACK[1]*green;
            }
            lux =(int)(((luxTemp*160*TRANS/gain)/time)/CUT_UNIT);
        }
        return (int)(lux*LIGHT_COEFFICIENT_VALUE);
    }

    /**
     * calculate color temperature
     * @param red
     * @param green
     * @param blue
     * @param clear
     * @return
     * @throws IOException
     */
    public float getColorTemperature(int red,int green,int blue,int clear) throws IOException{
        float colorTemperature = 0;
        double redAdj, greenAdj, blueAdj, clearAdj, rgbAdj;
        double rRatio, bRatio;
        double bEff;
        double temp;
        redAdj = red * 1.0 + 0.0;
        greenAdj = green * 1.0 + 0.0;
        blueAdj = blue * 1.0 + 0.0;
        clearAdj = clear * 1.0 + 0.0;
        rgbAdj = redAdj + greenAdj + blueAdj;
        if ((greenAdj < 1) || (rgbAdj < 1)){
            temp = 0;
        }else{
            rRatio = (redAdj) / (rgbAdj);
            bRatio = (blueAdj) / (rgbAdj);

            if ((clearAdj/greenAdj) < RATIO_JUDGEC)
            {
                if ((bRatio*beff[0]) > beff[2]){
                    bEff = beff[2];
                } else{
                    bEff = bRatio*beff[0];
                }
                temp = (1 - bEff) * cct_eff[0] * Math.exp(color[0] * rRatio) + bEff * cct_eff[1] * Math.exp(color[1] * bRatio);
            }
            else
            {
                if ((bRatio*beff[1]) > beff[2]){
                    bEff = beff[2];
                }else{
                    bEff = bRatio*beff[1];
                }
                temp = (1 - bEff) * cct_eff[2] * Math.exp(color[2] * rRatio) + bEff * cct_eff[3] * Math.exp(color[3] * bRatio);
            }
            if (temp > CLOLOR_TEMPRATURE_MAX)
            {
                temp = CLOLOR_TEMPRATURE_MAX;
            }
        }
        colorTemperature = (float)temp*TEMP_COEFFICIENT_VALUE;
        return colorTemperature;
    }

    public boolean isVaildData()throws IOException{
        control2Data = mDevice.readRegByte(MODE_CONTROL2);
        /**
         * 0 : RGBC data is not updated after last writing MODE_CONTROL1,2
         register or last reading MODE_CONTROL2 register.
         1 : RGBC data is updated after last writing MODE_CONTROL1,2 register
         or last reading MODE_CONTROL2 register
         */
        return ((control2Data & (1<<7)) != 0);
    }

    private short regWord(int low, int high) {
        return (short) (((high & 0xff) << 8) | (low & 0xff));
    }
}

