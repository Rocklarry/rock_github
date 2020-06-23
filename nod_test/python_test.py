#!/usr/bin/env python                                                                                                                                        
# -*- coding: UTF-8 -*-

import time
import os

def main():
	date_stamp = time.strftime('%y%m%d%H%M%S',time.localtime(time.time()))
	print date_stamp

	date_stamp = time.strftime('%y%m%d',time.localtime(time.time()))
	print date_stamp

	localtime = time.asctime( time.localtime(time.time()) )
	print "本地时间为 :", localtime

if __name__ == '__main__':
    main()
