package com.example.myapplication;



import android.app.Activity;
import android.content.Intent;
import android.os.Bundle;
import android.util.Log;
import android.view.Menu;
import android.view.MenuItem;
import android.view.View;
import android.widget.Button;
import android.widget.Toast;




public class MainActivity extends Activity implements View.OnClickListener {

    private Button tts_test,key_test,back_light,gps_test,Serial_Port;
    private Button camera_N4;
    private Button camera_0, camera_1, camera_2, camera_3;
    private Button cpu_z,service;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_main);

        tts_test = (Button)findViewById(R.id.tts_test);
        camera_N4 = (Button)findViewById(R.id.camera_n4);
        camera_0 = (Button)findViewById(R.id.camera_0);
        camera_1 = (Button)findViewById(R.id.camera_1);
        camera_2 = (Button)findViewById(R.id.camera_2);
        camera_3 = (Button)findViewById(R.id.camera_3);

        key_test = (Button)findViewById(R.id.key_test);
        gps_test = (Button)findViewById(R.id.gps_test);
        back_light = (Button)findViewById(R.id.backlight);
        cpu_z = (Button)findViewById(R.id.cpu_z);
        service = (Button)findViewById(R.id.service);
        Serial_Port = (Button)findViewById(R.id.Serial_Port);

        tts_test.setOnClickListener(this);
        camera_N4.setOnClickListener(this);
        camera_0.setOnClickListener(this);
        camera_1.setOnClickListener(this);
        camera_2.setOnClickListener(this);
        camera_3.setOnClickListener(this);
        key_test.setOnClickListener(this);
        back_light.setOnClickListener(this);
        cpu_z.setOnClickListener(this);
        service.setOnClickListener(this);
        Serial_Port.setOnClickListener(this);


    }

    public void onClick(View v) {
        switch (v.getId()) {
            case R.id.tts_test:
                Log.i("rrd","tts");
                Intent intent = new Intent(MainActivity.this,TTS_Activity.class);
                startActivity(intent);
                break;
            case R.id.camera_n4:
                Intent intent1 = new Intent(MainActivity.this,CameraActivity.class);
                startActivity(intent1);
                break;

            case R.id.key_test:
                Intent intent2 = new Intent(MainActivity.this,KeyActivity.class);
                startActivity(intent2);
                break;

            case R.id.backlight:
                Intent intent3 = new Intent(MainActivity.this,Back_Light.class);
                startActivity(intent3);
                break;

            case R.id.camera_0:
                Intent camera1 = new Intent(MainActivity.this,Camera1_Activity.class);
                startActivity(camera1);
                break;

            case R.id.camera_1:
                Intent camera2 = new Intent(MainActivity.this,Camera2_Activity.class);
                startActivity(camera2);
                break;

            case R.id.camera_2:
                Intent camera3 = new Intent(MainActivity.this,Camera3_Activity.class);
                startActivity(camera3);
                break;

            case R.id.camera_3:
                Intent camera_4 = new Intent(MainActivity.this,Camera4_Activity.class);
                startActivity(camera_4);
                break;

            case R.id.gps_test:
                Intent gps_act = new Intent(MainActivity.this,GpsActivity.class);
                startActivity(gps_act);
                break;
            case R.id.cpu_z:
                Intent intent5 = new Intent(MainActivity.this,Cpu_Activity.class);
                startActivity(intent5);
                break;
            case R.id.service:
                Intent service = new Intent(MainActivity.this,Service_Activity.class);
                startActivity(service);
                break;
            case R.id.Serial_Port:
                Intent intent_Port = new Intent(MainActivity.this,Serial_Activity.class);
                startActivity(intent_Port);
                break;


            default:
                break;
        }
    }


    public boolean onCreateOptionsMenu(Menu menu) {
        getMenuInflater().inflate(R.menu.menu_main, menu);
        return true;
    }

    @Override
    public boolean onOptionsItemSelected(MenuItem item) {
        switch (item.getItemId()) {
            case R.id.action_settings:
                Toast.makeText(this, "add_item", Toast.LENGTH_SHORT).show();
                break;
            case R.id.gps_test:
                Toast.makeText(this, "remove_item", Toast.LENGTH_SHORT).show();
                Intent intent = new Intent(MainActivity.this,GpsActivity.class);
                startActivity(intent);
            default:
                return super.onOptionsItemSelected(item); //不返回true，避免截断菜单项的点击事件
        }
        return true;
    }
}
