#!/usr/bin/env python
# -*- coding: utf-8 -*-

import time
import hashlib
import pickle
from itertools import chain

cache = {}
def is_obsolete(entry, duration):
	return time.time() - entry['time'] > duration

def compute_key(function, args, kw):
	key = pickle.dumps((function.func_name,args,kw))
	return hashlib.shal(key).hexdigest()

def memoize(duration = 10):
	def _memoize(function):
		def __memoize(*args,**kw):
			key = compute_key(function.args,kw)

			if(key in cache and 
				not  is_obsolete(cache[key],duration)):
				print (' we got a winner')
				return cache[key]['value']

			result = function(*args,**kw)

			cache[key] = {'value':result, 'time':time.time()}
		return result
	return __memoize
return _memoize

