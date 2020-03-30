package com.a3nod.lenovo.sparrowfactory.mmi.sensor;

import android.content.Context;
import android.hardware.Sensor;
import android.hardware.SensorEvent;
import android.hardware.SensorEventListener;
import android.hardware.SensorManager;
import android.util.Log;

/**
 * Created by jianzhou.peng on 2017/11/14.
 */

public class PSensor implements SensorEventListener{

    private static volatile PSensor instance = null;
    private static Context mContext;
    private SensorManager mSensorManager = null;
    private Sensor mSensor = null;

    //外部传过来的监听器
    private onGetPValueListener mOnGetpValueListener = null;

    //设置监听器
    public void setOnGetValueListener(onGetPValueListener listener){
        mOnGetpValueListener = listener;
    }
    private PSensor(){

    }
    public static PSensor getInstance (Context context){
        if(instance == null){
            synchronized (PSensor.class){
                if(instance == null){
                    instance = new PSensor();
                    mContext = context;
                }
            }
        }
        return instance;
    }

    public void initSensor(){
        mSensorManager = (SensorManager) mContext.getSystemService(Context.SENSOR_SERVICE);
        mSensor = mSensorManager.getDefaultSensor(Sensor.TYPE_PROXIMITY);
        if(mSensor==null){
            Log.e("TAG","没有距离传感器");
        }
        registerSensorListener();
    }

    public void registerSensorListener() {
        if(mSensorManager!=null&&mSensor!=null){
            mSensorManager.registerListener(this,mSensor, SensorManager.SENSOR_DELAY_FASTEST);
        }
    }

    public void unregisterSensorListener(){
        mSensorManager.unregisterListener(this);
        Log.e("TAG","注销当前的监听器");
    }

    @Override
    public void onSensorChanged(SensorEvent event) {
        float value = event.values[0];
        Log.e("TAG","光传感器获取的值 event 为"+value);
        if(mOnGetpValueListener != null){
            mOnGetpValueListener.getPValue(value);
        }
    }

    @Override
    public void onAccuracyChanged(Sensor sensor, int accuracy) {

    }

    //定义一个接口和接口实现
    public interface onGetPValueListener{
        void getPValue(float value);
    }

}
