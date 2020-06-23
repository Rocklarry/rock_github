#########################################################################
# File Name: test_reboot.sh
# Author: rock
# mail: rock_telp@163.com
# Created Time: Wednesday, December 04, 2019 PM01:45:16 HKT
#########################################################################
#!/bin/bash


#创建文件
if [ ! -f "tag_test.txt" ]; then
 touch "tag_test.txt"
 echo "创建文件"
 echo 10 > tag_test.txt
fi

#读取数据
echo $(cat tag_test.txt)
dataline=$(cat tag_test.txt)
echo "测试次数"=$dataline






if [ $dataline == 10 ]
then
	echo "测试2h 1次"
	#/system/bin/stressapptest -s 120 -i 4 -m 4 -M 400MB -W -l /sdcard/stressapptest_400MBmemory2m.log >2h_1.out 
	echo 11 > tag_test.txt
	echo "写入 11"
elif [ $dataline == 11 ]
then
	echo "测试2h 2次"
	#/system/bin/stressapptest -s 120 -i 4 -m 4 -M 400MB -W -l /sdcard/stressapptest_400MBmemory2m.log >2h_2.out 
	echo 12 > tag_test.txt
	echo "写入 12"
elif [ $dataline == 12 ]
then
	echo "测试2h 3次"
	#/system/bin/stressapptest -s 120 -i 4 -m 4 -M 400MB -W -l /sdcard/stressapptest_400MBmemory2m.log >2h_3.out 
	echo 13 > tag_test.txt
elif [ $dataline == 13 ]
then
	echo "测试2h 4次"
	#/system/bin/stressapptest -s 120 -i 4 -m 4 -M 400MB -W -l /sdcard/stressapptest_400MBmemory2m.log >2h_4.out 
	echo 20 > tag_test.txt
elif [ $dataline == 20 ]
then
	echo "测试4h 1次"
	#/system/bin/stressapptest -s 120 -i 4 -m 4 -M 400MB -W -l /sdcard/stressapptest_400MBmemory2m.log >4h_1.out 
	echo 21 > tag_test.txt
elif [ $dataline == 21 ]
then
	echo "测试4h 2次"
	#/system/bin/stressapptest -s 120 -i 4 -m 4 -M 400MB -W -l /sdcard/stressapptest_400MBmemory2m.log >4h_2.out 
	echo 22 > tag_test.txt
elif [ $dataline == 22 ]
then
	echo "测试4h 2次"
	#/system/bin/stressapptest -s 120 -i 4 -m 4 -M 400MB -W -l /sdcard/stressapptest_400MBmemory2m.log >4h_3.out 
	echo 23 > tag_test.txt
elif [ $dataline == 23 ]
then
	echo "测试4h 4次"
	#/system/bin/stressapptest -s 120 -i 4 -m 4 -M 400MB -W -l /sdcard/stressapptest_400MBmemory2m.log >4h__4.out 
	echo 24 > tag_test.txt
else
	echo "测试结束"
fi

#/system/bin/stressapptest -s $[$dataline*120] -i 4 -m 4 -M 400MB -W -l /sdcard/stressapptest_400MBmemory2m.log >$dataline.out 

#循环
#for ((i=1; i<=$dataline; i++))
#do
#  echo "循环测试"+$i
#done

#下次开启写入次数
#tmp=$[$dataline+2]
#echo $tmp > tag_test.txt





