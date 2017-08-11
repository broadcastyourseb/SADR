#!/usr/bin/python
#-*- coding: iso-8859-15 -*-
# INDUINO METEOSTATION
# http://induino.wordpress.com 
# 
# NACHO MAS 2013

import sys, os
import rrdtool
import time
from meteollskyconfig import *
import math

blue="0080F0"
orange="F08000"
red="FA2000"
white="AAAAAA"

preamble=["--width","600",
         "--height","300",
         "--color", "FONT#FF0000",
	 "--color", "SHADEA#00000000",
	 "--color", "SHADEB#00000000",
	 "--color", "BACK#00000000",
	 "--color", "CANVAS#00000000"]

def graphs(time):
	ret = rrdtool.graph( CHARTPATH+"temp"+str(time)+".png","--start","-"+str(time)+"h","-E",
          preamble,
	 "--title","Temperature",
	 "--vertical-label=Celsius ºC",
	 "DEF:T="+CHARTPATH+"meteo.rrd:T:AVERAGE",
	 "DEF:Tmax="+CHARTPATH+"meteo.rrd:T:MAX",
	 "DEF:Tmin="+CHARTPATH+"meteo.rrd:T:MIN",
	 "DEF:Dew="+CHARTPATH+"meteo.rrd:Dew:AVERAGE",
	 "LINE1:T#"+red+":Ambient Temperature",
	 "HRULE:0#00FFFFAA:ZERO",
	 "AREA:Dew#"+red+"40:Dew Point\\r",
	 "COMMENT:\\n",
	 "GPRINT:T:AVERAGE:Avg Temp\: %6.2lf %S\\r")

	ret = rrdtool.graph( CHARTPATH+"alltemp"+str(time)+".png","-A","--start","-"+str(time)+"h","-E",
          preamble,
	 "--title","Temperaturas",
	 "--vertical-label=Celsius ºC",
	 "DEF:IR="+CHARTPATH+"meteo.rrd:IR:AVERAGE",
	 "DEF:Thr="+CHARTPATH+"meteo.rrd:Thr:AVERAGE",
	 "DEF:Tp="+CHARTPATH+"meteo.rrd:Tp:AVERAGE",
	 "DEF:Tir="+CHARTPATH+"meteo.rrd:Tir:AVERAGE",
   	 "DEF:Dew="+CHARTPATH+"meteo.rrd:Dew:AVERAGE",
	 "LINE1:IR#00F0F0:IR",
	 "LINE1:Thr#00FF00:Thr",
	 "LINE1:Tp#FF0000:Tp",
	 "LINE1:Tir#0000FF:Tir",
	 "HRULE:0#00FFFFAA:ZERO",
	 "AREA:Dew#00008F10:Dew\\r")


	ret = rrdtool.graph( CHARTPATH+"pressure"+str(time)+".png","-A","--start","-"+str(time)+"h","-E",
          preamble,
	 "--title","Pressure",
	 "--vertical-label=hPa",
	 "-u",str(Pmax),
	 "-l",str(Pmin),
	 "-r",
	 "DEF:P="+CHARTPATH+"meteo.rrd:P:AVERAGE",
	 "HRULE:"+str(P0)+"#"+red+"AA:standard",
	 "LINE1:P#"+blue+":P\\r",
	 "COMMENT:\\n",
	 "GPRINT:P:AVERAGE:Avg P\: %6.2lf %S\\r")

	ret = rrdtool.graph( CHARTPATH+"hr"+str(time)+".png","--start","-"+str(time)+"h","-E",
          preamble,
	 "-u","105",
	 "-l","-5",
	 "-r",
	 "--title","Humidity",
	 "--vertical-label=%",
	 "DEF:HR="+CHARTPATH+"meteo.rrd:HR:AVERAGE",
	 "HRULE:100#FF00FFAA:100%",
	 "HRULE:0#00FFFFAA:0%",
	 "LINE1:HR#"+blue+":HR\\r",
	 "COMMENT:\\n",
	 "GPRINT:HR:AVERAGE:Avg HR\: %6.2lf %S\\r")

	ret = rrdtool.graph( CHARTPATH+"light"+str(time)+".png","--start","-"+str(time)+"h","-E",
          preamble,
	 "--title","Iradiance",
	 "--vertical-label=rel",
	 "DEF:Light="+CHARTPATH+"meteo.rrd:Light:AVERAGE",
	 "LINE1:Light#"+blue+":Irradiance\\r",
	 "COMMENT:\\n",
	 "GPRINT:Light:AVERAGE:Avg Light\: %6.2lf %S\\r")

	ret = rrdtool.graph( CHARTPATH+"clouds"+str(time)+".png","-A","--start","-"+str(time)+"h","-E",
          preamble,
	 "--title","Clouds",
	 "--vertical-label=%",
	 "-u","102",
	 "-l","-2",
	 "-r",
	 "DEF:clouds="+CHARTPATH+"meteo.rrd:clouds:AVERAGE",
	 "DEF:cloudFlag="+CHARTPATH+"meteo.rrd:cloudFlag:AVERAGE",
	 "CDEF:cloudy=clouds,cloudFlag,*",
	 "LINE1:clouds#"+orange+":clouds",
	 "AREA:cloudy#FFFFFF40:CloudyFlag\\r",
	 "AREA:30#00000a40:Clear",
	 "AREA:40#0000AA40:Cloudy:STACK",
	 "AREA:32#0000FF40:Overcast:STACK")
    
    ret = rrdtool.graph( CHARTPATH+"rain"+str(time)+".png","-A","--start","-"+str(time)+"h","-E",preamble,
	 "--title","Rain",
	 "--vertical-label=%",
	 "-u","200",
	 "-l","0",
	 "-r",
	 "DEF:CRain="+CHARTPATH+"meteo.rrd:CRain:AVERAGE",
	 "DEF:rainFlag="+CHARTPATH+"meteo.rrd:rainFlag:AVERAGE",
	 "CDEF:rainy=CRain,rainFlag,*",
	 "LINE1:CRain#"+orange+":CRain",
	 "AREA:rainy#FFFFFF40:rainFlag\\r",
	 "AREA:30#00000a40:Clear",
	 "AREA:40#0000AA40:Rainy:STACK",
	 "AREA:32#0000FF40:Overcast:STACK")

	ret = rrdtool.graph( CHARTPATH+"skyT"+str(time)+".png","--start","-"+str(time)+"h","-E",
          preamble,
	 "--title","Sky Temperatures",
	 "--vertical-label=Celsius ºC",
	 "DEF:skyT="+CHARTPATH+"meteo.rrd:skyT:AVERAGE",
	 "DEF:IR="+CHARTPATH+"meteo.rrd:IR:AVERAGE",
	 "DEF:Thr="+CHARTPATH+"meteo.rrd:Thr:AVERAGE",
	 "CDEF:Tc=IR,skyT,-",
	 "LINE1:skyT#"+blue+":Corrected Sky T",
	 "LINE1:IR#"+orange+":Actual Sky T",
	 "LINE1:Thr#"+red+":Ambient T",
	 "LINE1:Tc#"+white+":Correction",
	 "HRULE:0#00FFFFAA:ZERO",
	 "COMMENT:\\n",
	 "GPRINT:skyT:AVERAGE:Avg Sky Temp\: %6.2lf %S\\r")

P0=math.floor(1013.25/math.exp(-ALTITUDE/8431.))
Pdelta=25
Pmin=950#P0-Pdelta
Pmax=1050#P0+Pdelta
i=0
print "Starting GRAPHER"
while (True):
	graphs(3)
	print "Generating 3 hours graph"
	if (i % 4 == 0):
		print "Generating day graph"
		graphs(24)
	if (i % 21 == 0):
		print "Generating weekly graph"
		graphs(168)
	if (i % 147 == 0):
		print "Generating monthly graph"
		graphs(1176)
        i=i+1
	time.sleep(60)
