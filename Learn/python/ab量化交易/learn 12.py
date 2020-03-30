#!/usr/bin/env python
# -*- coding: utf-8 -*-

import numpy as np
import timeit
import matplotlib.pyplot as plt

print('统计学概念')
print('\n期望')
print('方差	  数据离散程度')
print('标准差  方差的算术平方根  表示数据分布离散程度')
print('        可以变成获利稳定性')
print('a b两个交易者')
a_investor = np.random.normal(loc=100,scale = 50,size=(100,1))
b_investor = np.random.normal(loc=100,scale = 20,size=(100,1))

#print(a_investor)
#print(b_investor)

print('a交易者期望{0:.2f}元,标准差{1:.2f}，方差{2:.2f}'.format(a_investor.mean(),a_investor.std(),a_investor.var() ))
print('b交易者期望{0:.2f}元,标准差{1:.2f}，方差{2:.2f}'.format(b_investor.mean(),b_investor.std(),b_investor.var() ))
'''
a_mean = a_investor.mean()
a_std = a_investor.std()
plt.plot(a_investor,color='r')
plt.axhline(a_mean+a_std,color ='y')
plt.axhline(a_mean,color='y')
plt.axhline(a_mean-a_std,color='y')


b_mean = b_investor.mean()
b_std = b_investor.std()
plt.plot(b_investor,color ='b')
plt.axhline(b_mean+b_std,color ='g')
plt.axhline(b_mean,color='g')
plt.axhline(b_mean-b_std,color='g')
#plt.show()

'''

import scipy.stats as scs

t2 = timeit.Timer('x=range(1000)')  
print (t2) 
print (t2.timeit())

stock_cnt = 200
view_days = 504
# 200个股票 504个交易日 求正态分布 均值 0 表差1
stock_day_change = np.random.standard_normal((stock_cnt,view_days))

print ("正态分布")
print (stock_day_change.shape) #200行 504列

stock_mean = stock_day_change[0].mean()
stock_std = stock_day_change[0].std()

print('股票0 mean均值期望值:{:.3f}'.format(stock_mean))
print('股票0 std振幅标准差:{:.3f}'.format(stock_std))
'''
plt.hist(stock_day_change[0],bins=50,normed=True)
fir_linspace = np.linspace(stock_day_change[0].min(),stock_day_change[0].max())
pdf = scs.norm(stock_mean,stock_std).pdf(fir_linspace)
plt.plot(fir_linspace,pdf,lw=2,c="r")

#print (pdf)
plt.show()
'''

keep_days = 50
print ('保留50天随机数据作为策略')
print ('统计454天中200只股票涨跌数据 切片出0-454天 view_days=504')
print ('打印出454天 3只跌幅最大的股票 总跌幅通过sum计算 sort排列')
stock_day_change_test = stock_day_change[:stock_cnt,0:view_days-keep_days]

print(np.sort(np.sum(stock_day_change_test,axis=1))[:3])

print ('使用argsort 对股票跌幅排序 返回序列')
stock_lower_arrary = np.argsort(np.sum(stock_day_change_test,axis=1))[:3]

print (stock_lower_arrary)

def show_buy_lower(stock_ind):
	#设置客户化表
	_, axs = plt.subplots(nrows=1,ncols=2,figsize=(16,5))
	#绘制走势图
	axs[0].plot(np.arange(0,view_days-keep_days),stock_day_change_test[stock_ind].cumsum())
	cs_buy=stock_day_change[stock_ind][view_days-keep_days:view_days].cumsum()
	axs[1].plot(np.arange(view_days-keep_days,view_days),cs_buy)
	return cs_buy[-1]

profit = 0
for stock_ind in stock_lower_arrary:
	profit+=show_buy_lower(stock_ind)

print ('买入{} 只股票， 从454天开始计算交易日持续盈亏：{:.2f}%'.format(stock_lower_arrary,profit))




