#!/usr/bin/python
#-*- coding: iso-8859-15 -*-
# SADR METEOLLSKY
# http://www.sadr.fr
# SEBASTIEN LECLERC 2017
# Inspired by :
# fehlfarbe
# http://indilib.org/forum/general/606-take-image-with-python-script.html
# allsky picture script

import sys, time, logging, os
import PyIndi
import pyfits
import cv2
import cStringIO
from PIL import Image, ImageDraw, ImageFont
from datetime import datetime
import numpy as np
from meteollskyconfig import *

if len(sys.argv) <> 3:
    print "Usage: python allsky_frame.py <<time exposure>> <<gain>>"
    sys.exit()

fi=open(CHARTPATH+"luminosity.js","r")
luminosity = fi.readline()
arg = luminosity.split('=')
lumValue = arg[1]
print("Luminosity: " + lumValue)
fi.close()
if lumValue == 0:
    #EXP_TIME = 19. - (19. / 100 * float(lumValue))
    EXT_TIME = 60
    EXP_GAIN = 30
    COLOR = 0
if lumValue > 0:
    EXP_TIME = 0.000001
    EXP_GAIN = 1
    COLOR = 1
EXP_TIME = str(EXP_TIME)
EXP_GAIN = str(EXP_GAIN)
print("EXP_TIME: " + str(EXP_TIME))
print("EXP_GAIN: " + str(EXP_GAIN))
#EXP_TIME = sys.argv[1]
#EXP_GAIN = str(50)
#EXP_GAIN = sys.argv[2]
 
