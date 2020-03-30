package com.a3nod.lenovo.sparrowfactory.runin.emmc;

import com.a3nod.lenovo.sparrowfactory.Constant;
import com.a3nod.lenovo.sparrowfactory.tool.FileUtils;

import java.io.File;
import java.util.ArrayList;
import java.util.List;

/**
 * Description
 * Created by aaa on 2018/1/24.
 */
class EMMCTestRunnable implements Runnable
{
    EMMCTestActivity.EMMCHandler handler;
    private static final String FILE_DIR = Constant.TEST_LOG_PATH;
    private static final String FILE_NAME = "emmc.txt";
    private static final int FILL_DATA_SIZE = 0x80;
    private static final int EMMC_TEST_START = 10;

    public EMMCTestRunnable(EMMCTestActivity.EMMCHandler handler)
    {
        this.handler = handler;
        File file = new File(FILE_DIR);

        if (!file.exists() || !file.isDirectory())
        {
            file.mkdirs();
        }
    }

    @Override
    public void run()
    {
        //获取外部存储卡状态
        boolean mStorageState = FileUtils.checkExternalStorageState();
        if (mStorageState)
        {
            File file = new File(FILE_DIR + FILE_NAME);
            if (file.exists())
            {
                file.delete();
            }
        } else
        {
            handler.obtainMessage(EMMC_TEST_START, false).sendToTarget();
            return;
        }

        List<int[]> mFillData = new ArrayList<>();
        List<int[]> mReadData = new ArrayList<>();
        FileUtils.createFile(FILE_DIR + FILE_NAME);

        for (int i = 0; i < 11; i++)
        {
            int dataSize = FILL_DATA_SIZE * (int) Math.pow(2, i);
            int[] fillData = new int[dataSize];
            int[] readData = new int[dataSize];
            for (int j = 0; j < dataSize; j++)
            {
                fillData[j] = j;
            }
            mFillData.add(fillData);
            mReadData.add(readData);
        }

        boolean isSuccess = FileUtils.write2File(FILE_DIR + FILE_NAME, mFillData);
        isSuccess = isSuccess && FileUtils.readAndCompareData(FILE_DIR + FILE_NAME, mReadData, mFillData);
        mFillData.clear();
        mFillData = null;
        mReadData.clear();
        mReadData = null;
        handler.obtainMessage(EMMC_TEST_START, isSuccess).sendToTarget();
    }
}
