package com.a3nod.lenovo.sparrowfactory.mmi.sensor;
import android.content.Context;
import android.hardware.Sensor;
import android.hardware.SensorEvent;
import android.hardware.SensorEventListener;
import android.hardware.SensorManager;
import android.util.Log;


/**
 * Created by jianzhou.peng on 2017/11/8.
 */

public class LightSensor implements SensorEventListener {
    private static volatile LightSensor instance = null;
    private SensorManager mSensorManager = null;
    private static Context mContext = null;
    private Sensor mSensor = null;

    //外部传过来的监听器
    private onGetLuxValueListener mOnGetLuxValueListener = null;

    //设置监听器
    public void setOnGetLuxValueListener(onGetLuxValueListener listener){
            mOnGetLuxValueListener = listener;
    }

    private LightSensor(){
    }
    public static LightSensor getInstance(Context context){
        if(instance == null){
            synchronized (LightSensor.class){
                if(instance == null){
                    instance = new LightSensor();
                    mContext = context;
                }
            }
        }
        return instance;
    }

    public void initSensor(){
        mSensorManager = (SensorManager) mContext.getSystemService(Context.SENSOR_SERVICE);
        mSensor = mSensorManager.getDefaultSensor(Sensor.TYPE_LIGHT);
        if(mSensor == null){
            Log.e("TAG","没有光传感器");
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
           Log.e("TAG","获取的值 value 为"+value);
           if(mOnGetLuxValueListener != null){
                mOnGetLuxValueListener.getValue(value);
            }
    }

    @Override
    public void onAccuracyChanged(Sensor sensor, int accuracy) {

    }

    //定义一个接口和接口实现
    public interface onGetLuxValueListener{
        void getValue(float value);
    }
}
