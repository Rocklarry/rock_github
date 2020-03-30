package com.a3nod.lenovo.sparrowfactory.interact;

import android.content.Context;
import android.content.Intent;
import android.os.Build;
import android.support.v4.content.LocalBroadcastManager;
import android.text.TextUtils;
import android.util.Log;

import com.a3nod.lenovo.sparrowfactory.Constant;
import com.a3nod.lenovo.sparrowfactory.GetPhotoActivity;
import com.a3nod.lenovo.sparrowfactory.SparrowApplication;
import com.a3nod.lenovo.sparrowfactory.runin.RuninService;
import com.a3nod.lenovo.sparrowfactory.tool.FileUtils;
import com.a3nod.lenovo.sparrowfactory.tool.LogUtil;
import com.a3nod.lenovo.sparrowfactory.tool.PcbaEntity;
import com.a3nod.lenovo.sparrowfactory.tool.ThreadPoolUtils;

import org.json.JSONException;
import org.json.JSONObject;

import java.io.BufferedReader;
import java.io.BufferedWriter;
import java.io.IOException;
import java.io.InputStreamReader;
import java.io.OutputStreamWriter;
import java.net.ServerSocket;
import java.net.Socket;

/**
 * Description
 * Created by aaa on 2018/3/26.
 */

public class ConnectManager
{
    private ServerSocket serverSocket = null;
    private final int SERVER_PORT = 10086;
    private static Socket client;
    private static volatile Boolean mainThreadFlag = false;
    private static volatile Boolean ioThreadFlag = false;
    private LocalBroadcastManager lbm;
    private BufferedWriter buffout;
    Context context;
    private String[] action = new String[]{
            "pcba_lcd",
            "pcba_tp",
     //       "pcba_camera",
            "pcba_mic",
            "pcba_speaker",
    //        "hello.PCBAGSensorActivity",
            "hello.PCBAPSensorActivity",
            "hello.PCBALSensorActivity",
            "pcba_button",
            "hello.BluetoothActivity",
            "wifi.action",
            "pcba.datacheck.action",
            "pcbargb.sensor.action"
    };

    private ConnectManager()
    {
        context = SparrowApplication.getInstance().getApplicationContext();
        lbm = LocalBroadcastManager.getInstance(context);
    }

    static class SocketHolder
    {
        private static ConnectManager instance = new ConnectManager();
    }

    public static ConnectManager getInstance()
    {
        return ConnectManager.SocketHolder.instance;
    }

    public void startServer()
    {
        if (!mainThreadFlag)
        {
            mainThreadFlag = true;
            ThreadPoolUtils.executeServerSocketThread(new Runnable()
            {
                public void run()
                {
                    Thread.currentThread().setName("Socket server thread");
                    doListen();
                }
            });
        }
    }

    void doListen()
    {
        mainThreadFlag = true;
        LogUtil.i("SocketServer : dolisten ");
        serverSocket = null;
        boolean openServer = false;

        //打开socket服务器
        while (!openServer)
        {
            try
            {
                serverSocket = new ServerSocket(SERVER_PORT);
                openServer = true;
            } catch (IOException e)
            {
                e.printStackTrace();
            }
        }

        //循环监听客户端连接
        while (mainThreadFlag)
        {
            try
            {
                client = serverSocket.accept();
                buffout = new BufferedWriter(new OutputStreamWriter(client.getOutputStream()));
                LogUtil.i(" client connected");
                ThreadPoolUtils.executeClientSocketThread(new ThreadReadWriterIOSocket(client));
            } catch (IOException e)
            {
                e.printStackTrace();
            }
        }


    }

    /**
     * Description
     * Created by aaa on 2018/1/10.
     */

    public class ThreadReadWriterIOSocket implements Runnable
    {

        private Socket client;

        public ThreadReadWriterIOSocket(Socket client)
        {
            this.client = client;
            PcbaEntity.getInstants().cleanEntity();
        }