class IndiClient(PyIndi.BaseClient):

    device = None
    roi = None
    
    def __init__(self):
        super(IndiClient, self).__init__()
        self.logger = logging.getLogger('PyQtIndi.IndiClient')
        self.logger.info('creating an instance of PyQtIndi.IndiClient')
    def newDevice(self, d):
        #self.logger.info("new device " + d.getDeviceName())
        if d.getDeviceName() == INDIDEVICE:
            self.logger.info("Set new device %s!" % INDIDEVICE)
            # save reference to the device in member variable
            self.device = d
    def newProperty(self, p):
        #self.logger.info("new property "+ p.getName() + " for device "+ p.getDeviceName())
        if self.device is not None and p.getName() == "CONNECTION" and p.getDeviceName() == self.device.getDeviceName():
            self.logger.info("Got property CONNECTION for %s!" % INDIDEVICE)
            # connect to device
            self.connectDevice(self.device.getDeviceName())
            # set BLOB mode to BLOB_ALSO
            self.setBLOBMode(1, self.device.getDeviceName(), None)
        if p.getName() == "CCD_EXPOSURE":
            # take first exposure
            self.takeExposure()
        if p.getName() == "CCD_GAIN":
            gain = self.device.getNumber("CCD_GAIN")
            gain[0].value = float(EXP_GAIN)
            self.sendNewNumber(gain)
    def removeProperty(self, p):
        #self.logger.info("remove property "+ p.getName() + " for device "+ p.getDeviceName())
        pass
    def newBLOB(self, bp):
        print "new Blob arrived"
        self.logger.info("new BLOB "+ bp.name.decode())
        # get image data
        img = bp.getblobdata()
        # write image data to StringIO buffer
        blobfile = cStringIO.StringIO(img)
        hdulist = pyfits.open(blobfile)
        scidata = hdulist[0].data
        if self.roi is not None:
            scidata = scidata[self.roi[1]:self.roi[1]+self.roi[3], self.roi[0]:self.roi[0]+self.roi[2]]
        hdulist[0].data = scidata
        # process bayer pattern and grey image
        processedImage = cv2.cvtColor(scidata, cv2.COLOR_BAYER_GR2RGB)
        if COLOR == 0:
            processedImage = cv2.cvtColor(processedImage, cv2.COLOR_BGR2GRAY)
        cv2.imwrite(CHARTPATH+"allsky.jpg" , processedImage)
        
        ######### WATERMARK #############
        # Open the watermark image
        main = Image.open(CHARTPATH+"allsky.jpg")
        # Adding an alpha channel to the main image
        main = main.convert("RGBA")
        
        # Open the watermark image
        overlay = Image.open(WATERMARK)
        # Adding an alpha channel to the main image
        overlay = overlay.convert("RGBA")
        
        # Get an ImageDraw object so we can draw on the image
        waterdraw = ImageDraw.ImageDraw(overlay, "RGBA")
        # get a font
        fnt = ImageFont.truetype('font/Typography Times Bold.ttf', 15)
        # Place the text at (10, 10) in the upper left corner. Text will be white.
        waterdraw.text((5, 5), "%s" % datetime.now(), font=fnt, fill=(255,255,255,255))
        waterdraw.text((5, 25), "GAIN "+EXP_GAIN+" / EXP "+EXP_TIME+"s", font=fnt, fill=(255,255,255,255))
        if COLOR == 0:
            waterdraw.text((5, 45), "NO COLOR / LUM "+str(lumValue), font=fnt, fill=(255,255,255,255))
        else:
            waterdraw.text((5, 45), "COLOR / LUM "+str(lumValue), font=fnt, fill=(255,255,255,255))
        
        # Make a final composite image
        result = Image.alpha_composite(main, overlay)
        ######### END OF WATERMARK #############
        
        # Paste the watermark (with alpha layer) onto the original image and save it
        result.save(CHARTPATH+"allsky.jpg", "JPEG")
        
        # start new exposure for timelapse images!
        time.sleep(10)
        self.takeExposure()
        
    def newSwitch(self, svp):
      self.logger.info ("new Switch "+ svp.name.decode() + " for device "+ svp.device.decode())
    def newNumber(self, nvp):
        self.logger.info("new Number "+ nvp.name.decode() + " for device "+ nvp.device.decode())
    def newText(self, tvp):
        self.logger.info("new Text "+ tvp.name.decode() + " for device "+ tvp.device.decode())
    def newLight(self, lvp):
        self.logger.info("new Light "+ lvp.name.decode() + " for device "+ lvp.device.decode())
    def newMessage(self, d, m):
        #self.logger.info("new Message "+ d.messageQueue(m).decode())
        pass
    def serverConnected(self):
        print("Server connected ("+self.getHost()+":"+str(self.getPort())+")")
        self.connected = True
    def serverDisconnected(self, code):
        self.logger.info("Server disconnected (exit code = "+str(code)+","+str(self.getHost())+":"+str(self.getPort())+")")
        # set connected to False
        self.connected = False
    def takeExposure(self):
        self.logger.info("<<<<<<<< Exposure >>>>>>>>>")
        #get current exposure time
        exp = self.device.getNumber("CCD_EXPOSURE")
        # set exposure time to 5 seconds
        print ("Exposition time"+EXP_TIME+" s.")
        exp[0].value = float(EXP_TIME)
        # send new exposure time to server/device
        self.sendNewNumber(exp)
        
logging.basicConfig(format='%(asctime)s %(message)s', level=logging.INFO)

############# MAIN #############

if __name__ == '__main__':
    # instantiate the client
    indiclient=IndiClient()
    # region of interest for my allsky cam
    indiclient.roi = (206, 30, 890, 820) 
    # set indi server localhost and port 7624
    INDIPORT=int(INDIPORT)
    indiclient.setServer(INDISERVER, INDIPORT)
    # connect to indi server
    print("Connecting and waiting 2secs")
    if (not(indiclient.connectServer())):
         print("No indiserver running on "+indiclient.getHost()+":"+str(indiclient.getPort())+" - Try to run")
         sys.exit(1)
    #time.sleep(30)
     
    # start endless loop, client works asynchron in background, loop stops after disconnect
    #while indiclient.connected:
    while True:
        #indiclient.takeExposure()
        time.sleep(10)
