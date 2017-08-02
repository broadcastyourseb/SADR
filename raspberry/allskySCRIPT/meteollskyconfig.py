#-*- coding: iso-8859-15 -*-
# SADR METEOLLSKY
# http://www.sadr.fr
# SEBASTIEN LECLERC 2017
# Inspired by :
# NACHO MAS 2013
# http://induino.wordpress.com
# Config file


##### INDI RELATED #####
#To start indiserver use 'localhost'
#otherwise not start and connect remote
#indiserver
INDISERVER="localhost"
#INDISERVER="allsky.sadr"
INDIPORT="7624"
INDIDEVICE="QHY CCD QHY5LII-C-6127d"

##### ARDUINO RELATED ####
DEVICEPORT="/dev/ttyACM0"

##### SITE RELATED ####
OWNERNAME="SADR"
SITENAME="HACIENDA DES ETOILES"
ALTITUDE=0   #1540
#Visit http://weather.uwyo.edu/upperair/sounding.html
#See the sounding location close your site
SOUNDINGSTATION="07510"

##### RRD RELATED #####
#PATH TO GRAPHs
CHARTPATH="/var/www/html/CHART/"
#EUMETSAT lastimagen. Choose one from:
#http://oiswww.eumetsat.org/IPPS/html/latestImages.html
#This is nice but only work at daylight time:
#EUMETSAT_LAST="http://oiswww.eumetsat.org/IPPS/html/latestImages/EUMETSAT_MSG_RGB-naturalcolor-westernEurope.jpg"
#This show rain
#EUMETSAT_LAST="http://oiswww.eumetsat.org/IPPS/html/latestImages/EUMETSAT_MSG_MPE-westernEurope.jpg"
#and this cloud cover at IR 39. Work at night
EUMETSAT_LAST="http://oiswww.eumetsat.org/IPPS/html/latestImages/EUMETSAT_MSG_IR039E-westernEurope.jpg"
