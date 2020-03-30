package com.a3nod.lenovo.sparrowfactory.runin;

import android.content.BroadcastReceiver;
import android.content.Context;
import android.content.Intent;


public class BootupReceiver extends BroadcastReceiver
{

    @Override
    public void onReceive(Context context, Intent intent)
    {
        if (intent.getAction().equals(Intent.ACTION_BOOT_COMPLETED))
        {
//            boolean isAuto = (boolean) SPUtils.get(context, Constant.SP_KEY_IS_AUTORUNIN, false);
//            LogUtil.i("aaaaaa system on  boot broadcast  isAuto : "+isAuto);
//            if(isAuto){
//                Intent startService = new Intent(context, RuninService.class);
//                startService.putExtra(Constant.INTENT_KEY_IS_AUTO, isAuto);
//                context.startService(startService);
//            }
        }
    }
}
