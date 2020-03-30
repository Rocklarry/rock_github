#!/usr/bin/env python
# -*- coding: utf-8 -*-

from __future__ import with_statement
from contextlib import closing
import socket

import multitask
import time

def coroutine_1():
	for i in range(5):
		print ('c1111')
		yield i

def coroutine_2():
	for i in range(5):
		print ('c2222')
		yield i	
		

#multitask.add(coroutine_1())
#multitask.add(coroutine_2())	
#multitask.run()



def client_handler(sock):
    with closing(sock):
        while True:
            data=(yield multitask.recv(sock,1024))
            print (data)
            if not data:
                break
            yield multitask.send(sock,data)


def echo_server(hostname,port):
    addrinfo=socket.getaddrinfo(hostname,port,socket.AF_UNSPEC,socket.SOCK_STREAM)
    (family,socktype,proto,canonname,sockaddr)=addrinfo[0]
    with closing(socket.socket(family,socktype,proto)) as sock:
        sock.setsockopt(socket.SOL_SOCKET,socket.SO_REUSEADDR,1)
        sock.bind(sockaddr)
        sock.listen(5)
        while True:
            multitask.add(client_handler((yield multitask.accept(sock))[0]))

if __name__=='__main__':
    import sys
    hostname=None
    port=1111
    if len(sys.argv)>1:
        hostname=sys.argv[1]
    if len(sys.argv)>2:
        port=int(sys.argv[2])
    multitask.add(echo_server(hostname,port))
    try:
        multitask.run()
    except KeyboardInterrupt:
        pass

