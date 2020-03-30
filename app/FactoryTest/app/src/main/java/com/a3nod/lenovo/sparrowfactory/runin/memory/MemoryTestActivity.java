package com.a3nod.lenovo.sparrowfactory.runin.memory;

import android.os.Bundle;
import android.widget.TextView;

import com.a3nod.lenovo.sparrowfactory.Constant;
import com.a3nod.lenovo.sparrowfactory.CrashHandler;
import com.a3nod.lenovo.sparrowfactory.R;
import com.a3nod.lenovo.sparrowfactory.runin.RuninBaseActivity;
import com.a3nod.lenovo.sparrowfactory.runin.RuninConfig;
import com.a3nod.lenovo.sparrowfactory.tool.LogUtil;
import com.a3nod.lenovo.sparrowfactory.tool.SPUtils;
import com.a3nod.lenovo.sparrowfactory.tool.ThreadPoolUtils;

public class MemoryTestActivity extends RuninBaseActivity
{
    public static final String TAG = "MemoryTestActivity";

    public static final int MSG_SHOW_TEXT = 21;
    private MemoryHandler handler;
    private TextView tv_status;
    private long endTime;

    @Override
    protected void onCreate(Bundle savedInstanceState)
    {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_runin_memory);

        tv_status = findViewById(R.id.tv_memory_status);

        CrashHandler.getInstance().setCurrentTag(TAG, CrashHandler.TYPE_RUNIN);
        testTime = config.getItemDuration(RuninConfig.RUNIN_MEMORY_ID);
        endTime = testTime + System.currentTimeMillis();
        LogUtil.i("test time : " + testTime);

        handler = new MemoryHandler(this);
        if (isErrorRestart)
        {
            String errInfo = getIntent().getStringExtra(Constant.INTENT_KEY_ERROR_MSG);
            SPUtils.put(this, Constant.SP_KEY_MEMORY_TEST_REMARK, errInfo);
            mTestSuccess = false;
            tv_status.setText(R.string.running_memory_test);
            handler.sendEmptyMessage(MSG_TEST_FINISH);
        } else
        {
            handler.sendEmptyMessage(MSG_START_TEST);
            /*for(int i=0;i<1;i++){

                handler.sendEmptyMessage(MSG_START_TEST);
            }*/
            handler.sendEmptyMessage(MSG_SHOW_TEXT);
        }
    }


    public void memoryTest(MemoryHandler handler){
        if (System.currentTimeMillis() < endTime && !interrupt)
        {
                ThreadPoolUtils.executeSingleThread(new RandomReaderRunnable(handler));
//                ThreadPoolUtils.executeMultiThread(new RandomReaderRunnable(handler));
        }else{
            mTestSuccess = true;
            handler.sendEmptyMessage(MSG_TEST_FINISH);
        }
    }
    int i = 0;
    String[] text = {"Memory reading/writing.", "Memory reading/writing..", "Memory reading/writing..."};
    public void setStatue()
    {
        i++;
        tv_status.setText(text[i % 3]);
    }

    public void testFinish()
    {
        if (mTestSuccess)
        {
            config.saveTestResult(Constant.SP_KEY_MEMORY_TEST_SUCCESS);
        } else
        {
            config.saveTestResult(Constant.SP_KEY_MEMORY_TEST_FAIL);
        }
        if (isAutoRunin)
        {
            // to  next test
            toNextText();
            finish();
        } else
        {
            tv_status.setText("memory test" + (mTestSuccess ? "success" : "fail"));
            //test finish , exit?
        }

    }

    /*public void testMemory()
    {
        System.out.println("开始测试内存性能……");
        int i = (int)(1048576.0D * 1.5D);
        ByteArrayOutputStream localByteArrayOutputStream = new ByteArrayOutputStream();
        byte[] arrayOfByte1 = new byte[1024];
        double d = 0.0D;
        long l1 = System.currentTimeMillis();
        while (true)
        {
            long l2;
            byte[] arrayOfByte2;
            if (d == i)
            {
                l2 = System.currentTimeMillis() - l1;
                arrayOfByte2 = localByteArrayOutputStream.toByteArray();
            }
            try
            {
                localByteArrayOutputStream.close();
                localByteArrayOutputStream.reset();
                byte[] arrayOfByte3 = new byte[arrayOfByte2.length];
                long l3 = System.currentTimeMillis();
                int j = 0;
                int k = arrayOfByte2.length;
                if (j >= k)
                {
                    return;
                    localByteArrayOutputStream.write(arrayOfByte1, 0, 1024);
                    d += 1024.0D;
                    Message localMessage1 = new Message();
                    localMessage1.getData().putInt("progress", (int)(50.0D * (d / i)));
                    localMessage1.what = 1;
                    this.handler.sendMessage(localMessage1);
                }
            }
            catch (IOException localIOException)
            {
                while (true)
                {
                    byte[] arrayOfByte3;
                    long l3;
                    int j;
                    localIOException.printStackTrace();
                    continue;
                    arrayOfByte3[j] = arrayOfByte2[j];
                    if ((j + 1) % 1024 == 0)
                    {
                        Message localMessage2 = new Message();
                        int m = 50 + (int)(50.0D * ((1.0D + j) / arrayOfByte2.length));
                        localMessage2.getData().putInt("progress", m);
                        if (m == 100)
                            localMessage2.getData().putInt("result", getMemoryScore((l2 + (System.currentTimeMillis() - l3)) / 1000.0D));
                        localMessage2.what = 1;
                        this.handler.sendMessage(localMessage2);
                    }
                    j++;
                }
            }
        }
    }*/
    private int getMemoryScore(double paramDouble)
    {
        return (int) (250.0D / paramDouble);
    }

    @Override
    protected void onPause()
    {
        super.onPause();
        handler.removeCallbacksAndMessages(null);
    }

    @Override
    protected void onDestroy()
    {
        super.onDestroy();
    }
}
