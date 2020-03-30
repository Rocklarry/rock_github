package com.example.android_things;

import android.app.Activity;
import android.bluetooth.BluetoothAdapter;
import android.bluetooth.BluetoothDevice;
import android.bluetooth.BluetoothProfile;
import android.content.Context;
import android.content.Intent;
import android.os.Build;
import android.os.Bundle;
import android.view.MotionEvent;
import android.view.View;
import android.widget.Button;
import android.widget.TextView;
import java.lang.reflect.Field;
import java.lang.reflect.Method;
import java.util.List;
import java.util.Set;

/**
 * Skeleton of an Android Things activity.
 * <p>
 * Android Things peripheral APIs are accessible through the class
 * PeripheralManagerService. For example, the snippet below will open a GPIO pin and
 * set it to HIGH:
 *
 * <pre>{@code
 * PeripheralManagerService service = new PeripheralManagerService();
 * mLedGpio = service.openGpio("BCM6");
 * mLedGpio.setDirection(Gpio.DIRECTION_OUT_INITIALLY_LOW);
 * mLedGpio.setValue(true);
 * }</pre>
 * <p>
 * For more complex peripherals, look for an existing user-space driver, or implement one if none
 * is available.
 *
 * @see <a href="https://github.com/androidthings/contrib-drivers#readme">https://github.com/androidthings/contrib-drivers#readme</a>
 */
public class MainActivity extends Activity {

   // private TextView tx;
    private Button bt;
    private Button ps;
    private Button ls;
    private Button io_list;
    private Button gpio_test;


    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_main);
        bt =  findViewById(R.id.bt);
        ps = findViewById(R.id.ps);
        ls = findViewById(R.id.ls);
        io_list = findViewById(R.id.io_list);
        gpio_test = findViewById(R.id.gpio_test);


        bt.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {

                Intent intent = new Intent(MainActivity.this,BT_test.class);
                startActivity(intent);
            }
        });

        ps.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                Intent intent = new Intent(MainActivity.this,P_sensor.class);
                startActivity(intent);
            }
        });

        ls.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                Intent intent = new Intent(MainActivity.this,L_sensor.class);
                startActivity(intent);
            }
        });
        io_list.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                Intent intent = new Intent(MainActivity.this,Io_list.class);
                startActivity(intent);
            }
        });

        gpio_test.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                Intent intent = new Intent(MainActivity.this,Gpio_test.class);
                startActivity(intent);
            }
        });
    }



    private int count = 0;
    private long firClick = 0;
    private long secClick = 0;
    private final int interval = 1500;

    public boolean dispatchTouchEvent(MotionEvent ev) {
        if (MotionEvent.ACTION_DOWN == ev.getAction()) {
            count++;
            if (1 == count) {
                firClick = System.currentTimeMillis();
            } else if (2 == count) {
                secClick = System.currentTimeMillis();
                if (secClick - firClick < interval) {
                    count = 0;
                    firClick = 0;
                    System.exit(0);
                } else {
                    firClick = secClick;
                    count = 1;
                }
                secClick = 0;
            }
        }
        return super.dispatchTouchEvent(ev);
    }

}
