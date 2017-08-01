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
    #ser = serial.Serial(port=DEVICEPORT,baudrate=9600,parity=serial.PARITY_NONE,stopbits=serial.STOPBITS_ONE,bytesize=serial.EIGHTBITS,timeout=0)
    ser = serial.Serial(DEVICEPORT, 9600, timeout=0)
    print("connected to: " + ser.portstr)

    #this will store the line
    line = []

    while True:
        for c in ser.read():
            line.append(c)
            if c == '\n':
		toto = ''.join(line)
                print(toto)
                line = []
                break
                # check integrity of the serial line
                #arg = line.split(':')
                #if len(arg) == 25 and arg[0]=='N':
                #    print "Valid serial line received"
                #print len(arg) 
                #line = []
                break
    ser.close()
    print line   
    return line

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
  #try:
	
    now=time.localtime()
    #json_dict={"TIME":time.strftime("%c",now)}
    data=recv_serial()
    #json_dict[d[0]]=int(d[1]*100)/100.
    print data
    #ret = rrdtool.update(CHARTPATH+'meteo.rrd',data);
    #if ret:
    #    print rrdtool.error() 
    #    x = simplejson.dumps(json_dict)
    #fi=open(CHARTPATH+"RTdata.json","w")
    #fi.write(x)
    #fi.close()
    del data
    #del json_dict 
    collected = gc.collect()
    time.sleep(10)
  #except:
    #print "UPDATER FAIL"
    #time.sleep(10)
