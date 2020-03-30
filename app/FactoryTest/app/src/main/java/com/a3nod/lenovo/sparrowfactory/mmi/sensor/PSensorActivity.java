package com.a3nod.lenovo.sparrowfactory.mmi.sensor;

import android.app.Activity;
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
import android.view.FrameStats;
import android.view.View;
import android.widget.Button;
import android.widget.TextView;

import com.a3nod.lenovo.sparrowfactory.Constant;
import com.a3nod.lenovo.sparrowfactory.CrashHandler;
import com.a3nod.lenovo.sparrowfactory.R;
import com.a3nod.lenovo.sparrowfactory.driver.CalibrateCallback;
import com.a3nod.lenovo.sparrowfactory.driver.ProximityService;
import com.a3nod.lenovo.sparrowfactory.mmi.MMITestProcessManager;
import com.a3nod.lenovo.sparrowfactory.mmi.result.ASSYEntity;
import com.a3nod.lenovo.sparrowfactory.tool.LogUtil;

/**
 * Created by jianzhou.peng on 2017/11/14.
 */

public class PSensorActivity extends Activity
{
    public static final String TAG = "MMI_P_SENSOR_TEST";

    private TextView tv_lux;
    private TextView tv_psensor_status;
    private Button btn_pass, btn_fail;//btn_startTest,
    private SensorManager mSensorManager;
    private EventListener mProximityEventListener;
    private ProximityService proximityService;
    private float p_sensor_value;
    private boolean isTesting = false;
//    private boolean autoTestFlag = false; //是否是自动测试，用于判断区分是否是单项测试
    private float tempValue = 0 ;

    private SensorManager.DynamicSensorCallback mDynamicSensorCallback = new SensorManager.DynamicSensorCallback()
    {
        @Override
        public void onDynamicSensorConnected(Sensor sensor)
        {
            if (sensor.getType() == Sensor.TYPE_PROXIMITY)
            {
                Log.i(TAG, "Proximity sensor connected");
                mProximityEventListener = new EventListener("Proximity");
                mSensorManager.registerListener(mProximityEventListener, sensor, SensorManager.SENSOR_DELAY_NORMAL);
            }
        }
    };

    @Override
    protected void onCreate(Bundle savedInstanceState)
    {
//        if(getIntent().getExtras()!=null){
//            if (Constant.INTENT_VALUE_TEST_MODEL_MMI==(int)getIntent().getExtras().get(Constant.INTENT_KEY_TEST_MODEL)){
//                Log.e("TAG","dasda");
//                autoTestFlag = true;
//            }
//        }
        CrashHandler.getInstance().setCurrentTag(TAG,CrashHandler.TYPE_MMI);
        super.onCreate(savedInstanceState);
        setContentView(R.layout.p_sensor_activity);
        initView();
        initConfirmButton();
//        if(autoTestFlag){
//            btn_startTest.setVisibility(View.INVISIBLE);
//        }
    }

    @Override
    protected void onResume()
    {
        super.onResume();
        startProximitySensorRequest();
    }

    @Override
    protected void onPause()
    {
        super.onPause();
        stopProximitySensorRequest();
        handler.removeCallbacksAndMessages(null);
    }

    private void startProximitySensorRequest()
    {
        bindService(new Intent(this, ProximityService.class), connection, Context.BIND_AUTO_CREATE);
        mSensorManager = (SensorManager) getSystemService(SENSOR_SERVICE);
        mSensorManager.registerDynamicSensorCallback(mDynamicSensorCallback);
    }

    private void stopProximitySensorRequest()
    {
        unbindService(connection);
        mSensorManager.unregisterDynamicSensorCallback(mDynamicSensorCallback);
        mSensorManager.unregisterListener(mProximityEventListener);
    }

