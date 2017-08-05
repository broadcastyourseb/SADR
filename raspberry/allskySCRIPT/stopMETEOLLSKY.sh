#!/bin/bash
#-*- coding: iso-8859-15 -*-
# INDUINO METEOSTATION
# http://induino.wordpress.com 
# 
# NACHO MAS 2013

source meteollskyconfig.py

if [ "$INDISERVER" = "localhost" ]
then
	killall indiserver
        killall indi_qhy_ccd
fi
killall meteoRRD_updater.py
killall meteoRRD_graph.py
killall sounding.py
killall meteoRRD_MaxMinAvg.py
killall allsky_frame.py
