package com.example.myapplication.utils;

import android.util.Log;

import org.greenrobot.eventbus.EventBus;

import java.io.File;
import java.io.IOException;
import java.io.InputStream;
import java.io.OutputStream;

import android_serialport_api.SerialPort;

/**
 * @author by AllenJ on 2018/4/20.
 *
 * ͨ���������ڽ��ջ�������
 */

public class SerialPortUtil {

    private SerialPort serialPort = null;
    private InputStream inputStream = null;
    private OutputStream outputStream = null;
    private ReceiveThread mReceiveThread = null;
    private boolean isStart = false;

    /**
     * �򿪴��ڣ���������
     * ͨ�����ڣ����յ�Ƭ��������������
     */
    public void openSerialPort() {
        try {
            serialPort = new SerialPort(new File("/dev/ttyS0"), 9600, 0);
            //serialPort = new SerialPort(new File(path), 9600, 0);
            //���ö���SerialPort��������ȡ������"����д"��������
            inputStream = serialPort.getInputStream();
            outputStream = serialPort.getOutputStream();
            isStart = true;

        } catch (IOException e) {
            e.printStackTrace();
        }
        getSerialPort();
    }

    /**
     * �رմ���
     * �رմ����е����������
     */
    public void closeSerialPort() {
        Log.i("test", "�رմ���");
        try {
            if (inputStream != null) {
                inputStream.close();
            }
            if (outputStream != null) {
                outputStream.close();
            }
            isStart = false;
        } catch (IOException e) {
            e.printStackTrace();
        }

    }

    /**
     * ��������
     * ͨ�����ڣ��������ݵ���Ƭ��
     *
     * @param data Ҫ���͵�����
     */
    public void sendSerialPort(String data) {
        try {
            byte[] sendData = DataUtils.HexToByteArr(data);
            outputStream.write(sendData);
            outputStream.flush();
        } catch (IOException e) {
            e.printStackTrace();
        }
    }

    private void getSerialPort() {
        if (mReceiveThread == null) {

            mReceiveThread = new ReceiveThread();
        }
        mReceiveThread.start();
    }

    /**
     * ���մ������ݵ��߳�
     */

    private class ReceiveThread extends Thread {
        @Override
        public void run() {
            super.run();
            //�����жϣ�ֻҪ����Ϊtrue����һֱִ������߳�
            while (isStart) {
                if (inputStream == null) {
                    return;
                }
                byte[] readData = new byte[1024];
                try {
                    int size = inputStream.read(readData);
                    if (size > 0) {
                        String readString = DataUtils.ByteArrToHex(readData, 0, size);
                        EventBus.getDefault().post(readString);
                    }

                } catch (IOException e) {
                    e.printStackTrace();
                }
            }

        }
    }

}