        @Override
        public void run()
        {
            Thread.currentThread().setName("SocketClient");
            try
            {
                BufferedReader in = new BufferedReader(new InputStreamReader(client.getInputStream()));
                ioThreadFlag = true;
                while (ioThreadFlag)
                {
                    try
                    {
                        /**
                         * Socket类还有一个isConnected方法来判断Socket对象是否连接成功。看到这个名字，也许读者会产生误解。
                         * 其实isConnected方法所判断的并不是Socket对象的当前连接状态，而是Socket对象是否曾经连接成功过，
                         * 如果成功连接过，即使现在isClose返回true，isConnected仍然返回true。
                         * isClosed也不完全保险 , 直接new Socket() isclosed=false ,但是并未连接到任何服务器
                         * 所以同时用两个
                         */
                        if (!client.isConnected() || client.isClosed())
                        {
                            LogUtil.i(" socket disconnected");
                            break;
                        }
                        String str = in.readLine();

                        if (str == null)
                        {
                            //客户端断开连接
                            LogUtil.i("client close");
                            break;
                        } else
                        {
                            //收到信息
                            JSONObject json = null;
                            int msgType = -1;
                            try
                            {
                                json = new JSONObject(str);
                                msgType = json.getInt("msg_type");

                                switch (msgType)
                                {
                                    case Protocol.MSG_START_TEST:
                                        int item = json.getInt("test_item");
                                        startPCBATest(item);
                                        break;
                                    case Protocol.MSG_STOP_TEST:
                                        ioThreadFlag = false;
                                        //TODO 需要添加写日志功能
                                        PcbaEntity.saveResult();
                                        break;
                                    case Protocol.MSG_CMD_SWITCH_MIC:
                                        int status = json.optInt("status");
                                        switchMic(status == 1);
                                        break;
                                    case Protocol.MSG_CAMETA_TEST:
                                        int Camerastatus = json.optInt("cmd");
                                        //                                        LogUtil.i( "receive string  20: " + Camerastatus);
                                        if (Camerastatus == 0)
                                        {
                                            Intent getPhotoIntent = new Intent(context, GetPhotoActivity.class);
                                            context.startActivity(getPhotoIntent);
                                        } else if (Camerastatus == 3)
                                        {
                                            Intent intent = new Intent(Constant.ACTION_TAKE_PICTURE);
                                            context.sendBroadcast(intent);
                                        }
                                        break;
                                    case Protocol.MSG_START_RUNIN:
                                        Intent startRuninIntent = new Intent(context, RuninService.class);
                                        long duration = json.optLong("runin_duration");
                                        String test_item = json.optString("runin_items");
                                        if (duration <= 0 || TextUtils.isEmpty(test_item))
                                        {
                                            return;
                                        } else
                                        {
                                            startRuninIntent.putExtra(Constant.INTENT_KEY_START_AUTO_RUNIN, true);
                                            startRuninIntent.putExtra(Constant.INTENT_KEY_RUNIN_DURATION, duration);
                                            startRuninIntent.putExtra(Constant.INTENT_KEY_RUNIN_TEST_ITEM, test_item);
                                            context.startService(startRuninIntent);
                                        }
                                        break;
                                    case Protocol.MSG_RUNIN_GET_STATUS:
                                        String report = FileUtils.readReport();
                                        setRuninResult(report);
                                        break;
                                    case Protocol.MSG_STOP_RUNIN:
                                        LogUtil.i("sockete receive cmd stop runin");
                                        Intent stopRuninIntent = new Intent(Constant.INTENT_ACTION_STOP_RUNIN);
                                        lbm.sendBroadcast(stopRuninIntent);
                                        break;
                                    default:
                                        //                                    handler.obtainMessage(MSG_RECEIVE_COMMAD, "小胖子 没这条命令").sendToTarget();
                                        break;
                                }
                            } catch (JSONException e)
                            {
                                e.printStackTrace();
                                continue;
                            }
                            LogUtil.i("receive string  : " + str);
                        }
                    } catch (Exception e)
                    {
                        e.printStackTrace();
                        break;
                    }
                }
                LogUtil.i(" in close");
                in.close();
            } catch (Exception e)
            {
                // TODO: handle exception
                e.printStackTrace();
            } finally
            {
                try
                {
                    if (client != null)
                    {
                        LogUtil.i(" server close socket");
                        client.close();
                    }
                } catch (IOException e)
                {
                    e.printStackTrace();
                }
            }
        }
    }

    public void startPCBATest(int i)
    {

        if (i >= 0 && i <= 12)
        {
            LogUtil.i("start i= " + i + " action : " + action[i]);
            Intent intent = new Intent(action[i]);
            intent.putExtra(Constant.INTENT_KEY_TEST_MODEL, Constant.INTENT_VALUE_TEST_MODEL_PCBA);
            context.startActivity(intent);
        }
    }

    public void switchMic(boolean isHigh)
    {
        Intent intent = new Intent(Constant.INTENT_ACTION_SWITCH_MIC);
        intent.putExtra(Constant.INTENT_KEY_MIC_GROUP, isHigh);
        lbm.sendBroadcast(intent);
    }

    class ResultRunnable implements Runnable
    {
        String result;

        public ResultRunnable(String result)
        {
            this.result = result;
        }

        @Override
        public void run()
        {
            Thread.currentThread().setName("MessageToPC");
            try
            {

                if (!result.endsWith("\n"))
                {
                    result = result + "\n";
                }
                buffout.write(result);
                buffout.flush();

            } catch (IOException e)
            {
                e.printStackTrace();
            }
        }

    }

