/*   
 * Copyright (c) 2013-2020 Founder Ltd. All Rights Reserved.   
 *   
 * This software is the confidential and proprietary information of   
 * Founder. You shall not disclose such Confidential Information   
 * and shall use it only in accordance with the terms of the agreements   
 * you entered into with Founder.   
 *   
 */

package com.a3nod.lenovo.sparrowfactory.mmi.wifi;

import com.a3nod.lenovo.sparrowfactory.tool.LogUtil;

import java.io.BufferedReader;
import java.io.ByteArrayOutputStream;
import java.io.InputStream;
import java.io.InputStreamReader;
import java.io.PrintWriter;
import java.net.HttpURLConnection;
import java.net.URL;


public class HttpUtil
{
	public static String requestGETWebService(String urlString, String params)
	{
		LogUtil.i("urlString : " + urlString + "?" + params);
		String result = "";
		// 网络地址 通过字符串，生成URL对象
		URL url = null;
		// 网络会话链接
		HttpURLConnection conn = null;
		try
		{
			url = new URL(urlString + "?" + params);
			LogUtil.d("url" + urlString + "?" + params);
			// 连接，从postUrl.openConnection()至此的配置必须要在connect之前完成，
			conn = (HttpURLConnection) url.openConnection();
			// 默认是get 方式
			conn.setRequestMethod("GET");
			// 设置是否向connection输出，如果是post请求，参数要放在http正文内，因此需要设为true
			// conn.setDoOutput(true);
			// Post 请求不能使用缓存
			conn.setUseCaches(false);
			// 设置请求头 一般没特殊要求， 不需要
			conn.setRequestProperty("Content-Type","application/x-www-form-urlencoded");

			// 要注意的是connection.getOutputStream会隐含的进行connect,所以conn.connect()这句可以不要
			// 要上传的参数

			// 时候需要获取输入， 废话，当然需要返回，最少要返回状态吧。 所以默认是true
			// conn.setDoInput(true);
			int code = conn.getResponseCode();
			LogUtil.d("response url : " + urlString + "?" + params);
			LogUtil.d("response code : " + code);
			if(conn.getResponseCode() == 200)
			{
				InputStreamReader is = new InputStreamReader(conn.getInputStream());

				BufferedReader br = new BufferedReader(is);
				String line = "";
				StringBuffer sb = new StringBuffer();
				while((line = br.readLine()) != null)
				{
					sb.append(line);
				}
				result = sb.toString();
				br.close();
				is.close();
			}
			conn.disconnect();
		} catch (Exception e)
		{
			e.printStackTrace();
		}
		LogUtil.i("urlString : " + urlString + " result:  " + result);
		return result;
	}

	public static String requestPOSTWebService(String urlString, String params)
	{
		LogUtil.i("urlString : " + urlString + " ? " + params);
		// password=getEncryptPassword(password);
		String result = "{}";
		// 网络地址 通过字符串，生成URL对象
		URL url = null;
		// 网络会话链接
		HttpURLConnection conn = null;
		// 获取网站返回的输入流
		InputStream in = null;
		// 每次读的字节数
		byte[] data = new byte[1024];
		// 每次读到的字节数，一般是1024，如果到了最后一行就会少于1024，到了末尾就是 -1
		int len = 0;
		// 本地的输出流
		ByteArrayOutputStream os;
		try
		{
			url = new URL(urlString);
			conn = (HttpURLConnection) url.openConnection();
			// 默认是get 方式
			conn.setRequestMethod("POST");
			// 设置是否向connection输出，如果是post请求，参数要放在http正文内，因此需要设为true
			conn.setDoOutput(true);
			// Post 请求不能使用缓存
			conn.setUseCaches(false);
			// 设置请求头 一般没特殊要求， 不需要
			conn.setRequestProperty("Content-Type","application/x-www-form-urlencoded");
			// 连接，从postUrl.openConnection()至此的配置必须要在connect之前完成，

			// 要注意的是connection.getOutputStream会隐含的进行connect,所以下面这句可以不要
			// conn.connect();
			// 要上传的参数
			PrintWriter pw = new PrintWriter(conn.getOutputStream());
			pw.print(params);
			pw.flush();
			pw.close();
			// 时候需要获取输入， 废话，当然需要返回，最少要返回状态吧。 所以默认是true
			// conn.setDoInput(true);
			System.out.println(conn.getResponseCode());
			if(conn.getResponseCode() == 200)
			{

				InputStreamReader is = new InputStreamReader(conn.getInputStream());

				BufferedReader br = new BufferedReader(is);
				String line = "";
				StringBuffer sb = new StringBuffer();
				while((line = br.readLine()) != null)
				{

					sb.append(line);
				}

				br.close();
				is.close();
				result = sb.toString();
			}
			conn.disconnect();
		} catch (Exception e)
		{
			e.printStackTrace();
		}
		LogUtil.i("urlString : " + urlString + " result:  " + result);
		return result;
	}



}
