package com.a3nod.lenovo.sparrowfactory.pcba;

import android.app.Activity;
import android.os.Bundle;
import android.os.Handler;
import android.os.HandlerThread;
import android.os.Message;
import android.util.Log;
import android.view.View;
import android.widget.Button;
import android.widget.TextView;

import com.a3nod.lenovo.sparrowfactory.Constant;
import com.a3nod.lenovo.sparrowfactory.CrashHandler;
import com.a3nod.lenovo.sparrowfactory.R;
import com.a3nod.lenovo.sparrowfactory.interact.ConnectManager;
import com.a3nod.lenovo.sparrowfactory.interact.Protocol;
import com.a3nod.lenovo.sparrowfactory.mmi.MMITestProcessManager;
import com.a3nod.lenovo.sparrowfactory.mmi.result.ASSYEntity;
import com.google.android.things.pio.I2cDevice;
import com.google.android.things.pio.PeripheralManager;

import java.io.BufferedReader;
import java.io.File;
import java.io.FileNotFoundException;
import java.io.FileReader;
import java.io.IOException;
import java.lang.ref.WeakReference;

public class PCBARGBActivity extends Activity {
    private TextView redText,greenText,blueText,brightnessTv,colortempTv,gainTv,timeTv,clearTv,brightnessCoefTv,colortempCoefTv;
    private  Button btn_pass,btn_fail,btn_reset;
    private RgbdataHandler handler = null;
    public static final String TAG = "ShowRGBActivity";

    private static final String RED_BUNDLE_KEY= "red";
    private static final String GREEN_BUNDLE_KEY= "green";
    private static final String BLUE_BUNDLE_KEY= "blue";
    private static final String GAIN_BUNDLE_KEY= "gain";
    private static final String TIME_BUNDLE_KEY= "time";
    private static final String BRIGHTNESS_KEY= "brightness";
    private static final String COLORTEMP_KEY= "colortemp";
    private static final String CLEAR_KEY= "clear";
    private I2cDevice rgbControl = null;

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

    private int brightnessValue = 0;
    private float colortempValue = 0;

    public static final int MODE_CONTROL1 = 0x41; //Function setting
    public static final int MODE_CONTROL2 = 0x42; //Function setting

    public static final int RED_DATA_LSB = 0x50; //Low byte of RED
    public static final int RED_DATA_MSB = 0x51; //High byte of RED
    public static final int GREEN_DATA_LSB = 0x52; //Low byte of GREEN
    public static final int GREEN_DATA_MSB = 0x53; //High byte of GREEN
    public static final int BLUE_DATA_LSB = 0x54; //Low byte of BLUE
    public static final int BLUE_DATA_MSB = 0x55; //High byte of BLUE

    private float currentValue;
    private boolean autoTestFlag = false;
    private Handler threadhandler = null;

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


