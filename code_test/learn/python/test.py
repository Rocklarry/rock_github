#!/usr/bin/python 
# -- coding: utf-8 --
import sys

print("Hello, World!");

print sys.platform
print(2*100)
x='Spam!'
print(x * 8)

print "test",x

# 带参数运行
'''
a = sys.argv[0]
b = sys.argv[1]

print "munber:",a ,b

if 
else
'''
flag = False
name = "python"
if name =="python":
    flag = True
    print "welcome boos"
else:
    print name

print ''
print '三角函数'

# 三角函数
import math

print "acos(0.64) : ",  math.acos(0.64)
print "acos(0) : ",  math.acos(0)
print "acos(-1) : ",  math.acos(-1)
print "acos(1) : ",  math.acos(1)

print ''
print ''

print "asin(0.64) : ",  math.asin(0.64)
print "asin(0) : ",  math.asin(0)
print "asin(-1) : ",  math.asin(-1)
print "asin(1) : ",  math.asin(1)

print ''
print 'Python 字典(Dictionary)'

#Python 字典(Dictionary)
dict = {'Name': 'Zara', 'Age': 7, 'Class': 'First'};
 
dict['Age'] = 8; # update existing entry
dict['School'] = "DPS School"; # Add new entry
 
 
print "dict['Age']: ", dict['Age'];
print "dict['School']: ", dict['School'];
'''
str = raw_input('请输入:');
print "你输入的是:",str
'''

#文件读写
print ''
print''
print "文件读写"
fo = open("foo.txt","wb")
print "文件名",fo.name
print "是否关闭",fo.closed
print "访问模式",fo.mode
print "末尾是否强制加空格",fo.softspace

fo.write("www.runoob.com!\nVery goog site!\n");

if fo.closed == False:
    fo.close()
print "是否关闭",fo.closed


fo = open("foo.txt","r+")
str = fo.read(10);
print"::",str

fo.close()

print '==========='
print '==========='
print ''
print ''
print" 文件定位"

fo = open("foo.txt","r+")
str = fo.read(10);
print"读取字符串是:",str

print''
print'查找当前位置'
position = fo.tell()
print'当前文件位置',position

print ''
print ''
print '指针再次重新定位到文件开头'

position = fo.seek(0,0)
str = fo.read(10)
print "重新读取字符串:",str

fo.close()

print ''
print ''
print "异常处理"

print '==========='
try:
    fh = open("testfile","w")
    fh.write("测试异常")
except IOError:
    print "error 没有找到文件"
else:
    print"写入内容成功"
    fh.close()



print ''
print ''
print '=================================================='
print ''
print '多线程'
print ''
print '=================================================='
print ''
print ''


import thread
import time

# 为线程定义一个函数
def print_time(threadName,delay):
	count = 0
	while count < 3:
		time.sleep(delay)
		count +=1
		print "%s: %s" % ( threadName, time.ctime(time.time()) )

# 创建两个线程
try:
   thread.start_new_thread( print_time, ("Thread-1", 1, ) )
   thread.start_new_thread( print_time, ("Thread-2", 2, ) )
except:
   print "Error: unable to start thread"
 
while 1:
   pass

