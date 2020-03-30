package com.a3nod.lenovo.sparrowfactory;

import android.app.Activity;
import android.content.Intent;
import android.os.Bundle;
import android.os.Handler;
import android.os.Message;
import android.view.KeyEvent;
import android.view.MotionEvent;
import android.view.View;
import android.widget.AdapterView;
import android.widget.ArrayAdapter;
import android.widget.Spinner;
import android.widget.TextView;
import android.widget.Toast;

import com.a3nod.lenovo.sparrowfactory.tool.SPUtils;
import com.google.android.things.device.DeviceManager;

import java.util.ArrayList;
import java.util.List;

public class RebootActivity extends Activity
{
    public static final int MSG_REBOOT=0;
    TextView tv_reboot_status;
    Spinner sp_reboot_test_times;
    int currentRebootTimes = -1;
    int settingRebootTimes = -1;
    List<String> ls_times;


    @Override
    protected void onCreate(Bundle savedInstanceState)
    {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_reboot);

        initData();
        initView();
        getCurrentStatus();

    }

    public void initView(){
        tv_reboot_status = findViewById(R.id.tv_reboot_test_status);
        sp_reboot_test_times = findViewById(R.id.sp_reboot_times);
        sp_reboot_test_times.setAdapter(new ArrayAdapter<String>(this, android.R.layout.simple_spinner_item, ls_times));
        sp_reboot_test_times.setOnItemSelectedListener(new AdapterView.OnItemSelectedListener()
        {
            @Override
            public void onItemSelected(AdapterView<?> parent, View view, int position, long id)
            {
                settingRebootTimes = Integer.parseInt(ls_times.get(position));
            }

            @Override
            public void onNothingSelected(AdapterView<?> parent)
            {
                settingRebootTimes = Integer.parseInt(ls_times.get(0));
            }
        });
        sp_reboot_test_times.setSelection(getPosition(settingRebootTimes));

    }
    public void initData()
    {
        currentRebootTimes = (int) SPUtils.get(this, Constant.SP_KEY_REBOOT_CURRENT_TIME, -1);
        settingRebootTimes = (int) SPUtils.get(this, Constant.SP_KEY_REBOOT_TIMES, -1);

        ls_times = new ArrayList<>();
        ls_times.add("10");
        ls_times.add("50");
        ls_times.add("100");
        ls_times.add("200");
        ls_times.add("1000");
    }

    public int getPosition(int setting)
    {
        int position = 0;
        switch (setting)
        {
            case 10:
                position=0;
                break;
            case 50:
                position=1;
                break;
            case 100:
                position=2;
                break;
            case 200:
                position=3;
                break;
            case 1000:
                position=4;
                break;
        }
        return position;
    }

    public void reboot(View view)
    {
        currentRebootTimes=0;
        SPUtils.put(RebootActivity.this, Constant.SP_KEY_REBOOT_TIMES, settingRebootTimes);
        getCurrentStatus();
    }
    Handler handler=new Handler(){
        @Override
        public void handleMessage(Message msg)
        {
            super.handleMessage(msg);
            switch (msg.what){
                case MSG_REBOOT:
                    reboot();
                    break;
            }

        }
    };

    public void getCurrentStatus(){
//        settingRebootTimes=3;
        if (currentRebootTimes < 0)
        {
            tv_reboot_status.setText("");
        } else if (currentRebootTimes < settingRebootTimes)
        {
            tv_reboot_status.setText(
                    "current reboot times : " + currentRebootTimes + "\n"
                    + "settting reboot times: " + settingRebootTimes + "\n"
                    + ((currentRebootTimes < settingRebootTimes) ? "device will reboot in 5 s" : "reboot test finish")
            );
            handler.sendEmptyMessageDelayed(MSG_REBOOT,5000);
        }else{
            SPUtils.put(this,Constant.SP_KEY_REBOOT_CURRENT_TIME,-1);
            SPUtils.put(this,Constant.SP_KEY_REBOOT_TIMES,-1);
            tv_reboot_status.setText("current reboot times : " + currentRebootTimes + "\n"
                    + "settting reboot times: " + settingRebootTimes + "\n"
                    + "reboot test finish");
        }
    }

    public void reboot()
    {
            currentRebootTimes++;
            SPUtils.put(RebootActivity.this, Constant.SP_KEY_REBOOT_CURRENT_TIME, currentRebootTimes);


//        Intent intent = new Intent(Intent.ACTION_REBOOT);
//        intent.setAction(Intent.ACTION_REBOOT);
//            intent.putExtra("nowait", 1);
//            intent.putExtra("interval", 1);
//            intent.putExtra("window", 0);
//            sendBroadcast(intent);
        DeviceManager manager = DeviceManager.getInstance();
        manager.reboot();
    }

    private long lastMillis=0;
    @Override
    public boolean onTouchEvent(MotionEvent event)
    {
        if(event.getAction()== KeyEvent.ACTION_DOWN){
            long currentMillis=System.currentTimeMillis();
            if(currentMillis-lastMillis>2000)
            {
                lastMillis=currentMillis;
                Toast.makeText(this,getString(R.string.double_click_exit),Toast.LENGTH_SHORT).show();
            }else{
                SPUtils.put(this,Constant.SP_KEY_REBOOT_CURRENT_TIME,-1);
                SPUtils.put(this,Constant.SP_KEY_REBOOT_TIMES,-1);
                finish();
            }
        }
        return false;
    }

    @Override
    protected void onPause()
    {
        super.onPause();
        handler.removeCallbacksAndMessages(null);
    }
}
