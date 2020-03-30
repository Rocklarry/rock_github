#!/usr/bin/env python
# -*- coding: utf-8 -*-


def power(values):
	for value in values:
		print ('powering %s' % value)
		yield value

def adder(values):
	for value in values:
		print ('adding %s' % value)
		if value % 2 == 0:
			yield value + 3
		else:
			yield value + 2




def fibonacci():
	a, b = 0, 1
	while True:
		yield b
		a, b = b ,a+b

fib = fibonacci()
print (fib.__next__())
print (fib.__next__())
print (fib.__next__())
print (fib.__next__())







elements = [1,4,7,9,12,19]
res = adder(power(elements))
res.__next__()
res.__next__()

res.__next__()
res.__next__()
res.__next__()
res.__next__()


