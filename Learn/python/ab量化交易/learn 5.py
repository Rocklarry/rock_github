#!/usr/bin/env python
# -*- coding: utf-8 -*-

def function_1():
	numbers = range(10)
	size = len(numbers)
	evens = []
	i = 0
	while i<size:
		if i % 2 == 0:
			evens.append(i)
		i += 1

	print (evens)


#高效 简洁方式
# []相当于数字 循环赋值
print ([i for i in range(10) if i%2==0])


def function_2():
	#enumerate 类
 	i = 0
 	seq = ["one","twO","three"]
 	for element in seq:
 		seq[i] = '%d: %s' % (i,seq[i])
 		i +=1
 	print (seq)


function_1()
function_2()