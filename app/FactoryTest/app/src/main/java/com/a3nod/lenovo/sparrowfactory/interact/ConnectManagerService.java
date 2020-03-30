package com.a3nod.lenovo.sparrowfactory.interact;

import android.app.Service;
import android.content.Intent;
import android.os.IBinder;
import android.support.annotation.Nullable;

import com.a3nod.lenovo.sparrowfactory.tool.LogUtil;

/**
 * 使用服务原因在于  有时候线程被销毁后无法重建
 */
public class ConnectManagerService extends Service
{
    public static final String TAG = "chl";


    @Override
    public void onCreate()
    {
        super.onCreate();
        LogUtil.i("connect manager service onCreate");

        ConnectManager.getInstance().startServer();

    }

    @Override
    public int onStartCommand(Intent intent, int flags, int startId)
    {
        LogUtil.i("connect manager service onStartCommand");

        return super.onStartCommand(intent, flags, startId);
    }

    @Override
    public void onLowMemory()
    {
        super.onLowMemory();
        LogUtil.i("connect manager service onLowMemory");
    }

    @Override
    public void onTrimMemory(int level)
    {
        super.onTrimMemory(level);
        LogUtil.i("connect manager service onTrimMemory  level: " + level);
    }

    @Nullable
    @Override
    public IBinder onBind(Intent intent)
    {
        return null;
    }

    @Override
    public void onDestroy()
    {
        super.onDestroy();
        LogUtil.i("connect manager service onDestroy");
        ConnectManager.getInstance().stopServer();
    }
}
