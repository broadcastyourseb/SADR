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


def recv_indi():
	tim=time.localtime()
        vectorHR=indi.get_vector(INDIDEVICE,"HR")
	HR=vectorHR.get_element("HR").get_float()
	Thr=vectorHR.get_element("T").get_float()

        vectorPresure=indi.get_vector(INDIDEVICE,"Presure")
	P=vectorPresure.get_element("P").get_float()
	Tp=vectorPresure.get_element("T").get_float()

        vectorIR=indi.get_vector(INDIDEVICE,"IR")
	IR=vectorIR.get_element("IR").get_float()
	Tir=vectorIR.get_element("T").get_float()

        vectorMeteo=indi.get_vector(INDIDEVICE,"Meteo")
	dew=vectorMeteo.get_element("DEW").get_float()
	clouds=vectorMeteo.get_element("clouds").get_float()
	T=vectorMeteo.get_element("T").get_float()
        skyT=vectorMeteo.get_element("SkyT").get_float()

        vectorLIGHT=indi.get_vector(INDIDEVICE,"LIGHT")
	light=vectorLIGHT.get_element("LIGHT").get_float()
   
        statusVector=indi.get_vector(INDIDEVICE,"STATUS")
	cloudFlag=int(statusVector.get_element("clouds").is_ok())
	dewFlag=int(statusVector.get_element("dew").is_ok())
	frezzingFlag=int(statusVector.get_element("frezzing").is_ok())
  
	return (("HR",HR),("Thr",Thr),("IR",IR),("Tir",Tir),("P",P),("Tp",Tp),("Dew",dew),("Light",light),
           ("clouds",clouds),("skyT",skyT),("cloudFlag",cloudFlag),("dewFlag",dewFlag),
           ("frezzingFlag",frezzingFlag))

def recv_serial():
    ser = serial.Serial('/dev/ttyACM0')
    line = ser.readline()
    print line
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
           ("P",P),("Tp",Tp),("Light",light),("Wind",Wind),("WindMax",WindMax),("CRain",CRain),("TRain",TRain),
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
		print d[0],d[1]
		updateString=updateString+":"+str(d[1])
		json_dict[d[0]]=int(d[1]*100)/100.
        print updateString
 	ret = rrdtool.update(CHARTPATH+'meteo.rrd',updateString);
 	if ret:
 		print rrdtool.error() 
        x = simplejson.dumps(json_dict)
    fi=open(CHARTPATH+"RTdata.json","w")
	fi.write(x)
	fi.close()
	indi.quit()
    del data
	del json_dict 
	collected = gc.collect()

	time.sleep(10)
  except:
	print "UPDATER FAIL"
	time.sleep(10)



