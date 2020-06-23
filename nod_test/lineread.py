# -*- coding: utf-8 -*-
'''
python读取文件，每两行为一组
'''
import io
def fenhang(infile,outfile):
    infopen = io.open(infile,'r',encoding='utf-8')
    outopen = io.open(outfile,'w',encoding='utf-8')
    lines = infopen.readlines()
    i = 1
    for line in lines:
	if i % 2 == 0:
            outopen.write(line.strip()+"},\n")
	else:
	    outopen.write("{"+line.strip()+",")
	i += 1
    infopen.close()
    outopen.close()
fenhang("myout.txt","tt.h")
