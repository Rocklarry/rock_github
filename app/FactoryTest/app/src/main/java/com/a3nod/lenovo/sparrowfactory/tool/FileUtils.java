package com.a3nod.lenovo.sparrowfactory.tool;

import android.os.Build;
import android.os.Environment;

import com.a3nod.lenovo.sparrowfactory.Constant;
import com.a3nod.lenovo.sparrowfactory.interact.Protocol;
import com.a3nod.lenovo.sparrowfactory.runin.TestItem;

import org.json.JSONException;
import org.json.JSONObject;

import java.io.BufferedInputStream;
import java.io.BufferedOutputStream;
import java.io.BufferedReader;
import java.io.BufferedWriter;
import java.io.DataInputStream;
import java.io.DataOutputStream;
import java.io.File;
import java.io.FileInputStream;
import java.io.FileNotFoundException;
import java.io.FileOutputStream;
import java.io.FileReader;
import java.io.FileWriter;
import java.io.IOException;
import java.util.List;

/**
 * Description
 * Created by aaa on 2017/11/15.
 */

public class FileUtils
{
    public static void createFile(String path)
    {
        File dir = new File(path);

        if (dir.exists())
        {
            dir.delete();
        }
        if (!dir.exists())
        {
            try
            {
                dir.createNewFile();
            } catch (Exception e)
            {
                e.printStackTrace();
            }
        }
    }

    public static boolean write2File(String outputPath, List<int[]> mFillData)
    {
        DataOutputStream out = null;
        try
        {
            out = new DataOutputStream(new BufferedOutputStream(new FileOutputStream(outputPath, true)));
            for (int i = 0; i < mFillData.size(); i++)
            {
                for (int j = 0; j < mFillData.get(i).length; j++)
                {
                    out.writeInt(mFillData.get(i)[j]);
                }
            }
        } catch (FileNotFoundException fnf)
        {
            fnf.printStackTrace();
            return false;
        } catch (IOException e)
        {
            e.printStackTrace();
            return false;
        } finally
        {
            try
            {
                if (null != out)
                {
                    out.flush();
                    out.close();
                }
            } catch (IOException e)
            {
                e.printStackTrace();
            }
        }
        return true;
    }

    public static boolean readAndCompareData(String inputPath, List<int[]> mReadData, List<int[]> mFillData)
    {
        DataInputStream input = null;
        try
        {
            input = new DataInputStream(new BufferedInputStream(new FileInputStream(inputPath)));
            for (int i = 0; i < mReadData.size(); i++)
            {
                for (int j = 0; j < mReadData.get(i).length; j++)
                {
                    mReadData.get(i)[j] = input.readInt();
                }
            }

        } catch (FileNotFoundException fnf)
        {
            fnf.printStackTrace();
            return false;
        } catch (IOException e)
        {
            e.printStackTrace();
            return false;
        } finally
        {
            try
            {
                if (null != input)
                {
                    input.close();
                }
            } catch (IOException e)
            {
                e.printStackTrace();
                e.printStackTrace();
            }
        }

        for (int i = 0; i < mReadData.size(); i++)
        {
            for (int j = 0; j < mReadData.get(i).length; j++)
            {
                if (mReadData.get(i)[j] != mFillData.get(i)[j])
                {
                    LogUtil.i(" data not equals");
                    return false;
                }
            }
        }
        return true;
    }

    /**
     * 检测 外部存储卡是否可用
     *
     * @return
     */
    public static boolean checkExternalStorageState()
    {
        String extStorageState = Environment.getExternalStorageState();
        if (extStorageState.equals(Environment.MEDIA_MOUNTED))
        {
            return true;
        }
        return false;
    }

    public static void createReportFile(List<TestItem> list, int failItem, long runintime, long startTime, long endTime)
    {
        JSONObject root = new JSONObject();
        JSONObject item;
        try
        {
            root.put("msg_type", Protocol.MSG_RUNIN_REPORT);
            root.put("sn", Build.SERIAL);
            root.put("fail_item", failItem);
            if (endTime == -1)
            {
                root.put("actual_duration", System.currentTimeMillis() - startTime);
            } else
            {
                root.put("actual_duration", endTime - startTime);
            }
            root.put("runin_duration", runintime);
            for (TestItem ti : list)
            {

                item = new JSONObject();
                item.put("success", ti.getSuccess());
                item.put("fail", ti.getFail());
                root.put(ti.getName(), item);

            }
        } catch (JSONException e)
        {
            e.printStackTrace();
        }
        writeReport(root.toString());
    }

    public static void writeReport(String report)
    {
        File file = new File(Constant.RUNIN_REPORT_PATH);
        if (file.exists())
        {
            file.delete();
        }
        try
        {
            FileWriter fw = new FileWriter(file);
            BufferedWriter bw = new BufferedWriter(fw);
            bw.write(report);
            bw.close();
            fw.close();
        } catch (IOException e)
        {
            e.printStackTrace();
        }
    }

    public static String readReport()
    {
        File file = new File(Constant.RUNIN_REPORT_PATH);
        if (file.isFile() && file.exists())
        {
            try
            {
                FileReader fr=new FileReader(file);
                BufferedReader br=new BufferedReader(fr);
                String report =br.readLine();
                br.close();
                fr.close();
                return report;
            } catch (FileNotFoundException e)
            {
                e.printStackTrace();
            } catch (IOException e)
            {
                e.printStackTrace();
            }
        }
        return null;
    }

}
