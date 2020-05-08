# coding: UTF-8  #设置编码

'''
给每行末尾加","逗号
'''
ff = open('untitdd.txt','w')  #打开一个文件，可写模式
with open('untit.txt','r') as f:  #打开一个文件只读模式
    line = f.readlines()#读取文件中的每一行，放入line列表中
    for line_list in line:
        line_new =line_list.replace('\n','')	#将换行符替换为空('')
        line_new=line_new+r','+'\n'  #行末尾加上"|",同时加上"\n"换行符
        print(line_new)
        ff.write(line_new) #写入一个新文件中