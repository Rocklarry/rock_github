package com.a3nod.lenovo.sparrowfactory.mmi.sensor;

import android.app.Activity;
import android.app.Service;
import android.content.ComponentName;
import android.content.Context;
import android.content.Intent;
import android.content.ServiceConnection;
import android.graphics.Color;
import android.hardware.Sensor;
import android.hardware.SensorEvent;
import android.hardware.SensorEventListener;
import android.hardware.SensorManager;
import android.os.Bundle;
import android.os.Handler;
import android.os.IBinder;
import android.os.Message;
import android.util.Log;
import android.view.LayoutInflater;
import android.view.View;
import android.view.ViewGroup;
import android.widget.BaseAdapter;
import android.widget.Button;
import android.widget.CheckBox;
import android.widget.CompoundButton;
import android.widget.GridView;
import android.widget.TextView;
import android.widget.Toast;

import com.a3nod.lenovo.sparrowfactory.Constant;
import com.a3nod.lenovo.sparrowfactory.CrashHandler;
import com.a3nod.lenovo.sparrowfactory.R;
import com.a3nod.lenovo.sparrowfactory.driver.AccelerometerService;
import com.a3nod.lenovo.sparrowfactory.driver.CalibrateCallback;
import com.a3nod.lenovo.sparrowfactory.mmi.MMITestProcessManager;
import com.a3nod.lenovo.sparrowfactory.mmi.result.ASSYEntity;
import com.a3nod.lenovo.sparrowfactory.pcba.PCBAHandler;
import com.a3nod.lenovo.sparrowfactory.tool.LogUtil;

import java.util.ArrayList;
import java.util.List;


/**
 * Created by jianzhou.peng on 2017/11/6.
 */

public class MyGVSensorActivity extends Activity
{
    public static final String TAG = "MMI_GSENSOR_TEST";
    public static final int MSG_GET_SENSOR_VALUE = 1;
    TextView tv_sensor_value;
    TextView tv_calibrate_status,tv_xStatus,tv_yStatus,tv_zStatus;
//    GridView gv_sensor_direction;
//    DirectionAdapter da;
//    List<Direction> ld;
    PCBAHandler pcbaHandler;


    private AccelerometerService accelerometerService;
    private MyGVSensor myGVSensor;
    private SensorManager mSensorManager;
    private EventListener mAccelerometerEventListener;
    private EventListener mAmbientTemperatureEventListener;
    private SensorManager.DynamicSensorCallback mDynamicSensorCallback = new SensorManager.DynamicSensorCallback()
    {
        @Override
        public void onDynamicSensorConnected(Sensor sensor)
        {
            if (sensor.getType() == Sensor.TYPE_ACCELEROMETER)
            {
                LogUtil.i("Accelerometer sensor connected");
                mAccelerometerEventListener = new EventListener("Accelerometer");
                mSensorManager.registerListener(mAccelerometerEventListener, sensor, SensorManager.SENSOR_DELAY_NORMAL);
            }
            if (sensor.getType() == Sensor.TYPE_AMBIENT_TEMPERATURE)
            {
                LogUtil.i("Ambient temperature sensor connected");
                mAmbientTemperatureEventListener = new EventListener("Ambient temperature");
                mSensorManager.registerListener(mAmbientTemperatureEventListener, sensor, SensorManager.SENSOR_DELAY_NORMAL);
            }
        }
    };

