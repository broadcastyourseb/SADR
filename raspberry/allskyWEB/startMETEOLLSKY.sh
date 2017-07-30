#!/bin/bash
#-*- coding: iso-8859-15 -*-
# SADR METEOLLSKY
# http://induino.wordpress.com 
# SEBASTIEN LECLERC 2017
# Inspired by :
# NACHO MAS 2013
# Startup script

source meteollskyconfig.py

./stopMETEALLSKY.sh
if [ "$INDISERVER" = "localhost" ]
then
	killall indiserver
	mkfifo  /tmp/INDIFIFO
	indiserver -f /tmp/INDIFIFO &
	#echo start indi_duino -n \"MeteoStation\" -s \"/usr/local/share/indi/meteostation_sk.xml\" >/tmp/INDIFIFO
fi
if [ -f "meteo.rrd" ];
then
   echo "RRD file exists."
else
   echo "RRD file exists does not exist. Creating"
   ./meteoRRD_createRRD.py
fi
./meteoRRD_updater.py &
./meteoRRD_graph.py &
./sounding.py &
./meteoRRD_MaxMinAvg.py &
#./allskyFRAME.py&

