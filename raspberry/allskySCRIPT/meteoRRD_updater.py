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
fi.write("var ownername=\"%s\"\n" % OWNERNAME)
fi.close()

#connect an retrive info
while (True):
  try:
    now=time.localtime()
    json_dict={"TIME":time.strftime("%c",now)}
    data=recv_serial()
    splitData = data.split(':')
    tupleData = (("T",splitData[1]), ("frezzing",splitData[2]), ("Light",splitData[3]), ("daylight",splitData[4]),
 ("T22int",splitData[5]), ("Hr22int",splitData[6]), ("T22ext",splitData[7]), ("Hr22ext",splitData[8]),
 ("DewExt",splitData[9]), ("dewing",splitData[10]), ("Tir",splitData[11]), ("IR",splitData[12]), ("skyT",splitData[13]),
 ("Clouds",splitData[14]), ("cloudy",splitData[15]), ("P",splitData[16]), ("Tp",splitData[17]), ("WindSpeed",splitData[18]),
 ("MaxWindSpeed",splitData[19]), ("windy",splitData[20]), ("Capacity",splitData[21]), ("rainy",splitData[22]),
 ("Consigne",splitData[23]), ("Temp",splitData[24]), ("Mosfet",splitData[25]))
    for d in tupleData:
         #print d[0],float(d[1])*100/100.
         json_dict[d[0]]=float(d[1])*100/100.
    ret = rrdtool.update(CHARTPATH+'meteo.rrd',data);
    if ret:
        print rrdtool.error()
    x = simplejson.dumps(json_dict)
    print x
    fi=open(CHARTPATH+"RTdata.json","w")
    fi.write(x)
    fi.close()
    del data
    del json_dict
    collected = gc.collect()
    time.sleep(30)
  except:
    print "UPDATER FAIL"
    time.sleep(10)