    @Override
    protected void onCreate(Bundle savedInstanceState)
    {
        CrashHandler.getInstance().setCurrentTag(TAG, CrashHandler.TYPE_MMI);
        super.onCreate(savedInstanceState);
        startAccelerometerSensorRequest();
        setContentView(R.layout.gv_sensor_activity);
        myGVSensor = findViewById(R.id.g_sensor);
        tv_sensor_value = findViewById(R.id.tv_gsensor_value);
        tv_calibrate_status = findViewById(R.id.tv_calibrate_status);
        tv_xStatus= findViewById(R.id.tv_x);
        tv_yStatus= findViewById(R.id.tv_y);
        tv_zStatus= findViewById(R.id.tv_z);
//        gv_sensor_direction = findViewById(R.id.gv_gsensor_item);
        pcbaHandler=new PCBAHandler(this);
        initConfirmButton();
//        initDirection();
    }

//    public void initDirection()
//    {
//        ld = new ArrayList<>();
//        ld.add(new Direction("UPWARD", false));
//        ld.add(new Direction("DOWNWARD", false));
//        ld.add(new Direction("LEFTWARD", false));
//        ld.add(new Direction("RIGHTWARD", false));
//        ld.add(new Direction("UPRIGHT", false));
//        ld.add(new Direction("HANDSTAND", false));
//        da = new DirectionAdapter(MyGVSensorActivity.this, ld);
//        gv_sensor_direction.setAdapter(da);
//    }

//    class Direction
//    {
//        public Direction(String direction, boolean isPass)
//        {
//            this.direction = direction;
//            this.isPass = isPass;
//        }
//
//        String direction;
//        boolean isPass;
//    }
//
//    class DirectionAdapter extends BaseAdapter
//    {
//        List<Direction> lti;
//        Context context;
//
//        public DirectionAdapter(Context context, List<Direction> lti)
//        {
//            this.context = context;
//            this.lti = lti;
//        }
//
//        @Override
//        public int getCount()
//        {
//            return lti.size();
//        }
//
//        @Override
//        public Object getItem(int position)
//        {
//            return lti.get(position);
//        }
//
//        @Override
//        public long getItemId(int position)
//        {
//            return position;
//        }
//
//        @Override
//        public View getView(int position, View convertView, ViewGroup parent)
//        {
//            ViewHolder holder;
//            if (convertView == null)
//            {
//                holder = new ViewHolder();
//                convertView = LayoutInflater.from(context).inflate(R.layout.gi_runin_test_item, parent, false);
//                holder.textView = convertView.findViewById(R.id.gi_tv_test_item);
//                convertView.setTag(holder);
//            } else
//            {
//                holder = (ViewHolder) convertView.getTag();
//            }
//            holder.textView.setText(lti.get(position).direction);
//            if (lti.get(position).isPass)
//            {
//                holder.textView.setTextColor(Color.WHITE);
//                holder.textView.setBackgroundColor(Color.GREEN);
//            } else
//            {
//                holder.textView.setTextColor(Color.DKGRAY);
//                holder.textView.setBackgroundColor(Color.LTGRAY);
//            }
//            return convertView;
//        }
//
//        class ViewHolder
//        {
//            TextView textView;
//        }
//    }

    public void initConfirmButton()
    {
        Button btn_pass = findViewById(R.id.btn_result_pass);
        btn_pass.setEnabled(false);
        btn_pass.setOnClickListener(new View.OnClickListener()
        {
            @Override
            public void onClick(View v)
            {
                ASSYEntity.getInstants().setGsensorTestResult(true);
                MMITestProcessManager.getInstance().toNextTest();
                finish();
            }
        });
        final Button btn_reset = findViewById(R.id.btn_result_reset);
        btn_reset.setText("CALIBRATE");
        btn_reset.setOnClickListener(new View.OnClickListener()
        {
            @Override
            public void onClick(View view)
            {
                btn_reset.setEnabled(false);
                tv_calibrate_status.setText("calibrating");
                accelerometerService.calibrate(new CalibrateCallback()
                {
                    @Override
                    public void onCalibrateFinish()
                    {
                        handler.post(new Runnable()
                        {
                            @Override
                            public void run()
                            {
                                btn_reset.setEnabled(true);
                                tv_calibrate_status.setText("calibrated");
                            }
                        });
                    }
                });
            }
        });

        Button btn_fail = findViewById(R.id.btn_result_fail);
        btn_fail.setOnClickListener(new View.OnClickListener()
        {
            @Override
            public void onClick(View v)
            {
                ASSYEntity.getInstants().setGsensorTestResult(false);
                MMITestProcessManager.getInstance().testFail();
                finish();
            }
        });
    }

    private void startAccelerometerSensorRequest()
    {
        bindService(new Intent(this, AccelerometerService.class), connection, Service.BIND_AUTO_CREATE);
        mSensorManager = (SensorManager) getSystemService(Context.SENSOR_SERVICE);
        mSensorManager.registerDynamicSensorCallback(mDynamicSensorCallback);
    }

    private void stopAccelerometerSensorRequest()
    {
        unbindService(connection);
        mSensorManager.unregisterDynamicSensorCallback(mDynamicSensorCallback);
        mSensorManager.unregisterListener(mAccelerometerEventListener);
        mSensorManager.unregisterListener(mAmbientTemperatureEventListener);
    }

