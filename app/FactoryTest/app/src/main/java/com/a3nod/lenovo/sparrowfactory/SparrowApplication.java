package com.a3nod.lenovo.sparrowfactory;

import android.app.Application;
import android.graphics.Typeface;

import com.a3nod.lenovo.sparrowfactory.runin.RuninConfig;
import com.squareup.leakcanary.RefWatcher;

import java.lang.reflect.Field;

/**
 * Description
 * Created by aaa on 2017/11/15.
 */

public class SparrowApplication extends Application
{
    private static SparrowApplication instance;
    private RefWatcher mRefWatcher;
    private boolean DEBUG=false;
    @Override
    public void onCreate()
    {
        super.onCreate();
        instance=this;
        RuninConfig.init(this);
        CrashHandler.init(this);

        setChineseWord();
//        mRefWatcher = DEBUG ?  LeakCanary.install(this) : RefWatcher.DISABLED;
    }
    public void setChineseWord(){

        Typeface  typefaceStHeiTi = Typeface.createFromAsset(getAssets(), "fonts/DroidSansFallback.ttf");
        try {
            Field field = Typeface.class.getDeclaredField("MONOSPACE");
            field.setAccessible(true);
            field.set(null, typefaceStHeiTi);
        } catch (NoSuchFieldException e) {
            e.printStackTrace();
        } catch (IllegalAccessException e) {
            e.printStackTrace();
        }

    }
    public static SparrowApplication getInstance() {
        return instance;
    }

    public static RefWatcher getRefWatcher() {
        return getInstance().mRefWatcher;
    }
}
