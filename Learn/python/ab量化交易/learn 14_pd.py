#!/usr/bin/env python
# -*- coding: utf-8 -*-
import numpy as np
import timeit
import pandas as pd

import matplotlib.mpl_finance as mpf
import matplotlib.pyplot as plt

from matplotlib import rcParams
rcParams['font.family']='simhei'

print('第二章 pandasd 的基本使用方法')

print ('保存本地数据 npy')

t2 = timeit.Timer('x=range(1000)')  
print (t2) 
print (t2.timeit())

stock_cnt = 200
view_days = 504
# 200个股票 504个交易日 求正态分布 均值 0 表差1
stock_day_change = np.random.standard_normal((stock_cnt,view_days))
print ("保存本地序列号操作")
np.save('D:\python_test\learn\gen\stock_day_change',stock_day_change)

print ("读出本地序列号")
stock_day_changes = np.load('D:\python_test\learn\gen\stock_day_change.npy')
print (stock_day_changes.shape)
print('输出head 结果')
#df=pd.DataFrame(stock_day_change).head()  #这三种写法相同
#df=pd.DataFrame(stock_day_change).head(10)
#df=pd.DataFrame(stock_day_change)[:10] #显示行
print(pd.DataFrame(stock_day_change)[:10])

stock_symbols = ['股票'+str(x) for x in range(stock_day_change.shape[0])]
#df = pd.DataFrame(stock_day_change,index=stock_symbols).head(5)
print(pd.DataFrame(stock_day_change,index=stock_symbols).head(5))

days = pd.date_range('2017-1-1',periods=stock_day_change.shape[1],freq='1D')# 是数字1 1d
stock_symbols = ['股票'+str(x) for x in range(stock_day_change.shape[0])]
df = pd.DataFrame(stock_day_change,index=stock_symbols,columns=days)
print (df.head(2))

print('金融时间序列')
df = df.T
print(df.head())

print ('21天内平均采用')


df_20 = df.resample('21D',how = 'mean')

print(df_20.head())

df_stock0 = df['股票0'] 
print(type(df_stock0)) #数据类型
print(df_stock0.head())

df_stock0.cumsum().plot()
plt.title(u"股票0的采样率",fontsize=16)
plt.show()#显示



'''
index = pd.date_range('1/1/2000', periods=9, freq='T')
series = pd.Series(range(9), index=index)
print(series)

print(series.resample('3T').sum())
'''

print('周期位5天的采样率')
df_stock0_5 = df_stock0.cumsum().resample('5D',how='ohlc')
print(df_stock0_5.head()) 
df_stock0_5.plot()
plt.title(u"周期位5天的采样率",fontsize=16)
plt.show()#

print('周期位21天的采样率')
df_stock0_21 = df_stock0.cumsum().resample('21D',how='ohlc')
print(df_stock0_21.head())
df_stock0_21.plot()
plt.title(u"周期位21天的采样率",fontsize=16)
plt.show()#


from abupy import ABuMarketDrawing

ABuMarketDrawing.plot_candle_stick(df_stock0_5.index,
				df_stock0_5 ['open'].values,
				df_stock0_5 ['high'].values,
				df_stock0_5 ['low'].values , 
				df_stock0_5 ['close'].values , 
				np.random. random( len(df_stock0_5)),
				None ,'stock',day_sum=False , 
				html_bk=False,  save=False)

print (type(df_stock0_5['open'].values))
print (df_stock0_5['open'].index)
print (df_stock0_5.cumsum)



