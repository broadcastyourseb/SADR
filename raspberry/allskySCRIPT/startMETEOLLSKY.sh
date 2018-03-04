#!/bin/bash
#-*- coding: iso-8859-15 -*-
# SADR METEOLLSKY
# http://www.sadr.fr
# SEBASTIEN LECLERC 2017
# Inspired by :
# NACHO MAS 2013
# http://induino.wordpress.com 
# Startup script

source meteollskyconfig.py

./stopMETEOLLSKY.sh
if [ "$INDISERVER" = "localhost" ]
then
	killall indiserver
	killall indi_qhy_ccd
        mkfifo  /tmp/INDIFIFO
	indiserver -f /tmp/INDIFIFO &
	echo start indi_qhy_ccd >/tmp/INDIFIFO
fi
if [ -f "$CHARTPATH/meteo.rrd" ];
then
   echo "RRD file exists."
else
   echo "RRD does not exist. Creating"
   ./meteoRRD_createRRD.py
fi
./meteoRRD_updater.py &
./meteoRRD_graph.py &
./meteoRRD_MaxMinAvg.py &
./allsky_frame.py 29 1 &
#./sounding.py &
#./pushetta.py &
