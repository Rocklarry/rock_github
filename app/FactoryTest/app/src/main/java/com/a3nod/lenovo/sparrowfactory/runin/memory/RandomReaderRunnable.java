package com.a3nod.lenovo.sparrowfactory.runin.memory;

import com.a3nod.lenovo.sparrowfactory.runin.RuninBaseActivity;
import com.a3nod.lenovo.sparrowfactory.tool.LogUtil;

import java.nio.ByteBuffer;
import java.util.ArrayList;
import java.util.List;
import java.util.Random;

/**
 * 内存随机读取测试
 */
class RandomReaderRunnable implements Runnable
{

    MemoryHandler handler;

    public RandomReaderRunnable(MemoryHandler handler)
    {
        this.handler = handler;
    }

    @Override
    public void run()
    {
        formerTest();
        //        allocateVM();
        //        allocateLocal();
        handler.sendEmptyMessageDelayed(RuninBaseActivity.MSG_START_TEST, 10);
        handler = null;
    }

    public void formerTest()
    {
        Random r = new Random();
        byte[] bytes;
        bytes = new byte[1024 * 1024 * 1];
        for (int i = 0; i < bytes.length; i++)
        {
            bytes[i] = (byte) r.nextInt(256);
        }
    }

    public void allocateVM()
    {
        List<byte[]> lb = new ArrayList<>();
        byte[] bytes;
        long totalMem = Runtime.getRuntime().totalMemory();
        //显示JVM尝试使用的最大内存
        long maxMem = Runtime.getRuntime().maxMemory();
        //空闲内存
        long freeMem = Runtime.getRuntime().freeMemory();
        LogUtil.i("total memory : " + totalMem + "  requere max memory : " + maxMem + "   free memory " + freeMem);
        for (int j = 0; j < maxMem / 1000000 * 4 / 5; j++)
        {
            bytes = new byte[1024 * 1024 * 1];
            lb.add(bytes);
            for (int i = 0; i < bytes.length; i++)
            {
                bytes[i] = (byte) i;
            }
            LogUtil.i("add byte  " + j);
        }
    }

    public void allocateLocal()
    {
        List<ByteBuffer> lbb = new ArrayList<>();
        for (int i = 0; i < 120; i++)
        {
            LogUtil.i("allocate local   " + i);
            ByteBuffer bb = ByteBuffer.allocateDirect(1024 * 1024);
            lbb.add(bb);
        }
        return;
    }
}
