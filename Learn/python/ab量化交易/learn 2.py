#!/usr/bin/env python
# -*- coding: utf-8 -*-




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


e1 = tk.Entry(window, show='*', font=('Arial', 14))   # 显示成密文形式
e2 = tk.Entry(window, show=None, font=('Arial', 14))  # 显示成明文形式
e1.pack()
e2.pack()


#主窗口循环显示
window.mainloop()












print ("")
print ("")
print ("")
print ("")