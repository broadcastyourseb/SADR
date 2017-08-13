#!/usr/bin/python
#-*- coding: iso-8859-15 -*-
# SADR METEOLLSKY
# http://www.sadr.fr
# SEBASTIEN LECLERC 2017
 
from meteoconfig import *
from pushetta import Pushetta

if __name__=='__main__':
    print "Starting pushing alarm notification"
    p=Pushetta(API_KEY)
    
    try:
        print "Waiting for alarm"
    except:
        print "Error with pushetta python"
    exit(0)

    while (True):

    

    try:
        #p.pushMessage(CHANNEL_NAME, "Hello World")
    except:
	    print "Fail to send push message"
    time.sleep(10)
