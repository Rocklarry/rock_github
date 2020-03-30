#!/usr/bin/env python
# -*- coding: utf-8 -*-

import socket 

def  baidu_test():
	target_host = "www.baidu.com"
	target_port = 80
 
	#建立一个socket对象
	client = socket.socket(socket.AF_INET,socket.SOCK_STREAM)
	#连接客户端
	client.connect((target_host,target_port))
	#发送一些数据
	client.send(("GET / HTTP/1.1\r\nHost: baidu.com\r\n\r\n").encode(encoding='utf_8', errors='strict'))
 
	#接收一些数据
	response = client.recv(4096)
	print (response)
















def host_test():
	target_host = "192.168.107.42"
	target_port = 8800

	#建立一个socket对象
	client = socket.socket(socket.AF_INET,socket.SOCK_DGRAM)

	#发送一些数据
	client.sendto(("This is an UDP client").encode(),(target_host,target_port))

	#接收一些数据
	data, addr = client.recvfrom(4096)
 
	print (data)
	print (addr)

#baidu_test()
host_test()