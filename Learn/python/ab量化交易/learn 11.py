#!/usr/bin/env python
# -*- coding: utf-8 -*-

import numpy as np
import timeit

t2 = timeit.Timer('x=range(1000)')  
print (t2) 
print (t2.timeit())

stock_cnt = 200
view_days = 504
# 200个股票 504个交易日 求正态分布 均值 0 表差1
stock_day_change = np.random.standard_normal((stock_cnt,view_days))

print ("正态分布")
print (stock_day_change.shape) #200行 504列

print ("第一只股票 前五个交易涨跌")
print (stock_day_change[0:1, :5])

print ("第一 二只股票 前五个交易涨跌")
print (stock_day_change[0:2, 0:5])

print ("倒数一 二只股票 最后五个交易涨跌")
print (stock_day_change[0:2, 0:5])

print ("倒数一 二只股票 最后五个交易涨跌 **保留两位小数")
print (np.around(stock_day_change[0:2, 0:5],2))

print ("倒数一 二 使用copy 不改变顺序 用nan代表缺失")
tmp_test = stock_day_change[0:2, 0:5].copy()
tmp_test[0] [0] = np.nan
print(tmp_test)
#print (np.nan(stock_day_change[0:2, 0:5].copy()))

print ('筛选')
mask = stock_day_change[0:2,0:5]>0.5
print (mask)

tmp_test = stock_day_change[0:2,0:5].copy()
tmp_test = np.around(stock_day_change[0:2,0:5],2).copy()

print (tmp_test)
print (tmp_test[mask])

print (">0.5 赋值 1")
tmp_test[tmp_test >0.5] = 1
print (tmp_test)


print ('通用系列函数')
print ('是否全部上涨')
print (np.all(stock_day_change[0:2,0:5] >0))

print ('是否有一个上涨')
print (np.any(stock_day_change[0:2,0:5] >0))

print ("minimum")
print (np.minimum(stock_day_change[0:2,0:5],stock_day_change[-2,-5]))

print ("maximum")
print (np.maximum(stock_day_change[0:2,0:5],stock_day_change[-2,-5]))

print ("唯一不重复的")			#两行五列
change_int = stock_day_change[0:2,0:5].astype(int)
print (change_int)

print ("唯一不重复的新数组")	
print (np.unique(change_int))

print ("前后两个临近值进行减法")
#axis = 1 axis 代表操作轴
print (np.diff(stock_day_change[0:2,0:5]))

print (np.diff(stock_day_change[0:2,0:5],axis=0))

print ("类似三目运算")
tmp_test = stock_day_change[-2:,-5:]
print (np.where(tmp_test>0.5,1,0))

print ("保存本地序列号操作")
np.save('D:\python_test\learn\gen\stock_day_change',stock_day_change)
stock_day_change = np.load('D:\python_test\learn\gen\stock_day_change.npy')
print (stock_day_change.shape)

#print (stock_day_change)
print ("保存本地序列号操作")

print ("基础统计")

print ("用切片保留前4只 前4天涨跌幅度")
stock_day_change_four = stock_day_change[:4,:4]
print (stock_day_change_four)

print ("横向分析出某个4天内的统计信息")

print ('最大涨幅{}'.format(np.max(stock_day_change_four,axis=1)))
print ('最大跌幅{}'.format(np.min(stock_day_change_four,axis=1)))
print ('振幅幅度{}'.format(np.std(stock_day_change_four,axis=1)))
print ('平均涨跌{}'.format(np.mean(stock_day_change_four,axis=1)))

print('纵向统计')
print ('最大涨幅{}'.format(np.max(stock_day_change_four,axis=1)))
print ('最大跌幅{}'.format(np.min(stock_day_change_four,axis=1)))
print ('振幅幅度{}'.format(np.std(stock_day_change_four,axis=1)))
print ('平均涨跌{}'.format(np.mean(stock_day_change_four,axis=1)))

print ('\n\n  =========== axis=1 =========>')
print (' ||')
print (' ||')
print (' ||')
print (' ||')
print (' ||')
print (' ||')
print (' axis=0 ')
print (' ||')
print (' ||')
print ('\  /')
print (' \/')

