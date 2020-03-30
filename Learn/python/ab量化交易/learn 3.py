#!/usr/bin/env python
# -*- coding: utf-8 -*-
import tkinter as tk

window = tk.Tk()

window.title('python window')
window.geometry('500x300')

e = tk.Entry(window,show = None)
e.pack()

def insert_point(): # 在鼠标焦点处插入输入内容
    var = e.get()
    t.insert('insert', var)

def insert_end():
	var = e.get()
	t.insert('end',var)

b1 = tk.Button(window, text='insert point', width=10,
               height=2, command=insert_point)
b1.pack()

b2 = tk.Button(window, text='insert end', width=10,
               height=2, command=insert_end)
b2.pack()

t = tk.Text(window,height=3)
t.pack()




var1 = tk.StringVar()  # 创建变量，用var1用来接收鼠标点击具体选项的内容
l = tk.Label(window, bg='green', fg='yellow',font=('Arial', 12), width=10, textvariable=var1)
l.pack()

def print_selection():
	value = lb.get(lb.curselection()) # 获取当前选中的文本
	var1.set(value)

b1 = tk.Button(window, text='print selection', width=15, height=2, command=print_selection)
b1.pack()



var2 = tk.StringVar()
var2.set((1,2,3,4)) # 为变量var2设置值

lb = tk.Listbox(window, listvariable=var2)  #将var2的值赋给Listbox
# 创建一个list并将值循环添加到Listbox控件中
list_items = [11,22,33,44,'aa','bb','cc']
for item in list_items:
    lb.insert('end', item)  # 从最后一个位置开始加入值
lb.insert(1, 'first')       # 在第一个位置加入'first'字符
lb.insert(2, 'second')      # 在第二个位置加入'second'字符
lb.delete(2)                # 删除第二个位置的字符
lb.pack()

window.mainloop()