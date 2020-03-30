#!/usr/bin/env python
# -*- coding: utf-8 -*-

import tkinter as tk

window = tk.Tk()
window.title('python window')

window.geometry('500x300')

l = tk.Label(window, text='      ', bg='green')
l.pack()

counter = 0

#用来代表菜单选项的功能
def do_job():
	global counter
	l.config(text='do'+str(counter))
	counter +=1


#触发函数功能
def hit_me():
    tkinter.messagebox.showinfo(title='Hi', message='你好！')    


#创建一个菜单栏
menubar = tk.Menu(window)	

#创建一个File菜单项（默认不下拉，下拉内容包括New，Open，Save，Exit功能项）
filemenu = tk.Menu(menubar, tearoff=0)

#将上面定义的空菜单命名为File，放在菜单栏中，就是装入那个容器中
menubar.add_cascade(label='File', menu=filemenu)

# 在File中加入New、Open、Save等小菜单，即我们平时看到的下拉菜单，每一个小菜单对应命令操作。
filemenu.add_command(label='New', command=do_job)
filemenu.add_command(label='Open', command=do_job)
filemenu.add_command(label='Save', command=do_job)
filemenu.add_separator()    # 添加一条分隔线
filemenu.add_command(label='Exit', command=window.quit) # 用tkinter里面自带的quit()函数


#创建一个Edit菜单项（默认不下拉，下拉内容包括Cut，Copy，Paste功能项）
editmenu = tk.Menu(menubar, tearoff=0)
# 将上面定义的空菜单命名为 Edit，放在菜单栏中，就是装入那个容器中
menubar.add_cascade(label='Edit', menu=editmenu)
 
# 同样的在 Edit 中加入Cut、Copy、Paste等小命令功能单元，如果点击这些单元, 就会触发do_job的功能
editmenu.add_command(label='Cut', command=do_job)
editmenu.add_command(label='Copy', command=do_job)
editmenu.add_command(label='Paste', command=do_job)

#创建第二级菜单，即菜单项里面的菜单
submenu = tk.Menu(filemenu) # 和上面定义菜单一样，不过此处实在File上创建一个空的菜单
filemenu.add_cascade(label='Import', menu=submenu, underline=0) # 给放入的菜单submenu命名为Import



#创建第三级菜单命令，即菜单项里面的菜单项里面的菜单命令（有点拗口，笑~~~）
submenu.add_command(label='Submenu_1', command=do_job)   # 这里和上面创建原理也一样，在Import菜单项中加入一个小菜单命令Submenu_1



tagger = tk.Menu(menubar, tearoff=0)
menubar.add_cascade(label='触发', menu=tagger)
tagger.add_command(label='触发1', command=do_job)
tagger.add_command(label='触发2', command=do_job)
tagger.add_command(label='触发3', command=do_job)

#创建菜单栏完成后，配置让菜单栏menubar显示出来
window.config(menu=menubar)

#主窗口循环显示
window.mainloop()





