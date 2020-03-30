#!/usr/bin/env python
# -*- coding: utf-8 -*-

import numpy as np
import matplotlib.pyplot as plt
from matplotlib import rcParams
rcParams['font.family']='simhei'

print ('伯努利分布')

print ('设置100个赌徒')

gamblers = 100

def casino(win_rate,win_once=1,loss_once=1,commission=0.01):
	"""
		每人100w 玩1000w次
		赢的概率   win_rate
		每次赢得钱 win_once
		输的钱 loss_once
		手续费 0.01%
	"""
	my_money  = 1000
	play_cont = 10000
	commission = commission
	cont =0
	for _ in np.arange(0,play_cont):
		w = np.random.binomial(1,win_rate)
		cont+=1
		#print ('次数',cont)
		if w:
			my_money+=win_once
		else:
			my_money-=loss_once
		my_money-=commission
		if my_money <0:
			break
	return my_money

heaven_moneys = [casino(0.5,commission=0) for _ in np.arange(0,gamblers)]
print('10个赌徒 胜率0.5 赔率1 没手续费')
print ('heaven_moneys=',heaven_moneys)

cheat_moneys = [casino(0.4,commission=0) for _ in np.arange(0,gamblers)]
print('有抽头 老千倒是胜率0.4 ')
print('10个赌徒 胜率0.4 赔率1 没手续费')
print ('cheat_moneys=',cheat_moneys)

commission_moneys = [casino(0.5,commission=0.1) for _ in np.arange(0,gamblers)]
print('没有抽头 手续费0.01')
print('10个赌徒 胜率0.5 赔率1 没手续费0.01')
print ('commission_moneys=',commission_moneys)

_=plt.hist(heaven_moneys,bins=30)
_=plt.title(u"天堂赌场",fontsize=16)
_=plt.show()
_=plt.hist(cheat_moneys,bins=30)
_=plt.title(u"老千赌场",fontsize=16)
_=plt.show()
_=plt.hist(commission_moneys,bins=30)
_=plt.title(u"抽头赌场",fontsize=16)
_=plt.show()

money=[casino(0.5,commission=0.01,win_once=1.02,loss_once=0.98)for _ in np.arange(0,gamblers)]

_=plt.hist(money,bins=30)
_=plt.title(u"非均衡赔率赌场",fontsize=16)
print('胜率0.5 赔率1.04 手续费 0.01')
_=plt.show()

moneys = [casino(0.45,commission=0.01,win_once=1.02,loss_once=0.98)for _ in np.arange(0,gamblers)]
_=plt.hist(moneys,bins=30)
_=plt.title(u"非均衡胜率和非均衡赔率 赌场",fontsize=16)
print('胜率0.45 赔率1.04 手续费 0.01')
_=plt.show()