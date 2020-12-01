package com.example.myapplication.utils;

import java.util.ArrayList;
import java.util.List;

/**
 * ��������ת��������
 * Created by Administrator on 2016/6/2.
 */
public class DataUtils {
    //-------------------------------------------------------
    // �ж�������ż����λ���㣬���һλ��1��Ϊ������Ϊ0��ż��
    public static int isOdd(int num) {
        return num & 1;
    }

    //-------------------------------------------------------
    //Hex�ַ���תint
    public static int HexToInt(String inHex) {
        return Integer.parseInt(inHex, 16);
    }

    public static String IntToHex(int intHex){
        return Integer.toHexString(intHex);
    }

    //-------------------------------------------------------
    //Hex�ַ���תbyte
    public static byte HexToByte(String inHex) {
        return (byte) Integer.parseInt(inHex, 16);
    }

    //-------------------------------------------------------
    //1�ֽ�ת2��Hex�ַ�
    public static String Byte2Hex(Byte inByte) {
        return String.format("%02x", new Object[]{inByte}).toUpperCase();
    }

    //-------------------------------------------------------
    //�ֽ�����תתhex�ַ���
    public static String ByteArrToHex(byte[] inBytArr) {
        StringBuilder strBuilder = new StringBuilder();
        for (byte valueOf : inBytArr) {
            strBuilder.append(Byte2Hex(Byte.valueOf(valueOf)));
            strBuilder.append(" ");
        }
        return strBuilder.toString();
    }

    //-------------------------------------------------------
    //�ֽ�����תתhex�ַ�������ѡ����
    public static String ByteArrToHex(byte[] inBytArr, int offset, int byteCount) {
        StringBuilder strBuilder = new StringBuilder();
        int j = byteCount;
        for (int i = offset; i < j; i++) {
            strBuilder.append(Byte2Hex(Byte.valueOf(inBytArr[i])));
        }
        return strBuilder.toString();
    }

    //-------------------------------------------------------
    //תhex�ַ���ת�ֽ�����
    public static byte[] HexToByteArr(String inHex) {
        byte[] result;
        int hexlen = inHex.length();
        if (isOdd(hexlen) == 1) {
            hexlen++;
            result = new byte[(hexlen / 2)];
            inHex = "0" + inHex;
        } else {
            result = new byte[(hexlen / 2)];
        }
        int j = 0;
        for (int i = 0; i < hexlen; i += 2) {
            result[j] = HexToByte(inHex.substring(i, i + 2));
            j++;
        }
        return result;
    }

    /**
     * ����ָ�������и��ַ���
     *
     * @param inputString ��Ҫ�и��Դ�ַ���
     * @param length      ָ���ĳ���
     * @return
     */
    public static List<String> getDivLines(String inputString, int length) {
        List<String> divList = new ArrayList<>();
        int remainder = (inputString.length()) % length;
        // һ��Ҫ�ָ�ɼ���
        int number = (int) Math.floor((inputString.length()) / length);
        for (int index = 0; index < number; index++) {
            String childStr = inputString.substring(index * length, (index + 1) * length);
            divList.add(childStr);
        }
        if (remainder > 0) {
            String cStr = inputString.substring(number * length, inputString.length());
            divList.add(cStr);
        }
        return divList;
    }

    /**
     * ���㳤�ȣ������ֽڳ���
     *
     * @param val value
     * @return ���
     */
    public static String twoByte(String val) {
        if (val.length() > 4) {
            val = val.substring(0, 4);
        } else {
            int l = 4 - val.length();
            for (int i = 0; i < l; i++) {
                val = "0" + val;
            }
        }
        return val;
    }

    /**
     * У���
     *
     * @param cmd ָ��
     * @return ���
     */
    public static String sum(String cmd) {
        List<String> cmdList = DataUtils.getDivLines(cmd, 2);
        int sumInt = 0;
        for (String c : cmdList) {
            sumInt += DataUtils.HexToInt(c);
        }
        String sum = DataUtils.IntToHex(sumInt);
        sum = DataUtils.twoByte(sum);
        cmd += sum;
        return cmd.toUpperCase();
    }

}