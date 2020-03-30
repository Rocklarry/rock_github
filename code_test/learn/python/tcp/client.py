#!/usr/bin/python
# -*- coding: UTF-8 -*-
# 文件名：client.py

import socket               # 导入 socket 模块

s = socket.socket()         # 创建 socket 对象
host = socket.gethostname() # 获取本地主机名
port = 12345                # 设置端口好
s.connect((host, port))

#for i in range(1,10):
i=1
while True: 
	
	i+=1
	print "i=",i
	print s.recv(1024)
s.close()  