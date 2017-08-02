#!/usr/bin/python
#-*- coding: iso-8859-15 -*-
# SADR METEOLLSKY
# http://www.sadr.fr
# SEBASTIEN LECLERC 2017
# Inspired by :
# NACHO MAS 2013
# http://induino.wordpress.com 
# 

import sys, os
import math
import time
import signal
import rrdtool
from meteollskyconfig import *
import simplejson
import gc
import serial

signal.signal(signal.SIGINT, signal.SIG_DFL)

def recv_serial():
    ser = serial.Serial(DEVICEPORT, 9600, timeout=1)
    print("connected to: " + ser.portstr)

    #this will store the line
    line = []
    arduino = ""
    test = False
    while test == False:
        for c in ser.read():
            line.append(c)
            if c == '\r'or c=='\n':
                del line[len(line)-1]
                arduino = ''.join(line)
                # check integrity of the serial line
                arg = arduino.split(':')
                if len(arg) == 26 and arg[0]=='N':
                    print("Valid serial line received: " + str(len(arg)) + " parts")
                    test = True
                else:
                    line = []
                    arduino=""
    ser.close()
    return arduino

############# MAIN #############

print "Starting UPDATER"
## Write configuration javascript 
fi=open(CHARTPATH+"meteoconfig.js","w")
fi.write("var altitude=%s\n" % ALTITUDE)
fi.write("var sitename=\"%s\"\n" % SITENAME)
fi.write("var OWNERNAME=\"%s\"\n" % OWNERNAME)
fi.close()

#connect an retrive info
while (True):
  try:
    now=time.localtime()
    data=recv_serial()
    print(data)
    ret = rrdtool.update(CHARTPATH+'meteo.rrd',data);
    if ret:
        print rrdtool.error() 
    del data
    collected = gc.collect()
    time.sleep(10)
  except:
    print "UPDATER FAIL"
    time.sleep(10)
