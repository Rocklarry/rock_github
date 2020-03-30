package com.a3nod.lenovo.sparrowfactory.tool;

import android.app.ActivityManager;
import android.content.Context;
import android.content.Intent;
import android.os.PowerManager;

import java.util.List;

/**
 * Created by xinsi on 17-4-19.
 */
public class CommonUtil
{
    public static PowerManager.WakeLock wakeLock;

    public static Intent testService;
    //Get power lock, Keep the screen bright
    public static  void acquireWakeLock(String TAG, Context context){

        if(wakeLock == null){
            PowerManager pm = (PowerManager) context.getSystemService(Context.POWER_SERVICE);
            wakeLock = pm.newWakeLock(PowerManager.FULL_WAKE_LOCK
                    | PowerManager.ACQUIRE_CAUSES_WAKEUP, "wakeLock my Tag");
            if(wakeLock != null){
                wakeLock.acquire();
            }
        }
    }

    //Release power lock
    public static void releaseWakeLock(String TAG, Context context){

        if(wakeLock != null && wakeLock.isHeld()){
            wakeLock.release();
            wakeLock = null;
        }

    }



    //Determine whether the TestService service is started
    public static boolean isRunningTestService(Context mContext, String className){

        boolean isRunning = false;
        ActivityManager activityManager = (ActivityManager) mContext
                .getSystemService(Context.ACTIVITY_SERVICE);
        List<ActivityManager.RunningServiceInfo> serviceList = activityManager
                .getRunningServices(40);

        if (serviceList.size() <= 0) {
            return false;
        }

        for (int i = 0; i < serviceList.size(); i++) {
            if (serviceList.get(i).service.getClassName().equals(className)) {
                isRunning = true;
                break;
            }
        }
        return isRunning;
    }

}