    ServiceConnection connection = new ServiceConnection()
    {
        @Override
        public void onServiceConnected(ComponentName name, IBinder service)
        {
            accelerometerService = ((AccelerometerService.MyBinder) service).getService();
        }

        @Override
        public void onServiceDisconnected(ComponentName name)
        {
            accelerometerService = null;
        }
    };
    private boolean[] autoBool = new boolean[]{false, false, false};
    private boolean isPass=false;
    private boolean isFirst=true;

    public static boolean rangeInDefined(float current, float min, float max)
    {
        return Math.max(min, current) == Math.min(current, max);
    }


    private class EventListener implements SensorEventListener
    {

        private final String name;

        EventListener(String name)
        {
            this.name = name;
        }

        @Override
        public void onSensorChanged(SensorEvent event)
        {
            Log.d("onSensorChanged",event.sensor.getType()+"----"+event.sensor.getStringType()+"---array.length ="+event.values.length);
            for (int i = 0; i < event.values.length; i++)
            {
                Log.i("onSensorChanged", name + " sensor changed: " + event.values[i]);
            }
            if(event.sensor.getType() == Sensor.TYPE_ACCELEROMETER){
                Log.e("TAG", "EVENT" + event.values[0]);
                if (!autoBool[0])
                {
                    autoBool[0] = (rangeInDefined(event.values[0], -1, 1) && rangeInDefined(event.values[1], -1, 1) && rangeInDefined(event.values[2], 8.8f, 10.8f));
                    if (autoBool[0])
                    {
                        tv_zStatus.setVisibility(View.VISIBLE);
                    }
                }
                if (!autoBool[1])
                {
                    autoBool[1] = (rangeInDefined(event.values[0], -1, 1) && rangeInDefined(event.values[1], 8.8f, 10.8f) && rangeInDefined(event.values[2], -1f, 1f));
                    if (autoBool[1])
                    {
                        tv_yStatus.setVisibility(View.VISIBLE);
                    }
                }
                if (!autoBool[2])
                {
                    autoBool[2] = (rangeInDefined(event.values[0], 8.8f, 10.8f) && rangeInDefined(event.values[1], -1, 1) && rangeInDefined(event.values[2], -1, 1));
                    if (autoBool[2])
                    {
                        tv_xStatus.setVisibility(View.VISIBLE);
                    }
                }
                for (int i = 0; i < event.values.length; i++)
                {
                    Log.i("TAG", name + " sensor changed: " + event.values[i]);
                }
                myGVSensor.SensorChange(event);
                handler.obtainMessage(MSG_GET_SENSOR_VALUE, event).sendToTarget();
                isPass = autoBool[0] && autoBool[1] && autoBool[2] ;
                if(Constant.TEST_TYPE_MMI1){
                    if(autoBool[0]&&isFirst){
                        isFirst=false;
                        pcbaHandler.sendEmptyMessageDelayed(PCBAHandler.MSG_TEST_SUCCESS,PCBAHandler.DELAY);
                    }

                }else if(Constant.TEST_TYPE_MMI2){
                    if(isPass&&isFirst){
                        isFirst=false;
                        pcbaHandler.sendEmptyMessageDelayed(PCBAHandler.MSG_TEST_SUCCESS,PCBAHandler.DELAY);
                    }
                }else{
                    if (tv_zStatus.isCursorVisible()&&tv_xStatus.isCursorVisible()&&tv_yStatus.isCursorVisible()){
                        findViewById(R.id.btn_result_pass).setEnabled(true);
                    }
                }
            }
        }

        @Override
        public void onAccuracyChanged(Sensor sensor, int accuracy)
        {

        }
    }

    Handler handler = new Handler()
    {
        @Override
        public void handleMessage(Message msg)
        {
            super.handleMessage(msg);
            switch (msg.what)
            {
                case MSG_GET_SENSOR_VALUE:
                    SensorEvent event = (SensorEvent) msg.obj;
                    float x = event.values[0];
                    //x>0 说明当前手机左翻 x<0右翻
                    float y = event.values[1];
                    //y>0 说明当前手机下翻 y<0上翻
                    float z = event.values[2];

                    tv_sensor_value.setText(
                            "X : " + x + "\n"
                                    + "Y : " + y + "\n"
                                    + "Z : " + z
                    );

                    break;
            }
        }
    };

    @Override
    protected void onDestroy()
    {
        super.onDestroy();
//        SharedPreferences sharedPreferences=getSharedPreferences("Gsensor",MODE_PRIVATE);
//        SharedPreferences.Editor editor=sharedPreferences.edit();
//        editor.putFloat()
        stopAccelerometerSensorRequest();
    }
}
