package com.a3nod.lenovo.sparrowfactory;

import android.content.BroadcastReceiver;
import android.content.Context;
import android.content.Intent;
import android.support.v4.content.LocalBroadcastManager;
import android.text.TextUtils;
import android.widget.Toast;

import com.a3nod.lenovo.sparrowfactory.runin.RuninService;
import com.a3nod.lenovo.sparrowfactory.tool.PcbaEntity;
import com.a3nod.lenovo.sparrowfactory.tool.SystemInfoTools;
import com.a3nod.lenovo.sparrowfactory.tool.LogUtil;

import java.io.File;

/**
 * Description
 * Created by aaa on 2017/12/13.
 */

public class PCMessageReceiver extends BroadcastReceiver
{
    @Override
    public void onReceive(Context context, Intent intent)
    {
        String action=intent.getAction();
        LogUtil.i("PCMessageReceiver action : "+action);
        if(Constant.ACTION_OPEN_CAMERA.equals(action))
        {
            File file=new File(Constant.CAMERA_DIR);
            if(!file.exists()||!file.isDirectory())
            {
                file.mkdirs();
            }
            Intent getPhotoIntent=new Intent(context, GetPhotoActivity.class);
            context.startActivity(getPhotoIntent);
        }else if(Constant.ACTION_START_PCBA.equals(action)){
            String serial=intent.getStringExtra("sn");
            SystemInfoTools.writeSNFile(serial,Constant.PATH_PCBA_SN);
/*            LogUtil.i("start pcba test");
            Toast.makeText(context,"start pcba test",Toast.LENGTH_SHORT).show();

            String flag=intent.getStringExtra("test_item");

            PcbaEntity.getInstants().cleanEntity();

            TestProcessManager.getInstance().readTestItems(context,flag);
            TestProcessManager.getInstance().toNextItem();

//            String serial_number=intent.getStringExtra("serial_number");
            PcbaEntity.getInstants().SN=SystemInfoTools.readSNFile(Constant.PATH_PCBA_SN);
//            SystemInfoTools.writeSNFile(serial_number);*/
            LogUtil.i("pcbac sn : "+SystemInfoTools.readSNFile(Constant.PATH_PCBA_SN));
        }else if(Constant.ACTION_WRITE_MMI_SN.equals(action)){
            String serial=intent.getStringExtra("sn");
            SystemInfoTools.writeSNFile(serial,Constant.PATH_MMI_SN);
            LogUtil.i("pcbac sn : "+SystemInfoTools.readSNFile(Constant.PATH_MMI_SN));
            Intent showSN=new Intent(context, ShowSNActivity.class);
            showSN.putExtra(Constant.INTENT_KEY_ACTION_SHOW_SN,action);
            context.startActivity(showSN);
        }else if(Constant.ACTION_WRITE_SN.equals(action)){
            String serial=intent.getStringExtra("sn");
            SystemInfoTools.writeSNFile(serial,Constant.PATH_SN);
            LogUtil.i("pcbac sn : "+SystemInfoTools.readSNFile(Constant.PATH_SN));
            Intent showSN=new Intent(context, ShowSNActivity.class);
            showSN.putExtra(Constant.INTENT_KEY_ACTION_SHOW_SN,action);
            context.startActivity(showSN);
        }else if(Constant.ACTION_START_RUNIN.equals(action)){
            Intent startRuninIntent = new Intent(context, RuninService.class);
            long duration = intent.getLongExtra(Constant.INTENT_KEY_RUNIN_DURATION,-1);
            String test_item = intent.getStringExtra(Constant.INTENT_KEY_RUNIN_TEST_ITEM);
            LogUtil.i("duration : "+duration +" test item : "+test_item);
            if(duration<=0|| TextUtils.isEmpty(test_item)){
                return;
            }else{
                startRuninIntent.putExtra(Constant.INTENT_KEY_START_AUTO_RUNIN,true);
                startRuninIntent.putExtra(Constant.INTENT_KEY_RUNIN_DURATION, duration);
                startRuninIntent.putExtra(Constant.INTENT_KEY_RUNIN_TEST_ITEM, test_item);
                context.startService(startRuninIntent);
            }
        }else if(Constant.ACTION_STOP_RUNIN.equals(action)){
            LogUtil.i("broadcast receive cmd stop runin");
            Intent stopRuninIntent = new Intent(Constant.INTENT_ACTION_STOP_RUNIN);
            LocalBroadcastManager.getInstance(context).sendBroadcast(stopRuninIntent);
        }


    }
}
