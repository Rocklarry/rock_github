package com.learn.app_test;

import androidx.appcompat.app.AppCompatActivity;

import android.content.Intent;
import android.os.Bundle;
import android.view.Menu;
import android.view.MenuInflater;
import android.view.MenuItem;
import android.widget.TextView;

import com.learn.app_test.test.AsyncTaskActivity;
import com.learn.app_test.test.Mqtt_Activity;
import com.learn.app_test.test.paho_mqtt_activity;

public class MainActivity extends AppCompatActivity {

    // Used to load the 'native-lib' library on application startup.
    static {
        System.loadLibrary ( "native-lib" );
    }

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate ( savedInstanceState );
        setContentView ( R.layout.activity_main );

        // Example of a call to a native method
        TextView tv = findViewById ( R.id.sample_text );
        tv.setText ( stringFromJNI ( ) );

        TextView test_jni = findViewById ( R.id.test_jni );
        test_jni.setText ( stringTestJNI () );
    }

    boolean shareSupport = true;

    public boolean onCreateOptionsMenu(Menu menu){
        MenuInflater inflater = new MenuInflater ( this );
        inflater.inflate(R.menu.menu_test, menu);
        //动态加载
        if(shareSupport){
            menu.add ( Menu.NONE,Menu.FIRST, Menu.NONE,"share" );
        }
        //返回true 才可见
        return  true;
    }

    public  boolean onPrepareOptionsMenu(Menu menu){
        return  super.onPrepareOptionsMenu ( menu );
    }

    public boolean onOptionsItemSelected(MenuItem item){
        switch (item.getItemId ()){
            case R.id.asyncTask:
                Intent intent = new Intent(MainActivity.this, AsyncTaskActivity.class);
                startActivity(intent);

            case R.id.mqtt_test:
                Intent intent_mqtt = new Intent(MainActivity.this, Mqtt_Activity.class);
                startActivity(intent_mqtt);

            case R.id.file_delete:
                break;
            case R.id.file_more:
                break;
            case Menu.FIRST:
                break;
            case R.id.paho_mqtt:
                Intent integer = new Intent ( MainActivity.this, paho_mqtt_activity.class );
                startActivity ( integer );

        }
        return  super.onOptionsItemSelected ( item );
    }



    public native String stringFromJNI();
    public native String stringTestJNI();
}
