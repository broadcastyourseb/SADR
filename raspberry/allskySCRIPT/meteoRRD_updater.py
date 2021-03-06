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
from time import strftime

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
                elif len(arg) == 27 and arg[0]=='error':
                    print("Unvalid serial line received: " + str(arduino))
                    test = True
                    arduino=""
                else:
                    line = []
                    arduino=""
    ser.close()
    return arduino

def weather_safe():
    print("Weather is good. Check if IsSafe.flg is here")
    if not os.path.isfile(CHARTPATH+"IsSafe.flg"):
    #if not we create it
        print("File IsSafe.flg created")
        os.mknod(CHARTPATH+"IsSafe.flg")
    else:
        print("File IsSafe.flg already here")

def weather_not_safe():
    print("Weather is bad. Check if IsSafe.flg is here")
    if os.path.isfile(CHARTPATH+"IsSafe.flg"):
    #if it is, we delete it
        print("File IsSafe.flg deleted")
        os.remove(CHARTPATH+"IsSafe.flg")
    else:
        print("File IsSafe.flg already deleted")

############# MAIN #############

print "Starting UPDATER"
## Write configuration javascript 
fi=open(CHARTPATH+"meteoconfig.js","w")
fi.write("var altitude=%s\n" % ALTITUDE)
fi.write("var sitename=\"%s\"\n" % SITENAME)
fi.write("var ownername=\"%s\"\n" % OWNERNAME)
SERVERTIMEZONE = int(strftime('%z'))/100
fi.write("var servertimezone=%s\n" % SERVERTIMEZONE)
fi.close()

#connect an retrive info
while (True):
  try:
    now=time.localtime()
    json_dict={"TIME":time.strftime("%c",now)}
    data=recv_serial()
    print("Data: " + str(data))
    if data != "" :
        splitData = data.split(':')
        tupleData = (("T",splitData[1]), ("frezzingFlag",splitData[2]), ("Light",splitData[3]), ("daylightFlag",splitData[4]),
     ("Thrint",splitData[5]), ("HRint",splitData[6]), ("Thr",splitData[7]), ("HR",splitData[8]),
     ("Dew",splitData[9]), ("dewFlag",splitData[10]), ("Tir",splitData[11]), ("IR",splitData[12]), ("skyT",splitData[13]),
     ("clouds",splitData[14]), ("cloudFlag",splitData[15]), ("P",splitData[16]), ("Tp",splitData[17]), ("Wind",splitData[18]),
     ("WindMax",splitData[19]), ("windFlag",splitData[20]), ("CRain",splitData[21]), ("rainFlag",splitData[22]),
     ("TargetRain",splitData[23]), ("TRain",splitData[24]), ("PIDRain",splitData[25]))
        for d in tupleData:
             #print d[0],float(d[1])
             json_dict[d[0]]=float(d[1])
        ret = rrdtool.update(CHARTPATH+'meteo.rrd',data);
        if ret:
            print rrdtool.error()
        x = simplejson.dumps(json_dict)
        print x
        fi=open(CHARTPATH+"RTdata.json","w")
        fi.write(x)
        fi.close()
        fi=open(CHARTPATH+"luminosity.js","w")
        fi.write("var luminosity=%s\n" % splitData[3])
        fi.close()
        # check if weather (cloud, rain, luminosity) is safe or not
        if float(splitData[15]) == 1 or float(splitData[22]) == 1 or float(splitData[4]) == 1 or float(splitData[20]) == 1:
             weather_not_safe()
        else:
             weather_safe()
        del data
        del splitData
        del json_dict
        collected = gc.collect()
    time.sleep(30)
  except:
      print "UPDATER FAIL"
      time.sleep(10)
