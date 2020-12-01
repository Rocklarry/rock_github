package com.example.myapplication;

import androidx.appcompat.app.AppCompatActivity;

import android.app.Activity;
import android.os.Bundle;
import android.provider.Settings;
import android.provider.Settings.System;
import android.util.Log;
import android.widget.SeekBar;
import android.widget.TextView;

import java.io.BufferedReader;
import java.io.BufferedWriter;
import java.io.DataOutputStream;
import java.io.FileReader;
import java.io.FileWriter;
import java.io.IOException;
import java.io.InputStream;
import java.io.InputStreamReader;

public class Back_Light extends Activity {

    private  String path = "/sys/class/leds/lcd-backlight/brightness";
    private SeekBar seekBar;
    private  TextView back_light;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_back__light);

        back_light = (TextView) findViewById(R.id.back_light);


        back_light.setText("当前背光值:" + readFile(path));

        seekBar = (SeekBar) findViewById(R.id.seekBar);
        seekBar.setOnSeekBarChangeListener(new SeekBar.OnSeekBarChangeListener() {
            @Override
            public void onProgressChanged(SeekBar seekBar, int i, boolean b) {
                int max = seekBar.getMax();
                String s = i + "/" + max;

                writeSysFile(path,String.valueOf(i));

                back_light.setText("当前背光值\n" + s);
            }

            @Override
            public void onStartTrackingTouch(SeekBar seekBar) {
            }

            @Override
            public void onStopTrackingTouch(SeekBar seekBar) {

            }
        });
    }


    //sys_path 为节点映射到的实际路径
    public static String readFile(String sys_path) {
        String prop = "waiting";// 默认值
        BufferedReader reader = null;
        try {
            reader = new BufferedReader(new FileReader(sys_path));
            prop = reader.readLine();
        } catch (IOException e) {
            e.printStackTrace();
            Log.i("rrd", " ***ERROR*** Here is what I know: " + e.getMessage());
        } finally {
            if(reader != null){
                try {
                    reader.close();
                } catch (IOException e) {
                    e.printStackTrace();
                }
            }
        }
        Log.i("rrd", "readFile cmd from"+sys_path + "data"+" -> prop = "+prop);
        return prop;
    }



    private void writeSysFile(String path, String value) {
        try {
            BufferedWriter bufWriter = null;
            bufWriter = new BufferedWriter(new FileWriter(path));
            bufWriter.write(value + "");
            bufWriter.close();
            Log.i("rrd", "write setLedOnOff: " + value);
        } catch (IOException e) {
            Log.i("rrd","erro= "+ Log.getStackTraceString(e));
        }
    }

//    public void setScreenBrightness(int brightness){
//        //如果是自动调整亮度则先换成手动调整再设置亮度
//        try {
//            if (Settings.System.SCREEN_BRIGHTNESS_MODE_AUTOMATIC==Settings.System.getInt(getContentResolver(), System.SCREEN_BRIGHTNESS_MODE)) {
//                Settings.System.putInt(getContentResolver(), System.SCREEN_BRIGHTNESS_MODE,System.SCREEN_BRIGHTNESS_MODE_MANUAL);
//            }
//        } catch (Settings.SettingNotFoundException e) {
//            e.printStackTrace();
//        }
//        Settings.System.putInt(getContentResolver(), System.SCREEN_BRIGHTNESS, brightness);
//    }
//
//
//    public int getScreenBrightness(){
//        int brightness=-1;
//        try {
//            brightness=Settings.System.getInt(getContentResolver(), System.SCREEN_BRIGHTNESS);
//        } catch (Settings.SettingNotFoundException e) {
//            e.printStackTrace();
//        }
//        return brightness;
//    }



}
