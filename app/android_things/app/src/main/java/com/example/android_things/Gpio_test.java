package com.example.android_things;

import android.app.Activity;
import android.graphics.Color;
import android.os.Bundle;
import android.os.Handler;
import android.view.MotionEvent;
import android.view.View;
import android.widget.Button;
import android.widget.Toast;

import com.google.android.things.pio.Gpio;
import com.google.android.things.pio.GpioCallback;
import com.google.android.things.pio.PeripheralManager;

import java.io.IOException;

public class Gpio_test extends Activity {
    //输入和输出GPIO引脚名称
    private static final String GPIO_IN_85 = "GPIO_85";
    private static final String GPIO_IN_86= "GPIO_86";
    private static final String GPIO_IN_87 = "GPIO_1019";
    //输入和输出Gpio
    private Gpio mGpio85;
    private Gpio mGpio86;
    private Gpio mGpio87;


    private Button gpio_85;
    private Button gpio_86;
    private Button gpio_87;
    private Handler mHandler = new Handler();

    private GpioCallback mGpioCallback = new GpioCallback() {
        @Override
        public boolean onGpioEdge(Gpio gpio) {
            try {
                gpio.setValue(!gpio.getValue());
            } catch (IOException e) {
                e.printStackTrace();
            }
            return true;
        }
    };
    private Runnable mBlinkRunnable = new Runnable() {
        @Override
        public void run() {
            try {
                if(mGpio85.getValue()){
                    gpio_85.setBackgroundColor(Color.RED);
                }else{
                    gpio_85.setBackgroundColor(Color.BLACK);
                }

                if(mGpio86.getValue()){
                    gpio_86.setBackgroundColor(Color.RED);
                }else{
                    gpio_86.setBackgroundColor(Color.BLACK);
                }

                if(mGpio87.getValue()){
                    gpio_87.setBackgroundColor(Color.RED);
                }else{
                    gpio_87.setBackgroundColor(Color.BLACK);
                }

                mHandler.postDelayed(mBlinkRunnable,100);

            } catch (IOException e) {
                e.printStackTrace();
            }

        }
    };


    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_gpio_test);

        gpio_85 = findViewById(R.id.GPIO_85);
        gpio_86 = findViewById(R.id.GPIO_86);
        gpio_87 = findViewById(R.id.GPIO_87);

        PeripheralManager manager = PeripheralManager.getInstance();
        //打开并设置输入Gpio，监听输入信号变化（开关按钮的开关）
        try {
            mGpio85 = manager.openGpio(GPIO_IN_85);
            mGpio85.setDirection(Gpio.DIRECTION_IN);
            mGpio85.setEdgeTriggerType(Gpio.EDGE_BOTH);
            mGpio85.registerGpioCallback(mGpioCallback);

            mGpio86 = manager.openGpio(GPIO_IN_86);
            mGpio86.setDirection(Gpio.DIRECTION_IN);
            mGpio86.setEdgeTriggerType(Gpio.EDGE_BOTH);
            mGpio86.registerGpioCallback(mGpioCallback);

            mGpio87 = manager.openGpio(GPIO_IN_87);
            mGpio87.setDirection(Gpio.DIRECTION_IN);
            mGpio87.setEdgeTriggerType(Gpio.EDGE_BOTH);
            mGpio87.registerGpioCallback(mGpioCallback);



        }catch (IOException e){
            e.printStackTrace();
        }

        mHandler.post(mBlinkRunnable);

    }

    protected void onDestroy(){
        super.onDestroy();
        //关闭Gpio
        if (mGpio85 != null) {
            try {
                mGpio85.unregisterGpioCallback(mGpioCallback);
                mGpio85.close();
                mGpio85 = null;
            } catch (IOException e) {
                e.printStackTrace();
            }
        }

        if (mGpio86 != null) {
            try {
                mGpio86.unregisterGpioCallback(mGpioCallback);
                mGpio86.close();
                mGpio86 = null;
            } catch (IOException e) {
                e.printStackTrace();
            }
        }

        if (mGpio86 != null) {
            try {
                mGpio86.unregisterGpioCallback(mGpioCallback);
                mGpio86.close();
                mGpio86 = null;
            } catch (IOException e) {
                e.printStackTrace();
            }
        }

    }


    private long exitTime = 0;
    public boolean dispatchTouchEvent(MotionEvent ev) {
        if (MotionEvent.ACTION_DOWN == ev.getAction()) {
            if ((System.currentTimeMillis() - exitTime) > 2000) {
                Toast.makeText(getApplicationContext(), "Press exit again", Toast.LENGTH_SHORT).show();
                exitTime = System.currentTimeMillis();
            } else {
                finish();
                System.exit(0);
            }
        }
        return super.dispatchTouchEvent(ev);
    }
}




