    public void setPCBAResult(int item, boolean result, String remark)
    {
        PcbaEntity.getInstants().setResult(item, result);
        //TODO 这里要判断失败 然后停止测试
        try
        {
            JSONObject resultJson = new JSONObject();
            resultJson.put("msg_type", Protocol.MSG_TEST_RESULT);
            resultJson.put("test_item", item);
            resultJson.put("test_result", result ? 0 : 1);
            resultJson.put("remark", TextUtils.isEmpty(remark) ? "" : remark);

            if (client != null && client.isConnected() && !client.isClosed())
            {
                ThreadPoolUtils.executeClientSocketThread(new ResultRunnable(resultJson.toString()));
            }
        } catch (JSONException e)
        {
            e.printStackTrace();
        }
    }

    public void setRGBSensorResult(int item, boolean result, String remark,int lightness,float colorTemp)
    {
        PcbaEntity.getInstants().setResult(item, result);
        //TODO 这里要判断失败 然后停止测试
        try
        {
            JSONObject resultJson = new JSONObject();
            resultJson.put("msg_type", Protocol.MSG_TEST_RESULT);
            resultJson.put("test_item", item);
            resultJson.put("test_result", result ? 0 : 1);
            resultJson.put("remark", TextUtils.isEmpty(remark) ? "" : remark);
            resultJson.put("lightness", lightness);
            resultJson.put("colorTemp", colorTemp);
            if (client != null && client.isConnected() && !client.isClosed())
            {
                ThreadPoolUtils.executeClientSocketThread(new ResultRunnable(resultJson.toString()));
            }
        } catch (JSONException e)
        {
            e.printStackTrace();
        }
    }

    /**
     * {
     * "msg_type":32,
     * "test_result":0|1,
     * "fail_item":5,
     * "runin_duration":100000,
     * "actual_duration":20000,
     * "remark":
     * {
     * "CPU":{"success":106,"fail":0},
     * "Memory":{"success":106,"fail":0},
     * "EMMC":{"success":106,"fail":0},
     * "LCD":{"success":106,"fail":0},
     * "2D":{"success":106,"fail":0},
     * "3D":{"success":106,"fail":0},
     * "Audio":{"success":106,"fail":0},
     * "Video":{"success":105,"fail":0},
     * "Camera":{"success":105,"fail":0},
     * "Reboot":{"success":105,"fail":0}
     * }
     * }
     *
     * @return isSendSuccess
     */
    public boolean setRuninResult(int failItem, long runin_duration, long actual_duration, String remark)
    {
        try
        {
            JSONObject resultJson = new JSONObject();
            resultJson.put("msg_type", Protocol.MSG_RUNIN_REPORT);
            resultJson.put("sn", Build.SERIAL);
            resultJson.put("fail_item", failItem);
            resultJson.put("runin_duration", runin_duration);
            resultJson.put("actual_duration", actual_duration);
            JSONObject report = new JSONObject(remark);
            resultJson.put("remark", report);

            if (client != null && client.isConnected() && !client.isClosed())
            {
                ThreadPoolUtils.executeClientSocketThread(new ResultRunnable(resultJson.toString()));
                return true;
            } else
            {
                return false;
            }
        } catch (JSONException e)
        {
            e.printStackTrace();
        }
        return false;
    }

    public boolean setRuninResult(String report)
    {
        if (client != null && client.isConnected() && !client.isClosed())
        {
            ThreadPoolUtils.executeClientSocketThread(new ResultRunnable(report));
            return true;
        } else
        {
            return false;
        }
    }


    public void setTakePhotoResult(int item, String remark)
    {
        try
        {
            JSONObject resultJson = new JSONObject();
            resultJson.put("msg_type", 20);
            resultJson.put("cmd", item);
            resultJson.put("remark", TextUtils.isEmpty(remark) ? "" : remark);

            if (client != null && client.isConnected() && !client.isClosed())
            {
                ThreadPoolUtils.executeClientSocketThread(new ResultRunnable(resultJson.toString()));
            }
        } catch (JSONException e)
        {
            e.printStackTrace();
        }
    }

    public void sendData(String data)
    {
        if (client != null && client.isConnected() && !client.isClosed())
        {
            ThreadPoolUtils.executeClientSocketThread(new ResultRunnable(data));
        }
    }

    public void stopServer()
    {
        LogUtil.d("connect service stop");
        mainThreadFlag = false;
        ioThreadFlag = false;
        try
        {
            if (serverSocket != null)
            {
                serverSocket.close();
            }
            if (buffout != null)
            {
                buffout.close();
            }
        } catch (IOException e)
        {
            e.printStackTrace();
        }
    }
}