    private ServiceConnection connection = new ServiceConnection()
    {

        @Override
        public void onServiceDisconnected(ComponentName name)
        {
            proximityService = null;
        }

        @Override
        public void onServiceConnected(ComponentName name, IBinder service)
        {
            proximityService = ((ProximityService.MyBinder) service).getService();
            System.out.println(getString(R.string.service_connect_success));
            // 执行Service内部自己的方法
//            myService.excute();
        }
    };


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
            p_sensor_value = event.values[0];
            Log.i(TAG, name + "values：" + p_sensor_value);
            tv_lux.setText("values:" + p_sensor_value);
//            if (autoTestFlag)
//            {
//                    if(tempValue == 0) {
//                        tempValue = p_sensor_value;
//                    }
//                    if(p_sensor_value != tempValue){
//                        btn_startTest.setText("start test");
//                        tv_psensor_status.setTextColor(Color.GREEN);
//                        tv_psensor_status.setTextSize(50);
//                        tv_psensor_status.setText("test success");
//                        Message message = Message.obtain();
//                        message.what = 0;
//                        handler.sendMessageDelayed(message, 3000);
//                    }
//
//            }else{
//
//                if(isTesting){
//                    if (p_sensor_value >= 5 && p_sensor_value <= 11)
//                    {
//                        isTesting = false;
//                        btn_startTest.setText("start test");
//                        tv_psensor_status.setTextColor(Color.GREEN);
//                        tv_psensor_status.setTextSize(50);
//                        tv_psensor_status.setText("test success");
//                        btn_fail.setEnabled(false);
//                    }
//                }
//            }
                if (Constant.TEST_TYPE_MMI1)
                {
                    if(tempValue == 0) {
                        tempValue = p_sensor_value;
                    }
                    if(p_sensor_value >=10&& p_sensor_value <=40){
                        LogUtil.e("start test");
                       // btn_startTest.setText("start test");
                        tv_psensor_status.setTextColor(Color.GREEN);
                        tv_psensor_status.setTextSize(50);
                        tv_psensor_status.setText("test success");
                        if(Constant.TEST_TYPE_MMI_AUDO) {
                            Message message = Message.obtain();
                            message.what = 0;
                            handler.sendMessageDelayed(message, 2000);
                        }else{
                            btn_pass.setEnabled(true);
                        }
                    }

                }else if(Constant.TEST_TYPE_MMI2){
                    if (p_sensor_value >=10&& p_sensor_value <=40)
                    {
                        Log.e("TAG","测试条件");
                      //  btn_startTest.setText("start test");
                        tv_psensor_status.setTextColor(Color.GREEN);
                        tv_psensor_status.setTextSize(50);
                        tv_psensor_status.setText("test success");
                        if (Constant.TEST_TYPE_MMI_AUDO){
                            Message message = Message.obtain();
                            message.what = 0;
                            handler.sendMessageDelayed(message, 2000);
                        }else {
                            btn_pass.setEnabled(true);
                        }
                    }
                }else{
                    if (p_sensor_value >=10&& p_sensor_value <=80)
                    {
                        btn_pass.setEnabled(true);
                    }
                }

        }

        @Override
        public void onAccuracyChanged(Sensor sensor, int accuracy)
        {
            Log.i(TAG, name + "sensor accuracy changed: " + accuracy);
        }
    }
    //Button btn_pass;
    public void initConfirmButton()
    {
        btn_pass = findViewById(R.id.btn_result_pass);
        btn_pass.setEnabled(false);
        btn_pass.setOnClickListener(new View.OnClickListener()
        {
            @Override
            public void onClick(View v)
            {
                ASSYEntity.getInstants().setPsensorTestResult(true);
                ASSYEntity.getInstants().PSensorValue=p_sensor_value;
                MMITestProcessManager.getInstance().toNextTest();
                finish();
            }
        });
        final Button btn_calibrate = findViewById(R.id.btn_result_reset);
        btn_calibrate.setText("CALIBARATE");
        btn_calibrate.setOnClickListener(new View.OnClickListener()
        {
            @Override
            public void onClick(View view)
            {
                tv_psensor_status.setTextColor(Color.BLACK);
                tv_psensor_status.setTextSize(50);
                tv_psensor_status.setText("calibrating...");
                btn_calibrate.setEnabled(false);
                proximityService.calibrate(new CalibrateCallback()
                {
                    @Override
                    public void onCalibrateFinish()
                    {
                        handler.post(new Runnable()
                        {
                            @Override
                            public void run()
                            {
                                btn_calibrate.setEnabled(true);
                                tv_psensor_status.setText("calibrated");
                            }
                        });
                    }
                });
            }
        });


        btn_fail = findViewById(R.id.btn_result_fail);
        btn_fail.setOnClickListener(new View.OnClickListener()
        {
            @Override
            public void onClick(View v)
            {
                ASSYEntity.getInstants().PSensorValue=p_sensor_value;
                ASSYEntity.getInstants().setPsensorTestResult(false);
                MMITestProcessManager.getInstance().testFail();
                finish();
            }
        });

    }

    Handler handler = new Handler()
    {
        @Override
        public void handleMessage(Message msg)
        {
            switch (msg.what)
            {
                case 0:
                    ASSYEntity.getInstants().setPsensorTestResult(true);
                    MMITestProcessManager.getInstance().toNextTest();
                    finish();
                    break;
                default:
                    break;
            }
            super.handleMessage(msg);
        }
    };

    private void initView()
    {
        tv_lux = findViewById(R.id.lux_text);
        tv_psensor_status = findViewById(R.id.tv_psensor_status);
       // btn_startTest = findViewById(R.id.btn_start_test);
      //  btn_startTest.setText("start test");
       // btn_startTest.setOnClickListener(new View.OnClickListener()
       // {
      //      @Override
       //     public void onClick(View v)
       //     {
       //         if (isTesting)
       //         {
       //             btn_startTest.setText("start test");
       //             isTesting = false;
       //         } else
      //          {
       //             btn_startTest.setText("stop test");
       //             isTesting = true;
      //          }
       //     }
      //  });
    }


    @Override
    protected void onDestroy()
    {
        super.onDestroy();
    }
}
