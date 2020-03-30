package com.a3nod.lenovo.sparrowfactory.mmi;

import android.app.Activity;
import android.content.Intent;
import android.os.Bundle;
import android.util.Log;
import android.view.KeyEvent;
import android.view.MotionEvent;
import android.view.View;
import android.widget.AdapterView;
import android.widget.Button;
import android.widget.GridView;
import android.widget.ImageView;
import android.widget.RadioButton;
import android.widget.RadioGroup;
import android.widget.TextView;
import android.widget.Toast;

import com.a3nod.lenovo.sparrowfactory.R;
import com.a3nod.lenovo.sparrowfactory.pcba.PCBAMicTestActivity;
import com.a3nod.lenovo.sparrowfactory.Constant;

import java.util.ArrayList;
import java.util.Arrays;
import java.util.List;


/**
 * Skeleton of an Android Things activity.
 * <p>
 * Android Things peripheral APIs are accessible through the class
 * PeripheralManagerService. For example, the snippet below will open a GPIO pin and
 * set it to HIGH:
 * <p>
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
public class MainActivity extends Activity implements View.OnClickListener{
    private GridView mlist;
    private MainGridAdapter arrayAdapter;
    private Button bt_start;
    private RadioButton rd_bt1,rd_bt2;
    private RadioGroup rg_group;
    private Boolean mmi1 = false,mmi2 = false;
    @Override
    protected void onCreate(Bundle savedInstanceState)
    {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_main);
        initTitle();
        initView();
        bt_start.setOnClickListener(this);
    }
    public void initTitle()
    {
        TextView tv_title = findViewById(R.id.tv_menu_title);
        tv_title.setText(R.string.assymmi);
        ImageView iv_back = findViewById(R.id.iv_menu_back);
        iv_back.setOnClickListener(new View.OnClickListener()
        {
            @Override
            public void onClick(View v)
            {
                finish();
            }
        });
    }
    void startOtherActivity(String action)
    {
        Intent intent = new Intent(action);
        startActivity(intent);
    }

    private void initView()
    {
        mlist = findViewById(R.id.item_id);
        rg_group = findViewById(R.id.test_type);
        rg_group.setOnCheckedChangeListener(new RadioGroup.OnCheckedChangeListener() {
            @Override
            public void onCheckedChanged(RadioGroup group, int checkedId) {
                if(checkedId==R.id.mmi1){
                    Constant.TEST_TYPE_MMI1 = true;
                    Constant.TEST_TYPE_MMI2 = false;
                    Log.e("TAG","mmi1");
                }
                else if(checkedId==R.id.mmi2){
                    Constant.TEST_TYPE_MMI1 = false;
                    Constant.TEST_TYPE_MMI2 = true;
                    Log.e("TAG","mmi2");
                }
            }
        });
        rd_bt1 = findViewById(R.id.mmi1);
        rd_bt2 = findViewById(R.id.mmi2);
        bt_start = findViewById(R.id.start_test);
        String[] data = {/*"Camera",*/ "Backlight", "Buttons", "Speaker", "Mic", /* "G-sensor",*/ "P-sensor", "L-sensor","RGB-sensor", "BT", "Wifi", "LCD", "TP", "Mult-point", "Data check", "Last Result"};
        List<String> list= Arrays.asList(data);


        arrayAdapter = new MainGridAdapter(list, this);
        mlist.setAdapter(arrayAdapter);
        mlist.setOnItemClickListener(new AdapterView.OnItemClickListener()
        {
            @Override
            public void onItemClick(AdapterView<?> adapterView, View view, int i, long l)
            {
                switch (i)
                {
               /*     case 0:
                        Intent intentCamera = new Intent("hello.testActivity");
                        intentCamera.putExtra(Constant.INTENT_KEY_TEST_MODEL,Constant.INTENT_VALUE_TEST_MODEL_MMI);
                        startActivity(intentCamera);
                        break; */
                    case 0:
                        startOtherActivity("backlight.action");
                        break;
                    case 1:
                        startOtherActivity("button.action");
                        break;
                    case 2:
                        startOtherActivity("hello.SpeakerActivity");
                        break;
                    case 3:
                        startOtherActivity("pcba_mic");
//                        Intent intentMic=new Intent(MainActivity.this, PCBAMicTestActivity.class);
//                        intentMic.putExtra(Constant.INTENT_KEY_TEST_MODEL,Constant.INTENT_VALUE_TEST_MODEL_MMI);
//                        startActivity(intentMic);
                        break;
            /*        case 5:
                        startOtherActivity("gv_sensor.action");
//                        startActivity(new Intent(MainActivity.this, GSensorActivity.class));
                        break; */
                    case 4:
                        startOtherActivity("p_sensor.action");
                        break;
                    case 5:
                        startOtherActivity("l_sensor.action");
                        break;
                    case 6:
                        startOtherActivity("rgb.sensor.action");
                        break;
                    case 7:
                        Intent intentBluetooth=new Intent("hello.BluetoothActivity");
                        intentBluetooth.putExtra(Constant.INTENT_KEY_TEST_MODEL,Constant.INTENT_VALUE_TEST_MODEL_MMI);
                        startActivity(intentBluetooth);
//                        startOtherActivity(""hello.BluetoothActivity");
                        break;
                    case 8:
                        Intent intentWifi=new Intent("wifi.action");
                        intentWifi.putExtra(Constant.INTENT_KEY_TEST_MODEL,Constant.INTENT_VALUE_TEST_MODEL_MMI);
                        startActivity(intentWifi);
//                        startOtherActivity("wifi.action");

                        break;
                    case 9:
                        startOtherActivity("lcd.action");
                        break;
                    case 10:
                        startOtherActivity("hello.TPTestActivity");
                        break;
                    case 11:
                        startOtherActivity("hello.MultPointActivity");
                        break;
                    case 12:
                      //  startOtherActivity("pcba.datacheck.action");
                        startOtherActivity("datacheck.action");
                        break;
                    case 13:
                        startOtherActivity("hello.ResultActivity");
                        break;
                }
            }
        });
    }

    @Override
    protected void onResume()
    {
        super.onResume();
        arrayAdapter.updateData();
    }

    @Override
    protected void onDestroy() {
        super.onDestroy();
        Constant.TEST_TYPE_MMI1 = false;
        Constant.TEST_TYPE_MMI2 = false;
    }

    private long lastMillis = 0;

    @Override
    public boolean onTouchEvent(MotionEvent event)
    {
        if (event.getAction() == KeyEvent.ACTION_DOWN)
        {
            long currentMillis = System.currentTimeMillis();
            if (currentMillis - lastMillis > 2000)
            {
                lastMillis = currentMillis;
                Toast.makeText(this, getString(R.string.double_click_exit), Toast.LENGTH_SHORT).show();
            } else
            {
                finish();
            }
        }
        return false;
    }
    @Override
    public void onClick(View v) {
       switch (v.getId()){
           //点击主界面开始测试
           case R.id.start_test:
               startTest();
           break;
           default:
               break;
       }
    }

    /**
     * 开始测试
     */
    private void startTest() {
        if(Constant.TEST_TYPE_MMI1==false&&Constant.TEST_TYPE_MMI2==false){
            Toast.makeText(this,"please select test type",Toast.LENGTH_LONG).show();
            return;
        }
        Constant.TEST_TYPE_MMI_AUDO = true;
        MMITestProcessManager.getInstance().resetCurrentTimes();
        MMITestProcessManager.getInstance().setApplicationContext(this,true);
        MMITestProcessManager.getInstance().toNextTest();
    }
}
