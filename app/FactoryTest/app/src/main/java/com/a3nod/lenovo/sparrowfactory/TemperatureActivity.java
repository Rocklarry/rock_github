package com.a3nod.lenovo.sparrowfactory;

import android.app.Activity;
import android.app.AlertDialog;
import android.content.DialogInterface;
import android.os.Bundle;
import android.os.Handler;
import android.os.Message;
import android.support.annotation.Nullable;
import android.util.Log;
import android.view.KeyEvent;
import android.view.MotionEvent;
import android.widget.TextView;
import android.widget.Toast;

import com.a3nod.lenovo.sparrowfactory.tool.SystemInfoTools;

import java.io.File;

/**
 * Created by admin on 2018/1/29.
 */

public class TemperatureActivity extends Activity {
    private TextView temperature_show;
    private long delayMillis = 1000;
    private int currentTemperature = 0;
    private String TAG = "TemperatureActivity";
    private long lastMillis = 0;
    private AlertDialog alertDialog = null;
    private  String currentFile = null ;
    @Override
    protected void onCreate(@Nullable Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_temperature);
        initView();
    }

    @Override
    protected void onResume() {
        super.onResume();
        getFile();
        handler.post(runnable);
    }

    @Override
    protected void onDestroy() {
        super.onDestroy();
        if (alertDialog != null) alertDialog.dismiss();
        handler.removeCallbacks(runnable);
        handler.removeMessages(0);
    }

    private void initView() {
        temperature_show = findViewById(R.id.temperature_show);
    }

    Runnable runnable = new Runnable() {
        @Override
        public void run() {
            getCurrentTemperature();
            handler.postDelayed(runnable, delayMillis);
        }
    };

    private void showDialog() {
        if (alertDialog == null)
            alertDialog = new AlertDialog.Builder(this)
                    .setPositiveButton("OK", new DialogInterface.OnClickListener() {
                        @Override
                        public void onClick(DialogInterface dialog, int which) {

                        }
                    }).create();

        alertDialog.setTitle("Warning !!!" + currentTemperature);
        alertDialog.setMessage("The temperature of the mobile phone is too high !!!");

        if (!alertDialog.isShowing()) {
            alertDialog.show();
        }
    }

    private void hideDialog() {
        if (alertDialog != null && alertDialog.isShowing()) {
            alertDialog.dismiss();
        }
    }

    private void getCurrentTemperature() {
        if (currentFile == null) return;
        currentTemperature = Integer.parseInt(SystemInfoTools.readSNFile(currentFile))/1000;
        handler.sendEmptyMessage(0);
        Log.d(TAG, "currentTemperature = " + currentTemperature);

    }

    private  void  getFile(){
        try {
            File path =  new File("/sys/class/thermal/");
            File[] files = path.listFiles();
            if(files ==null) return ;
            for (File file : files){
                Log.d(TAG, "F-name="+file.getName()+ " --- " + file.getPath()+"\n");
              //  String type = SystemInfoTools.readSNFile(file.getPath() +"/type");
                String type=file.getName().toString();
                Log.d(TAG, "type====="+type);
               // if(type.contains("case_therm")){
                if(type.equals("thermal_zone9")){
                    currentFile = file.getPath()+"/temp";
                    break;
                }
            }
        } catch (Exception e) {
            currentFile = null ;
            e.printStackTrace();
        }
        Log.d(TAG, "currentFile = " + currentFile);
    }

    Handler handler = new Handler() {
        @Override
        public void handleMessage(Message msg) {
            Log.d(TAG, "handleMessage");
            switch (msg.what) {
                case 0:
                    if (temperature_show != null)
                        temperature_show.setText("CurrentTemperature:  " + currentTemperature);
                    if (currentTemperature >= 60) showDialog();
                    else hideDialog();
                    break;
            }
        }
    };

    @Override
    public boolean onTouchEvent(MotionEvent event) {
        if (event.getAction() == KeyEvent.ACTION_DOWN) {
            long currentMillis = System.currentTimeMillis();
            if (currentMillis - lastMillis > 2000) {
                lastMillis = currentMillis;
                Toast.makeText(this, getString(R.string.double_click_exit), Toast.LENGTH_SHORT).show();
            } else {
                finish();
            }
        }
        return false;
    }
}
