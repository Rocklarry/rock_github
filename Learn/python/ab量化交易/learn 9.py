#!/usr/bin/env python
# -*- coding: utf-8 -*-

class WhatFor(object):
	def it(cls):
		print ('work with %s ' % cls)

	it = classmethod(it)

	def uncommmon():
		print ('I could be a global function')
	uncommmon = staticmethod(uncommmon)



class WhatFor(object):
	@classmethod
	def it(cls):
		print ('work with %s ' % cls)

	@staticmethod
	def uncommmon():
		print ('I could be a global function')

this_is = WhatFor()
this_is.it()

this_is.uncommmon()




