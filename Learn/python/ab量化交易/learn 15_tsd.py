#!/usr/bin/env python
# -*- coding: utf-8 -*-

import numpy as np
from  abupy  import  ABuSymbolPd 
import matplotlib.pyplot as plt
import pandas as pd

from matplotlib import rcParams
rcParams['font.family']='simhei'


#  n_folds=2两年
tsla_df =  ABuSymbolPd.make_kl_df('usTSLA',n_folds=2) 
#表4-7 所示
tsla_df.tail()
print(tsla_df.head(20))
tsla_df.plot()
plt.title(u"tsl",fontsize=16)
plt.show()#

tsla_df[['close','volume']].plot(subplots=True,style=['r','g'],grid=True)
plt.title(u"总览分析数据",fontsize=16)
plt.show()#

print (tsla_df.info())

print (tsla_df.describe())
print('显示2018-01-17到2018-01-28开盘价')
print(tsla_df.loc['2018-01-17':'2018-01-28','open'])
print('显示2018-01-17到2018-01-28开所有数据')
print(tsla_df.loc['2018-01-17':'2018-01-28'])

print('显示[1:5,2:6]数据')
print (tsla_df.iloc[1:5,2:6])

print('显示涨跌幅大于8')
#注意数据格式 tsla_df数据的属性有open    high     low   close   volume  pre_close  p_change等等
print(tsla_df[np.abs( tsla_df.p_change)>8])
print('# 涨跌幅大于8%，交易成交量大于统计周期内的平均值的2.5倍')
print(tsla_df[(np.abs(tsla_df.p_change) > 8) & (tsla_df.volume > 2.5 * tsla_df.volume.mean())])

print ('数据转换与规整')
print(tsla_df.sort_index(by='p_change')[:5])


print('数据本地序列化操作')

#tsla_df.to_csv('D:\python_test\learn\gen\/tsla_df.csv', columns=tsla_df.columns, index=True)
#tsla_df_load = pd.read_csv('.\gen\/tsla_df.csv', parse_dates=True, index_col=0)
#tsla_df_load.head()

print('针对价格序列做pct_change（）后即为涨跌幅')
print(tsla_df.close.pct_change()[:3])

print('针对close做pct change （）后的结果就是涨跌幅')
change_ratio = tsla_df.close.pct_change
print(change_ratio)

print('保留两位小数')
print(np.round(tsla_df[-5:] * 100, 2))

print('****************************************')
print('*************tsla_df.info**************')
print('****************************************')
print(tsla_df.info)

xt = pd.crosstab(tsla_df.date_week,tsla_df.key)
print(xt)
