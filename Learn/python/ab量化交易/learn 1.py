#!/usr/bin/env python
# -*- coding: utf-8 -*-

var1 = "python learn"
var2 = "Runoob"

print ("var1[0]",var1[0])
print ("var2[1:5]",var2[1:5])

print ("我叫 %s 今年 %d 岁!" % ('小明', 10))


class people:
	#定义基本属性
	name = ''
	age = ''
	# 定义私有属性 私有属性类外部无法访问
	_weight = 0
	#定义构造方法
	def __init__(self,n,a,w):
		self.name = n
		self.age = a
		self._weight = w
	def speak(self):
		print ("%s 说：我%d 岁."%(self.name,self.age))

#单继承示例
class student(people):
    grade = ''
    def __init__(self,n,a,w,g):
        #调用父类的构函
        people.__init__(self,n,a,w)
        self.grade = g
    #覆写父类的方法
    def speak(self):
        print("%s 说: 我 %d 岁了，我在读 %d 年级"%(self.name,self.age,self.grade))		



#另一个类，多重继承之前的准备
class speaker():
	topic = ''
	name = ''
	def __init__(self,n,t):
		self.name = n
		self.topic = t
	def speak(self):
		print("我叫 %s，我是一个演说家，我演讲的主题是 %s"%(self.name,self.topic))


class sample(speaker,student):
	a = ''
	def __init__(self,n,a,w,g,t):
		student.__init__(self,n,a,w,g)
		speaker.__init__(self,n,t)

test = sample("Tim",25,80,4,"Python")
test.speak()
test.speak()


s = student('ken',10,60,3)
s.speak()
s.speak()

p = people('runoob',10,30)
p.speak()
p.speak()



print ("")
print ("")

import re
print (re.match('www','www.runoob.com').span())

line = "cats are smarter than dogs"
# .* 表示任意匹配除换行符（\n、\r）之外的任何单个或多个字符
# re.M：多行匹配
# re.I:忽略大小写进行匹配
matchObj = re.match(r"(.*) are (.*?) .*", line, re.M | re.I)

if matchObj:
	print ("matchObj.group() : ", matchObj.group())
	print ("matchObj.group(1) : ", matchObj.group(1))
	print ("matchObj.group(2) : ", matchObj.group(2))
	print ("matchObj.group(3) : ", matchObj.group(1,2))
	print ("matchObj.group(0) : ", matchObj.group(0))
else:
	print ("No match!!")

print ("")
print ("")

matchObj = re.match(r"(.*) dogs (.*?) .*", line, re.M | re.I)#按照次序匹配
if matchObj:
   print ("match --> matchObj.group() : ", matchObj.group())
else:
   print ("No match!!")
 
matchObj = re.search( r'dogs', line, re.M|re.I)#只负责搜索
if matchObj:
   print ("search --> matchObj.group() : ", matchObj.group())
else:
   print ("No match!!")



print ("")
print ("--------替换---------")
phone = "2004-959-559 # 这是一个电话号码"

print (phone)
print ("删除注释")
num = re.sub(r'#.*$',"",phone)
print ("电话号码：",num)

print("移除非数字的内容")
num = re.sub(r'\D', "", phone)
print ("电话号码: ", num)


print ("")
print ("--------参数是一个函数---------")
print("将匹配的数字乘于 2")
def double(matched):
    value = int(matched.group('value'))
    print (value)
    return str(value * 2)
 
s = 'A23G4HFD567'
print(re.sub('(?P<value>\d+)', double, s))
print (s)



import tkinter as tk # 使用Tkinter前需要先导入

#建立窗口
window = tk.Tk()

#窗口名称
window.title('python  window')

#窗口大小
window.geometry('500x300')

#在图形界面上设定标签
#l = tk.Label(window, text='你好！this is Tkinter', bg='green', font=('Arial', 12), width=30, height=2)
# 说明： bg为背景，fg为字体颜色，font为字体，width为长，height为高，这里的长和高是字符的长和高，比如height=2,就是标签有2个字符这么高

var = tk.StringVar()    # 将label标签的内容设置为字符类型，用var来接收hit_me函数的传出内容用以显示在标签上
l = tk.Label(window, textvariable=var, bg='green', fg='white', font=('Arial', 12), width=30, height=2)

#放置标签
l.pack()

#Button 功能函数
on_hit = False
def hit_me():
    global on_hit
    if on_hit == False:
        on_hit = True
        var.set('you hit me')
    else:
        on_hit = False
        var.set('')


#button 按键
b = tk.Button(window, text='hit me', font=('Arial', 12), width=10, height=1, command=hit_me)
b.pack()

#主窗口循环显示
window.mainloop()












print ("")
print ("")
print ("")
print ("")