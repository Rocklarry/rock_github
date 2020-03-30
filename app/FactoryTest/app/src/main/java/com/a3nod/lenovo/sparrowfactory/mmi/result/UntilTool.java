package com.a3nod.lenovo.sparrowfactory.mmi.result;

import android.os.Environment;

import com.google.gson.Gson;

import java.io.File;
import java.io.FileOutputStream;

/**
 * Created by Lenovo on 2017/11/22.
 */

public class UntilTool {
    static void save(String name) throws Exception {
        Gson gson=new Gson();
        String mContent=gson.toJson(ASSYEntity.getInstants());
        File file=new File(getSDPath() +"/" + name);
        //Context.MODE_PRIVATE权限，只有自身程序才能访问，而且写入的内容会覆盖文本内原有内容
//        FileOutputStream output = openFileOutput(name, Context.MODE_PRIVATE);
        FileOutputStream output =new FileOutputStream(file);
        output.write(mContent.getBytes());  //将String字符串以字节流的形式写入到输出流中
        output.close();         //关闭输出流
    }

    static String getSDPath(){
        File sdDir = null;
        boolean sdCardExist = Environment.getExternalStorageState()
                .equals(android.os.Environment.MEDIA_MOUNTED);//判断sd卡是否存在
        if(sdCardExist)
        {
            sdDir = Environment.getExternalStorageDirectory();//获取跟目录
        }
        return sdDir.toString();
    }
}
