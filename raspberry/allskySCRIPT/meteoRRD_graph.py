#!/usr/bin/python
#-*- coding: iso-8859-15 -*-
# SADR METEOLLSKY
# http://www.sadr.fr
# SEBASTIEN LECLERC 2017
# Inspired by :
# NACHO MAS 2013
# http://induino.wordpress.com 


import sys, os
import rrdtool
import time
from meteollskyconfig import *
import math

blue="0000FF"
orange="F08000"
red="FA2000"
white="AAAAAA"
green="00FFFF"
colorClear="00000A40"
colorMedium="0000AA40"
colorHigh="0000FF40"
colorFill="FFFFFF40"

preamble=["--width","600",
          "--height","300",
          "--color", "FONT#FF0000",
          "--color", "SHADEA#00000000",
          "--color", "SHADEB#00000000",
          "--color", "BACK#00000000",
          "--color", "CANVAS#00000000"]

def graphs(time):
    ret = rrdtool.graph(CHARTPATH+"temp"+str(time)+".png","--start","-"+str(time)+"h","-E",
                        preamble,
                        "--title","Temperature",
                        "--watermark","SADR OBSERVATORY",
                        "--vertical-label=Celsius ºC",
                        "DEF:T="+CHARTPATH+"meteo.rrd:T:AVERAGE",
                        "DEF:Tmax="+CHARTPATH+"meteo.rrd:T:MAX",
                        "DEF:Tmin="+CHARTPATH+"meteo.rrd:T:MIN",
                        "DEF:Dew="+CHARTPATH+"meteo.rrd:Dew:AVERAGE",
                        "LINE1:T#"+orange+":Ambient Temperature",
                        "HRULE:0#00FFFFAA:ZERO",
                        "AREA:Dew#"+orange+"40:Dew Point\\r",
                        "COMMENT:\\n",
                        "GPRINT:T:AVERAGE:Avg Temp\: %6.2lf %S\\r")

    ret = rrdtool.graph(CHARTPATH+"alltemp"+str(time)+".png","-A","--start","-"+str(time)+"h","-E",
                        preamble,
                        "--title","All Temperature sensor",
                        "--watermark","SADR OBSERVATORY",
                        "--vertical-label=Celsius ºC",
                        "DEF:IR="+CHARTPATH+"meteo.rrd:IR:AVERAGE",
                        "DEF:Thr="+CHARTPATH+"meteo.rrd:Thr:AVERAGE",
                        "DEF:Tp="+CHARTPATH+"meteo.rrd:Tp:AVERAGE",
                        "DEF:Tir="+CHARTPATH+"meteo.rrd:Tir:AVERAGE",
                        "DEF:Thrint="+CHARTPATH+"meteo.rrd:Thrint:AVERAGE",
                        "DEF:Dew="+CHARTPATH+"meteo.rrd:Dew:AVERAGE",
                        "LINE1:IR#00F0F0:IR",
                        "LINE1:Thr#00FF00:Thr",
                        "LINE1:Tp#FF0000:Tp",
                        "LINE1:Tir#0000FF:Tir",
                        "LINE1:Thrint#F4A742:TBox",
                        "HRULE:0#00FFFFAA:ZERO",
                        "AREA:Dew#F4954250:Dew\\r")

    ret = rrdtool.graph(CHARTPATH+"pressure"+str(time)+".png","-A","-X 0", "--start","-"+str(time)+"h","-E",
                        preamble,
                        "--title","Pressure",
                        "--watermark","SADR OBSERVATORY",
                        "--vertical-label=hPa",
                        "-y","10:1",
                        "-u",str(Pmax),
                        "-l",str(Pmin),
                        "-r",
                        "DEF:P="+CHARTPATH+"meteo.rrd:P:AVERAGE",
                        "HRULE:"+str(P0)+"#"+white+"AA:standard",
                        "LINE1:P#"+orange+":P\\r",
                        "GPRINT:P:AVERAGE:Avg P\: %6.2lf %S\\r")
                        
    ret = rrdtool.graph(CHARTPATH+"regul"+str(time)+".png","-A","--start","-"+str(time)+"h","-E",
                        preamble,
                        "--title","Temperature sensor",
                        "--watermark","SADR OBSERVATORY",
                        "--vertical-label=°C",
                        "-y","1:1",
                        "DEF:T="+CHARTPATH+"meteo.rrd:TRain:AVERAGE",
                        "DEF:Target="+CHARTPATH+"meteo.rrd:TargetRain:AVERAGE",
                        "LINE1:T#"+orange+":T",
                        "LINE1:Target#"+white+":Target\\r")
                        
    ret = rrdtool.graph(CHARTPATH+"PID"+str(time)+".png","-A","--start","-"+str(time)+"h","-E",
                        preamble,
                        "-u","260",
                        "-l","-5",
                        "-r",
                        "--title","Rain sensor power heating",
                        "--watermark","SADR OBSERVATORY",
                        "--vertical-label=PWM",
                        "DEF:PIDRain="+CHARTPATH+"meteo.rrd:PIDRain:AVERAGE",
                        "LINE1:PIDRain#"+green+":PID\\r")

    ret = rrdtool.graph(CHARTPATH+"hr"+str(time)+".png","--start","-"+str(time)+"h","-E",
                        preamble,
                        "-u","105",
                        "-l","-5",
                        "-r",
                        "--title","All Humidity sensor",
                        "--vertical-label=%",
                        "DEF:HR="+CHARTPATH+"meteo.rrd:HR:AVERAGE",
                        "DEF:HRint="+CHARTPATH+"meteo.rrd:HRint:AVERAGE",
                        "LINE1:HR#"+orange+":HR",
                        "LINE1:HRint#"+green+":HRBox\\r",
                        "COMMENT:\\n",
                        "GPRINT:HR:AVERAGE:Avg HR\: %6.2lf %S\\r")

    ret = rrdtool.graph(CHARTPATH+"light"+str(time)+".png","--start","-"+str(time)+"h","-E",
                        preamble,
                        "--title","Light",
                        "--watermark","SADR OBSERVATORY",
                        "--vertical-label=Volt",
                        "-u","5",
                        "-l","-0.1",
                        "-r",
                        "DEF:Light="+CHARTPATH+"meteo.rrd:Light:AVERAGE",
                        "DEF:daylightFlag="+CHARTPATH+"meteo.rrd:daylightFlag:AVERAGE",
                        "CDEF:LightV=Light,0.001,*",
                        "CDEF:luminous=LightV,daylightFlag,*",
                        "LINE1:LightV#"+orange+":Light",
                        "AREA:luminous#"+colorFill+":daylightFlag\\r",
                        "AREA:0.25#"+colorClear+":Dark",
                        "AREA:2.5#"+colorMedium+":Light:STACK",
                        "AREA:5#"+colorHigh+":Very Light:STACK")

    ret = rrdtool.graph(CHARTPATH+"clouds"+str(time)+".png","-A","--start","-"+str(time)+"h","-E",
                        preamble,
                        "--title","Clouds",
                        "--watermark","SADR OBSERVATORY",
                        "--vertical-label=%",
                        "-u","100",
                        "-l","-2",
                        "-r",
                        "DEF:clouds="+CHARTPATH+"meteo.rrd:clouds:AVERAGE",
                        "DEF:cloudFlag="+CHARTPATH+"meteo.rrd:cloudFlag:AVERAGE",
                        "CDEF:cloudy=clouds,cloudFlag,*",
                        "LINE1:clouds#"+orange+":Clouds",
                        "AREA:cloudy#"+colorFill+":CloudyFlag\\r",
                        "AREA:30#"+colorClear+":Clear",
                        "AREA:40#"+colorMedium+":Cloudy:STACK",
                        "AREA:32#"+colorHigh+":Overcast:STACK")
                        
    ret = rrdtool.graph(CHARTPATH+"wind"+str(time)+".png","-A","--start","-"+str(time)+"h","-E",
                        preamble,
                        "--title","Wind",
                        "--watermark","SADR OBSERVATORY",
                        "--vertical-label=km/h",
                        "-u","50",
                        "-l","-2",
                        "-r",
                        "DEF:Wind="+CHARTPATH+"meteo.rrd:Wind:AVERAGE",
                        "DEF:WindMax="+CHARTPATH+"meteo.rrd:WindMax:AVERAGE",
                        "DEF:windFlag="+CHARTPATH+"meteo.rrd:windFlag:AVERAGE",
                        "CDEF:windy=WindMax,windFlag,*",
                        "LINE1:Wind#"+orange+":Wind",
                        "LINE1:WindMax#"+red+":WindMax",
                        "AREA:windy#"+colorFill+":windFlag\\r",
                        "AREA:20#"+colorClear+":Calm",
                        "AREA:15#"+colorMedium+":Windy:STACK",
                        "AREA:15#"+colorHigh+":Very Windy:STACK")

    ret = rrdtool.graph(CHARTPATH+"rain"+str(time)+".png","-A","--start","-"+str(time)+"h","-E",preamble,
                        "--title","Rain",
                        "--vertical-label=Capacity",
                        "--watermark","SADR OBSERVATORY",
                        "-u","300",
                        "-l","30",
                        "-r",
                        "DEF:CRain="+CHARTPATH+"meteo.rrd:CRain:AVERAGE",
                        "DEF:rainFlag="+CHARTPATH+"meteo.rrd:rainFlag:AVERAGE",
                        "CDEF:rainy=CRain,rainFlag,*",
                        "LINE1:CRain#"+orange+":Rain",
                        "AREA:rainy#"+colorFill+":RainFlag\\r",
                        "AREA:80#"+colorClear+":Dry",
                        "AREA:80#"+colorMedium+":Wet:STACK",
                        "AREA:150#"+colorHigh+":Rain:STACK")

    ret = rrdtool.graph(CHARTPATH+"skyT"+str(time)+".png","--start","-"+str(time)+"h","-E",
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
#methode 1
#P0=840
#methode 2
P0=math.floor(1013-0.11201*ALTITUDE)
#methode 3
#P0=math.floor(1013/math.exp(-ALTITUDE/8431))
Pdelta=50
Pmin=P0-Pdelta
Pmax=P0+Pdelta
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
