package com.example.myapplication;

import android.app.AlarmManager;
import android.app.PendingIntent;
import android.app.Service;
import android.content.Intent;
import android.os.IBinder;
import android.os.SystemClock;
import android.util.Log;

import java.util.Date;

public class MyService extends Service {

    public static final String TAG = "MyService";

    public static int type;


    public MyService() {
    }

    @Override
    public IBinder onBind(Intent intent) {
        // TODO: Return the communication channel to the service.
        throw new UnsupportedOperationException("Not yet implemented");
    }


    public void onCreate() {
        super.onCreate();
        Log.i(TAG, "onCreate() executed");
    }

    public int onStartCommand(Intent intent, int flags, int startId) {
        Log.i(TAG, "onStartCommand() executed");

        /*new Thread(new Runnable() {
            @Override
            public void run() {
                type++;
                Log.i("LongRunningService", "executed at " + new Date().toString()+"===="+type);


            }
        }).start();
*/

        type++;
        Log.i("LongRunningService", "executed at " + new Date().toString()+"===="+type);

        AlarmManager manager = (AlarmManager) getSystemService(ALARM_SERVICE);
        //int anHour = 60 * 60 * 1000; // 这是一小时的毫秒数
        int anHour = 1000; // 这是一小时的毫秒数
        long triggerAtTime = SystemClock.elapsedRealtime() + anHour;

        Intent i = new Intent(this, AlarmReceiver.class);
        PendingIntent pi = PendingIntent.getBroadcast(this, 0, i, 0);
        manager.set(AlarmManager.ELAPSED_REALTIME_WAKEUP, triggerAtTime, pi);


        return super.onStartCommand(intent, flags, startId);
    }

    @Override
    public void onDestroy() {
        super.onDestroy();
        Log.i(TAG, "onDestroy() executed");
    }

}
