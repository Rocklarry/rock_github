# _*_ coding:utf-8 _*_
#xiaohei.python.seo.call.me:)
#win+python2.7.x
import csv
csvfile = file('csvtest.csv', 'wb')
writer = csv.writer(csvfile)
writer.writerow(['id', 'url', 'keywords'])
data = [
  ('1', 'http://www.xiaoheiseo.com/', '小黑'),
  ('2', 'http://www.baidu.com/', '百度'),
  ('3', 'http://www.jd.com/', '京东')
]
writer.writerows(data)
csvfile.close()