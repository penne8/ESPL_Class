#!/usr/bin/env python3
# -*- coding: utf-8 -*-
"""
@author: Pan @ Omri
"""
import socket
import threading
import queue
import sys
import sqlite3
from sqlite3 import Error

bufferSize = 1024


#Server Code
def RecvData(sock,recvPackets):
    while True:
        data, addr = sock.recvfrom(bufferSize)
        recvPackets.put((data,addr))

def RunServer(host):
    return 0
#Serevr Code Ends Here

if __name__ == '__main__':
    RunServer(sys.argv[1])