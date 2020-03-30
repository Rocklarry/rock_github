package com.learn.app_test.test;

import androidx.appcompat.app.AppCompatActivity;

import android.os.Bundle;
import android.text.method.ScrollingMovementMethod;
import android.util.Log;
import android.view.View;
import android.widget.Button;
import android.widget.TextView;
import com.learn.app_test.R;
import com.learn.app_test.mqtt.MqttManager;
import com.learn.app_test.utrl.BaseFunction;


import org.eclipse.paho.client.mqttv3.MqttConnectOptions;
import org.eclipse.paho.client.mqttv3.MqttException;
import org.eclipse.paho.client.mqttv3.MqttMessage;
import org.greenrobot.eventbus.EventBus;
import org.greenrobot.eventbus.Subscribe;
import org.greenrobot.eventbus.ThreadMode;

public class Mqtt_Activity extends AppCompatActivity {

    Button bt_connect;
    Button bt_sub;
    Button bt_pub;
    Button bt_unconnect;
    Button bt_test;
    TextView tv_mag;
    BaseFunction baseFunction;

    public static final String URL = "tcp://192.168.123.172:1883";
    private String userName = "admin";
    private String password = "password";
    private String clientId = "901000";
    private MqttConnectOptions mMqttConnectOptions;
    String TAG = "Mqtt:\n";


    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate ( savedInstanceState );
        setContentView ( R.layout.activity_mqtt );

        bt_connect = findViewById ( R.id.bt_connect );
        bt_pub = findViewById ( R.id.bt_pub );
        bt_sub = findViewById ( R.id.bt_sub );
        bt_unconnect = findViewById ( R.id.bt_unconnect );
        bt_test = findViewById ( R.id.test );
        tv_mag = findViewById ( R.id.textView );

        initBaseFunction();
        EventBus.getDefault().register(this);

        bt_pub.setEnabled ( false );
        bt_sub.setEnabled ( false );
        bt_unconnect.setEnabled ( false );
        bt_test.setEnabled ( false );
        tv_mag.setMovementMethod( ScrollingMovementMethod.getInstance());


        initMqtt();

        //测试不用线程也可以，但是网络不好奔溃
        bt_connect.setOnClickListener ( new View.OnClickListener ( ) {
            @Override
            public void onClick(View view) {
                //创建客户端
                //new Thread ( new Runnable ( ) {
                 //   @Override
                   // public void run() {
                        boolean b = MqttManager.getInstance ( ).creatConnect ( URL, userName, password, clientId );
                        Log.d ( TAG, "isConnected: " + b );
                tv_mag.append ( "MQTT连接:"+b+"\n");
                    //}
                //} ).start ( );

                bt_pub.setEnabled ( true );
                bt_sub.setEnabled ( true );
                bt_unconnect.setEnabled ( true );
                bt_test.setEnabled ( true );
                bt_connect.setEnabled ( false );


            }
        } );

        bt_sub.setOnClickListener ( new View.OnClickListener ( ) {
            @Override
            public void onClick(View view) {
                final String topic = "temperature";
                new Thread(new Runnable() {
                    @Override
                    public void run() {
                        MqttManager.getInstance().subscribe(topic, 1);
                        tv_mag.append ( "订阅主题 SUB:\n topic:"+topic+"\n");
                        tv_mag_move(tv_mag);

                    }
                }).start();
            }

        } );

        bt_test.setOnClickListener ( new View.OnClickListener ( ) {
            @Override
            public void onClick(View view) {
                final String topic = "temperature";
                final String TOPIC_MESSAGE = "88888°";
                new Thread ( new Runnable ( ) {
                    public void run() {
                        MqttManager.getInstance ().publish ( topic,1,TOPIC_MESSAGE.getBytes () );
                        tv_mag.append ( "发布测试 :\n topic:"+topic+"\r\r\r message"+TOPIC_MESSAGE+"\n");
                        tv_mag_move(tv_mag);
                    }
                } ).start ( );
            }
        });

        bt_pub.setOnClickListener ( new View.OnClickListener ( ) {
            @Override
            public void onClick(View view) {
                new Thread (new Runnable (){
                    final String topic = "Question";
                    final String TOPIC_MESSAGE = "What's the temperature?";

                    @Override
                    public void run() {
                        MqttManager.getInstance ().publish ( topic,1,TOPIC_MESSAGE.getBytes () );
                        tv_mag.append ( "发布主题:\n topic:"+topic+"\r\r\r message"+TOPIC_MESSAGE+"\n");
                        tv_mag_move(tv_mag);

                    }
                }).start ();
            }
        } );


        bt_unconnect .setOnClickListener ( new View.OnClickListener ( ) {
            @Override
            public void onClick(View view) {
               // new Thread(new Runnable() {
                //    @Override
                   // public void run() {
                        try {
                            MqttManager.getInstance().disConnect();
                            tv_mag.append ( "断开连接  \n");
                            tv_mag_move(tv_mag);
                            } catch (MqttException e) {
                        }
            //        }
             //   }).start();
                bt_pub.setEnabled ( false );
                bt_sub.setEnabled ( false );
                bt_test.setEnabled ( false );
                bt_connect.setEnabled ( true );
                bt_unconnect.setEnabled ( false );
            }
        } );


    }

    private void initMqtt() {
        mMqttConnectOptions = new MqttConnectOptions();
        mMqttConnectOptions.setCleanSession(true); //设置是否清除缓存
        mMqttConnectOptions.setConnectionTimeout(10); //设置超时时间，单位：秒
        mMqttConnectOptions.setKeepAliveInterval(20); //设置心跳包发送间隔，单位：秒
        //mMqttConnectOptions.setUserName(USERNAME); //设置用户名
        //mMqttConnectOptions.setPassword(PASSWORD.toCharArray()); //设置密码
    }


    private void tv_mag_move(TextView textView){
        int offset=textView.getLineCount()*textView.getLineHeight();
        if(offset>(textView.getHeight()-textView.getLineHeight()-20)){
            textView.scrollTo(0,offset-textView.getHeight()+textView.getLineHeight()+20);
        }
    }



    @Subscribe(threadMode = ThreadMode.MAIN,sticky = true)
    public void Event(MqttMessage message) {
        Log.d ( TAG,"接受到messeage == "+message);
        tv_mag.append("\n 接受到:"+message.toString ());
        tv_mag_move(tv_mag);
    }



    private void initBaseFunction() {
        if (baseFunction == null) {
            baseFunction = new BaseFunction (this);
        }
    }

    /**
     * 订阅接收到的消息
     * 这里的Event类型可以根据需要自定义, 这里只做基础的演示
     *
     * @param message
     */
        public void onEvent(MqttMessage message) {
            Log.d(TAG,message.toString());
            tv_mag.append("\n onEvent:"+message.toString ());
            tv_mag_move(tv_mag);
        }

    protected void onDestroy() {
        super.onDestroy ( );
        if (EventBus.getDefault ( ).isRegistered ( this )) {
            EventBus.getDefault ( ).unregister ( this );
        }
    }


}