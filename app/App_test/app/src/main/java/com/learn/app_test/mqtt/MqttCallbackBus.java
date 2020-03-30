package com.learn.app_test.mqtt;
import android.util.Log;

import org.eclipse.paho.client.mqttv3.IMqttDeliveryToken;
import org.eclipse.paho.client.mqttv3.MqttCallback;
import org.eclipse.paho.client.mqttv3.MqttMessage;
import org.greenrobot.eventbus.EventBus;

class MqttCallbackBus implements MqttCallback {
    private static final String TAG = MqttCallbackBus.class.getCanonicalName();


    @Override
    public void connectionLost(Throwable cause) {
        Log.e(TAG,cause.getMessage());
    }

    @Override
    public void messageArrived(String topic, MqttMessage message) throws Exception {
        Log.d(TAG,topic + "====" + message.toString());
        EventBus.getDefault().post(message);
    }

    @Override
    public void deliveryComplete(IMqttDeliveryToken token) {

    }
}
