package com.a3nod.lenovo.sparrowfactory.pcba;

import android.app.Activity;
import android.content.Intent;
import android.os.Bundle;
import android.view.KeyEvent;
import android.view.MotionEvent;
import android.view.View;
import android.widget.AdapterView;
import android.widget.ArrayAdapter;
import android.widget.ImageView;
import android.widget.ListView;
import android.widget.TextView;
import android.widget.Toast;

import com.a3nod.lenovo.sparrowfactory.CrashHandler;
import com.a3nod.lenovo.sparrowfactory.R;
import com.a3nod.lenovo.sparrowfactory.Constant;
import com.a3nod.lenovo.sparrowfactory.mmi.MainActivity;

/**
 * Created by Lenovo on 2017/11/13.
 */

public class PcbaTestActivity extends Activity
{
    public static final String TAG = "PcbaTestActivity";
    private ListView mlist;


    @Override
    protected void onCreate(Bundle savedInstanceState)
    {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_pcba_main);
        initView();
        initTitle();
    }

    public void initTitle()
    {
        TextView tv_title = findViewById(R.id.tv_menu_title);
        tv_title.setText(R.string.pcba);
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

    public void initView()
    {
        mlist = findViewById(R.id.lv_pcba_list);
        String[] data = {"LCD", "TP", /*"Camera", */ "Mic", "Speaker", /*"G-Sensor",*/ "P-Sensor", "L-Sensor","RGB-Sensor", "Key","Bluetooth","WIFI", "DataCheck"};
        ArrayAdapter<String> arrayAdapter = new ArrayAdapter<>(this, android.R.layout.simple_list_item_1, data);
        mlist.setAdapter(arrayAdapter);
        mlist.setOnItemClickListener(new AdapterView.OnItemClickListener()
        {
            @Override
            public void onItemClick(AdapterView<?> adapterView, View view, int i, long l)
            {
                switch (i)
                {
                    case 0:
                        startOtherActivity("pcba_lcd");
                        break;
                    case 1:
                        startOtherActivity("pcba_tp");
                        break;
           /*         case 2:
//                        Intent intent = new Intent("hello.testActivity");
//                        intent.putExtra(Constant.INTENT_KEY_TEST_MODEL, Constant.INTENT_VALUE_TEST_MODEL_PCBA);
//                        startActivity(intent);
                        startOtherActivity("pcba_camera");
                        break;*/
                    case 2:
                        //startOtherActivity("pcba_mic");
                        Intent intentMic=new Intent("pcba_mic");
                        intentMic.putExtra(Constant.INTENT_KEY_TEST_MODEL,Constant.INTENT_VALUE_TEST_MODEL_PCBA);
                        startActivity(intentMic);
                        //startActivity(new Intent(PcbaTestActivity.this, PCBAMicTestActivity.class));
                        break;
                    case 3:
                        startOtherActivity("pcba_speaker");
                        // startActivity(new Intent(PcbaTestActivity.this, PCBASpeakerTestActivity.class));
                        break;
        /*            case 5:
                        startOtherActivity("hello.PCBAGSensorActivity");
                        break; */
                    case 4:
                        startOtherActivity("hello.PCBAPSensorActivity");
                        break;
                    case 5:
                        startOtherActivity("hello.PCBALSensorActivity");
                        break;
                    case 6:
                        startOtherActivity("pcbargb.sensor.action");
                        break;
                    case 7:
                        startOtherActivity("pcba_button");
                        break;
                    case 8:
                        Intent bluetoothIntent=new Intent("hello.BluetoothActivity");
                        bluetoothIntent.putExtra(Constant.INTENT_KEY_TEST_MODEL, Constant.INTENT_VALUE_TEST_MODEL_PCBA);
                        startActivity(bluetoothIntent);
                        break;
                    case 9:
                        Intent wifiIntent=new Intent("wifi.action");
                        wifiIntent.putExtra(Constant.INTENT_KEY_TEST_MODEL, Constant.INTENT_VALUE_TEST_MODEL_PCBA);
                        startActivity(wifiIntent);
                        break;
                    case 10:
                        startOtherActivity("pcba.datacheck.action");
                        // startActivity(new Intent(PcbaTestActivity.this, PCBADataCheckTestActivity.class));
                        break;
                }
            }
        });
    }

    void startOtherActivity(String action)
    {
        Intent intent = new Intent(action);
        startActivity(intent);
    }

    @Override
    protected void onDestroy()
    {
        super.onDestroy();
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
                CrashHandler.getInstance().setCurrentTag(TAG, CrashHandler.TYPE_OTHER);
                finish();
            }
        }
        return false;
    }
}
