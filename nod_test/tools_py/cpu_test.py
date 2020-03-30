#!/usr/bin/env python
# -*- coding: utf-8 -*-


import os
import sys
def cpu_count():
	import multiprocessing
	return multiprocessing.cpu_count()




def main():
	print '\n cpu线程数:%d\n'%cpu_count()



if __name__ == '__main__':
    main()
