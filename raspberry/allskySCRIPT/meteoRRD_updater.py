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
    # Port série DEVICEPORT
    # Vitesse de baud : 9600
    ser = serial.Serial(DEVICEPORT,9600)
    serial_line = ser.readline()
    print(serial_line)
    ser.close()
    T=1
    HR=2
    Thr=3
    dew=4
    IR=5
    Tir=6
    clouds=7
    skyT=8
    P=9
    Tp=10
    Light=11
    Wind=12
    WindMax=13
    CRain=14
    TRain=15
    PIDRain=16
    HRint=17
    Thrint=18
    cloudFlag=19
    dewFlag=20
    frezzingFlag=21
    daylightFlag=22
    windFlag=23
    rainFlag=24

    return (("T",T),("HR",HR),("Thr",Thr),("Dew",dew),("IR",IR),("Tir",Tir),("clouds",clouds),("skyT",skyT),
           ("P",P),("Tp",Tp),("Light",Light),("Wind",Wind),("WindMax",WindMax),("CRain",CRain),("TRain",TRain),
           ("PIDRain",PIDRain),("HRint",HRint),("Thrint",Thrint),("cloudFlag",cloudFlag),("dewFlag",dewFlag),
           ("frezzingFlag",frezzingFlag),("daylightFlag",daylightFlag),("windFlag",windFlag),("rainFlag",rainFlag))

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
    json_dict={"TIME":time.strftime("%c",now)}
    data=recv_serial()
    updateString="N"
    for d in data:
        #print d[0],d[1]
        updateString=updateString+":"+str(d[1])
        #json_dict[d[0]]=int(d[1]*100)/100.
        print updateString
    #ret = rrdtool.update(CHARTPATH+'meteo.rrd',updateString);
    #if ret:
    #    print rrdtool.error() 
    #    x = simplejson.dumps(json_dict)
    #fi=open(CHARTPATH+"RTdata.json","w")
    #fi.write(x)
    #fi.close()
    del data
    del json_dict 
    #collected = gc.collect()

    time.sleep(10)
  except:
    print "UPDATER FAIL"
    time.sleep(10)
