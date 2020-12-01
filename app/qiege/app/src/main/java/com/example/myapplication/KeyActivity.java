package com.example.myapplication;

import android.app.Activity;
import android.content.Intent;
import android.graphics.Color;
import android.os.Bundle;

import android.util.Log;
import android.view.KeyEvent;
import android.view.View;
import android.view.View.OnClickListener;
import android.widget.Button;

import android.widget.TextView;
import android.widget.Toast;

import com.example.myapplication.utils.ShellUtils;


public class KeyActivity extends Activity  implements View.OnClickListener {

    private TextView text_code;
    private Button bt_text, bt_sleep, bt_home, bt_call, bt_nav, bt_sat, bt_mon;

    private static String led_home_string = "/sys/class/leds/home_led/brightness";
    private static String led_navigtion_string = "/sys/class/leds/navigate_led/brightness";
    private static String led_monitor_string = "/sys/class/leds/monitor_led/brightness";
    private static String led_recall_string = "/sys/class/leds/recall_led/brightness";
    private static String led_sat_string = "/sys/class/leds/sat_led/brightness";

    //public static final String GPIO_SCREEN_ON = "setprop persist.backlight.check 0";

    private static boolean led_home_type = false;
    private static boolean led_navigation_type = false;
    private static boolean led_monitor_type = false;
    private static boolean led_recall_type = false;
    private static boolean led_sat_type = false;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_key);

        text_code = (TextView) findViewById(R.id.key_code);
        bt_text = (Button) findViewById(R.id.bt_text);
        bt_sleep = (Button) findViewById(R.id.sleep_test);

        bt_home = (Button) findViewById(R.id.bt_home);
        bt_call = (Button) findViewById(R.id.bt_call);
        bt_nav = (Button) findViewById(R.id.bt_nav);
        bt_mon = (Button) findViewById(R.id.bt_mon);
        bt_sat = (Button) findViewById(R.id.bt_sat);

        bt_home.setOnClickListener(this);
        bt_call.setOnClickListener(this);
        bt_nav.setOnClickListener(this);
        bt_mon.setOnClickListener(this);
        bt_sat.setOnClickListener(this);


        bt_sleep.setOnClickListener(view -> {


        });

        bt_text.setOnClickListener(view -> finish());
    }


    public boolean onKeyDown(int keyCode, KeyEvent event) {

        Log.i("key down", "keycode:" + keyCode);

        text_code.setText(keyCode + "   Down");

        //Toast.makeText(this, "keycode:"+keyCode, Toast.LENGTH_SHORT).show();


        // 当按下返回键时所执行的命令
        if (keyCode == KeyEvent.KEYCODE_BACK) {
            // 此处写你按返回键之后要执行的事件的逻辑
            return super.onKeyDown(keyCode, event);
        }
        return super.onKeyDown(keyCode, event);

    }

    public boolean onKeyUp(int keyCode, KeyEvent event) {

        Log.i("key onKeyUp", "keycode:" + keyCode);
        text_code.setText(keyCode + "   Up");

        //Toast.makeText(this, "keycode:"+keyCode, Toast.LENGTH_SHORT).show();
        // 当按下返回键时所执行的命令
        if (keyCode == KeyEvent.KEYCODE_BACK) {
            // 此处写你按返回键之后要执行的事件的逻辑
            return super.onKeyDown(keyCode, event);
        }
        return super.onKeyDown(keyCode, event);

    }

    @Override
    public void onClick(View view) {
        switch (view.getId()) {
            case R.id.bt_home:
                Log.i("led","value="+ShellUtils.execCommand("cat /sys/class/leds/home_led/brightness ",false).successMsg);
                if (!led_home_type) {
                    ShellUtils.execCommand("echo 1 > "+led_home_string, false);
                    bt_home.setBackgroundColor(Color.parseColor("#CD2626"));

                    //ShellUtils.execCommand(GPIO_SCREEN_ON,true);
                } else {
                    ShellUtils.execCommand("echo 0 > "+led_home_string, false);
                    bt_home.setBackgroundColor(Color.parseColor("#4CAF50"));
                }
                led_home_type=!led_home_type;
                break;
            case R.id.bt_call:
                if(!led_recall_type){
                    ShellUtils.execCommand("echo 1 > "+led_recall_string, false);
                    bt_call.setBackgroundColor(Color.parseColor("#CD2626"));
                } else {
                    ShellUtils.execCommand("echo 0 > "+led_recall_string, false);
                    bt_call.setBackgroundColor(Color.parseColor("#4CAF50"));
                }
                led_recall_type=!led_recall_type;
                break;
            case R.id.bt_nav:
                if(!led_navigation_type){
                    ShellUtils.execCommand("echo 1 > "+led_navigtion_string, false);
                    bt_nav.setBackgroundColor(Color.parseColor("#CD2626"));
                } else {
                    ShellUtils.execCommand("echo 0 > "+led_navigtion_string, false);
                    bt_nav.setBackgroundColor(Color.parseColor("#4CAF50"));
                }
                led_navigation_type=!led_navigation_type;
                break;

            case R.id.bt_mon:
                if(!led_monitor_type){
                    ShellUtils.execCommand("echo 1 > "+led_monitor_string, false);
                    bt_mon.setBackgroundColor(Color.parseColor("#CD2626"));
                } else {
                    ShellUtils.execCommand("echo 0 > "+led_monitor_string, false);
                    bt_mon.setBackgroundColor(Color.parseColor("#4CAF50"));
                }
                led_monitor_type=!led_monitor_type;
                break;
            case R.id.bt_sat:
                if(!led_sat_type){
                    ShellUtils.execCommand("echo 1 > "+led_sat_string, false);
                    bt_sat.setBackgroundColor(Color.parseColor("#CD2626"));
                } else {
                    ShellUtils.execCommand("echo 0 > "+led_sat_string, false);
                    bt_sat.setBackgroundColor(Color.parseColor("#4CAF50"));
                }
                led_sat_type=!led_sat_type;
                break;


            default:
                break;

        }
    }
}