    private Runnable run = new Runnable() {
        @Override
        public void run() {
            try {
                Thread.sleep(1000);
            }catch (InterruptedException e){
                e.printStackTrace();
            }
            getRgbDataMethod();
        }
    };

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        CrashHandler.getInstance().setCurrentTag(TAG,CrashHandler.TYPE_MMI);
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_show_rgb);

        int testModel = getIntent().getIntExtra(Constant.INTENT_KEY_TEST_MODEL, Constant.INTENT_VALUE_TEST_MODEL_MMI);
        if(testModel== Constant.INTENT_VALUE_TEST_MODEL_PCBA){
            autoTestFlag=true;
        }

        handler = new RgbdataHandler(this);
        String I2C_DEVICE_NAME_2 = "I2C2";
        int I2C_SLAVE_ADDRESS_2 = 0x38;
        PeripheralManager manager = PeripheralManager.getInstance();
        try {
            rgbControl = manager.openI2cDevice(I2C_DEVICE_NAME_2,I2C_SLAVE_ADDRESS_2);
        }catch (IOException e){
            e.printStackTrace();
        }
        initView();
        initConfirmButton();
        getFactoryProperties();
    }

    private void initView(){
        redText = (TextView) findViewById(R.id.red_data);
        greenText = (TextView) findViewById(R.id.green_data);
        blueText = (TextView) findViewById(R.id.blue_data);
        brightnessTv =  (TextView) findViewById(R.id.brightness_data);
        colortempTv =  (TextView) findViewById(R.id.colortemp_data);
        gainTv =  (TextView) findViewById(R.id.gain_data);
        timeTv = (TextView) findViewById(R.id.time_data);
        clearTv = (TextView) findViewById(R.id.clear_data);
        brightnessCoefTv = (TextView) findViewById(R.id.colortemp_coef_data);
        colortempCoefTv = (TextView) findViewById(R.id.brightness_coef_data);
        HandlerThread getDataThread = new HandlerThread("getData");
        getDataThread.start();
        threadhandler = new Handler(getDataThread.getLooper());
        threadhandler.post(run);
    }

    public void initConfirmButton()
    {
        btn_pass = findViewById(R.id.btn_result_pass);
        btn_pass.setEnabled(false);
        btn_pass.setTextColor(getResources().getColor(R.color.colorWhite));
        btn_pass.setOnClickListener(new View.OnClickListener()
        {
            @Override
            public void onClick(View v)
            {
                ConnectManager.getInstance().setRGBSensorResult(Protocol.PCBA_TEST_ITEM_RGBSENSOR,true,"008",brightnessValue,colortempValue);
                destroyThing();
                finish();
            }
        });
        btn_reset = findViewById(R.id.btn_result_reset);
        btn_reset.setVisibility(View.GONE);
        btn_reset.setTextColor(getResources().getColor(R.color.colorWhite));
        btn_reset.setOnClickListener(new View.OnClickListener()
        {
            @Override
            public void onClick(View v)
            {
                reset();
            }
        });
        btn_fail = findViewById(R.id.btn_result_fail);
        if (autoTestFlag){
            btn_fail.setEnabled(false);
        }
        btn_fail.setTextColor(getResources().getColor(R.color.colorWhite));
        btn_fail.setOnClickListener(new View.OnClickListener()
        {
            @Override
            public void onClick(View v)
            {
                ConnectManager.getInstance().setRGBSensorResult(Protocol.PCBA_TEST_ITEM_RGBSENSOR,false,"008",brightnessValue,colortempValue);
                destroyThing();
                finish();
            }
        });
    }

    /**
     * 重新测试
     */
    private void reset()
    {
        redText.setText(""+0);
        greenText.setText("" + 0);
        blueText.setText(""+0);
        brightnessTv.setText(0);
        colortempTv.setText(""+0);
    }

    private void getRgbDataMethod(){
        if (rgbControl == null){
            Log.d(TAG,"rgbControl is null ");
            return;
        }
        try {
            int lightValue = 0;
            float colorValue = 0;
            int red =0;
            int green = 0;
            int blue = 0;
            byte timeBuffer = rgbControl.readRegByte(MODE_CONTROL1);
            byte control2Data = rgbControl.readRegByte(MODE_CONTROL2);
            int gain = again[(control2Data&0x03)];
            long time = atime[(timeBuffer&0x07)];
            if (isVaildData(control2Data)){
                red = readRedValue();
                green = readGreenValue();
                blue = readBlueValue();
                int clear = readClearValue();
                lightValue = getLightValue(timeBuffer,control2Data,red,green,clear);
                colorValue = getColorTemperature(red,green,blue,clear);
                Log.d(TAG," red ="+red+",green ="+green+",blue ="+blue+" ,lightValue ="+lightValue +",colorTempValue ="+colorValue);
                Bundle bundle = new Bundle();
                bundle.putInt(RED_BUNDLE_KEY,red);
                bundle.putInt(GREEN_BUNDLE_KEY,green);
                bundle.putInt(BLUE_BUNDLE_KEY,blue);
                bundle.putInt(BRIGHTNESS_KEY,lightValue);
                bundle.putFloat(COLORTEMP_KEY,colorValue);
                bundle.putInt(GAIN_BUNDLE_KEY,gain);
                bundle.putLong(TIME_BUNDLE_KEY,time);
                bundle.putInt(CLEAR_KEY,clear);
                currentValue= colorValue;
                if (handler != null){
                    Message message = new Message();
                    message.setData(bundle);
                    handler.sendMessage(message);
                }
            }else{
                Log.d(TAG,"RGBSensor data is not vaild");
            }
        }catch (IOException e){
            e.printStackTrace();
        }
    }

    private static class RgbdataHandler extends Handler{
        private WeakReference<PCBARGBActivity> reference;

        public RgbdataHandler(PCBARGBActivity activity){
            reference = new WeakReference<>(activity);
        }
        @Override
        public void handleMessage(Message msg) {
            super.handleMessage(msg);
            PCBARGBActivity activity = reference.get();
            Bundle bundle = msg.getData();
            int what = msg.what;
            if (activity != null && what == 2){
                ConnectManager.getInstance().setRGBSensorResult(Protocol.PCBA_TEST_ITEM_RGBSENSOR,true,"008",activity.brightnessValue,activity.colortempValue);
                activity.destroyThing();
                activity.finish();
            }
            if (activity != null && bundle != null){
                activity.setValueText(bundle.getInt(RED_BUNDLE_KEY),bundle.getInt(GREEN_BUNDLE_KEY),bundle.getInt(BLUE_BUNDLE_KEY),bundle.getInt(BRIGHTNESS_KEY),bundle.getFloat(COLORTEMP_KEY),bundle.getInt(GAIN_BUNDLE_KEY),bundle.getLong(TIME_BUNDLE_KEY),bundle.getInt(CLEAR_KEY));
            }
        }
    }

    private void setValueText(int red,int green,int blue,int brightness,float colortemp,int gain ,long time,int clear){
        brightnessValue = brightness;
        colortempValue =colortemp;
        if (redText != null && greenText != null && blueText != null){
            redText.setText(""+red);
            greenText.setText("" + green);
            blueText.setText(""+blue);
            gainTv.setText(""+gain);
            timeTv.setText(""+time);
            brightnessTv.setText(""+brightness);
            colortempTv.setText(""+colortemp);
            clearTv.setText(""+clear);
            colortempCoefTv.setText(""+TEMP_COEFFICIENT_VALUE);
            brightnessCoefTv.setText(""+LIGHT_COEFFICIENT_VALUE);
        }
        if (!btn_pass.isEnabled()&& brightness>0&& colortemp>0){
            if (autoTestFlag){
                handler.sendEmptyMessageDelayed(2,3000);
            }else{
                btn_pass.setEnabled(true);
            }

        }
    }

    public int readClearValue()  throws IOException {
        byte clearLow =   rgbControl.readRegByte(0x56);
        byte clearHigh =   rgbControl.readRegByte(0x57);
        int clearValue = ((clearHigh & 0x0ff )<< 8) | (clearLow & 0x0ff);
        return clearValue;
    }

    private void destroyThing(){
        if (rgbControl != null){
            try {
                rgbControl.close();
                rgbControl = null;
            }catch (IOException e){
                e.printStackTrace();
            }
        }
        if (threadhandler != null){
            threadhandler.removeCallbacks(run);
        }
    }

    @Override
    protected void onDestroy() {
        super.onDestroy();
        destroyThing();
    }

    /**
     * calculate brightness value
     * @param red
     * @param green
     * @param clear
     * @return
     * @throws IOException
     */
    public int getLightValue(byte timeBuffer,byte control2Data ,int red,int green,int clear)throws IOException {
        long lux = 0;
        int gain = again[(control2Data&0x03)];
        long time = atime[(timeBuffer&0x07)];
        if (green<1){
            lux = 0;
        }else{
            long luxTemp = 0;
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
//            bRatio = (blueAdj) / (rgbAdj);

            bRatio = blueAdj/(0.1*redAdj + greenAdj + blueAdj);
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

    public int readBlueValue() throws IOException {
        byte blueLow = rgbControl.readRegByte(BLUE_DATA_LSB);
        byte blueHigh = rgbControl.readRegByte(BLUE_DATA_MSB);
        int blueValue = ((blueHigh & 0x0ff)<< 8 )| (blueLow & 0x0ff);
        return  blueValue;
    }

    public int readRedValue() throws IOException {
        byte redLow = rgbControl.readRegByte(RED_DATA_LSB);
        byte redHigh = rgbControl.readRegByte(RED_DATA_MSB);
        int redValue = ((redHigh & 0x0ff)<<8) | (redLow& 0x0ff);
        return redValue;
    }

    public int readGreenValue() throws IOException {
        byte greenLow = rgbControl.readRegByte(GREEN_DATA_LSB);
        byte greenHigh = rgbControl.readRegByte(GREEN_DATA_MSB);
        int greenValue = ((greenHigh & 0x0ff )<< 8) | (greenLow & 0x0ff);
        return greenValue;
    }


    public boolean isVaildData(byte isVaildData ){
        /**
         * 0 : RGBC data is not updated after last writing MODE_CONTROL1,2
         register or last reading MODE_CONTROL2 register.
         1 : RGBC data is updated after last writing MODE_CONTROL1,2 register
         or last reading MODE_CONTROL2 register
         */
        return ((isVaildData & (1<<7)) != 0);
    }
}
