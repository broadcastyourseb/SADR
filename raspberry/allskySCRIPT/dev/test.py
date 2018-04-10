#!/usr/bin/python
#-*- coding: iso-8859-15 -*-
# SADR METEOLLSKY
# http://www.sadr.fr
# SEBASTIEN LECLERC 2018
# Inspired by Marcus Degenkolbe
# http://indilib.org/develop/tutorials/151-time-lapse-astrophotography-with-indi-python.html
# allsky frame script

import sys, time, logging
import PyIndi
import pyfits
import cv2
from meteollskyconfig import *
  
class IndiClient(PyIndi.BaseClient):
    global exposition
    exposition = 0.000001
    global USB_TRAFFIC
    USB_TRAFFIC=0
    global USB_SPEED
    USB_SPEED=0
    def __init__(self):
        super(IndiClient, self).__init__()
        self.logger = logging.getLogger('PyQtIndi.IndiClient')
        self.logger.info('creating an instance of PyQtIndi.IndiClient')
    def newDevice(self, d):
        self.logger.info("new device " + d.getDeviceName())
        if d.getDeviceName() == INDIDEVICE:
            self.logger.info("Set new device %s!" % INDIDEVICE)
            # save reference to the device in member variable
            self.device = d
    def newProperty(self, p):
        self.logger.info("new property "+ p.getName() + " for device "+ p.getDeviceName())
        if self.device is not None and p.getName() == "CONNECTION" and p.getDeviceName() == self.device.getDeviceName():
            self.logger.info("Got property CONNECTION for %s!" % INDIDEVICE)
            # connect to device
            self.connectDevice(self.device.getDeviceName())
            # set BLOB mode to BLOB_ALSO
            self.setBLOBMode(1, self.device.getDeviceName(), None)
        if p.getName() == "CCD_EXPOSURE":
            # take first exposure
            self.takeExposure()
        if p.getName() == "USB_TRAFFIC":
            traffic = self.device.getNumber("USB_TRAFFIC")
            print ("USB Traffic: "+str(traffic[0].value))
            if traffic[0].value <> 0:
                traffic[0].value = 0
                self.sendNewNumber(traffic)
        if p.getName() == "USB_SPEED":
            speed = self.device.getNumber("USB_SPEED")
            print ("USB Speed: "+str(speed[0].value))
            if speed[0].value <> 0:
                speed[0].value = 0
                self.sendNewNumber(speed)
    def removeProperty(self, p):
        self.logger.info("remove property "+ p.getName() + " for device "+ p.getDeviceName())
    def newBLOB(self, bp):
        self.logger.info("new BLOB "+ bp.name)
        # get image data
        img = bp.getblobdata()
        # write image data to BytesIO buffer
        import io
        blobfile = io.BytesIO(img)
        # open a file and save buffer to disk
        with open(CHARTPATH+"dev/frame.fits", "wb") as f:
            f.write(blobfile.getvalue())
        self.imageProcessing()
        self.takeExposure()
    def newSwitch(self, svp):
        self.logger.info ("new Switch "+ svp.name + " for device "+ svp.device)
        self.logger.info ("label "+ svp.label)
        self.logger.info ("state "+ str(svp.s))
    def newNumber(self, nvp):
        self.logger.info("new Number "+ nvp.name + " for device "+ nvp.device)
    def newText(self, tvp):
        self.logger.info("new Text "+ tvp.name + " for device "+ tvp.device)
        self.logger.info("label "+ tvp.label)
    def newLight(self, lvp):
        self.logger.info("new Light "+ lvp.name + " for device "+ lvp.device)
    def newMessage(self, d, m):
        #self.logger.info("new Message "+ d.messageQueue(m))
        pass
    def serverConnected(self):
        print("Server connected ("+self.getHost()+":"+str(self.getPort())+")")
    def serverDisconnected(self, code):
        self.logger.info("Server disconnected (exit code = "+str(code)+","+str(self.getHost())+":"+str(self.getPort())+")")
    def takeExposure(self):
        self.logger.info(">>>>>>>>")
        #get current exposure time
        exp = self.device.getNumber("CCD_EXPOSURE")
        # set exposure time to 5 seconds
        exp[0].value = exposition
        # send new exposure time to server/device
        #time.sleep(1000)
        self.sendNewNumber(exp)
    def imageProcessing(self):
        global exposition  
        self.logger.info("<<<<<<<< Image processing >>>>>>>>>")
        hdulist = pyfits.open(CHARTPATH+"dev/frame.fits")
        scidata = hdulist[0].data
        moyenne = scidata.mean()
        self.logger.info("Moyenne: " + str(moyenne))
        self.logger.info("Ancienne exposition: " + str(exposition))
        gain = self.device.getNumber("CCD_GAIN")
        self.logger.info("Ancien gain: " + str(gain[0].value))
        #if moyenne > 120:
        #    if gain[0].value == 1:            
        #        exposition = float(exposition) / 10
        #        if exposition < 0.000001:
        #            exposition = 0.000001
        #            #COLOR =1
        #    else :
        #        gain[0].value -= 5
        #        if gain[0].value < 1:
        #            gain[0].value = 1
        #            #COLOR = 1
        #elif moyenne < 100:
        #    if exposition < 120:            
        #        exposition *= 10
        #        if exposition > 120:
        #            exposition = 120
        #            #COLOR = 0
        #    else :
        #        gain[0].value += 5
        #        if gain[0].value > 50:
        #            gain[0].value = 50
        #            #COLOR = 1
        # send new gain to server/device
        self.logger.info("Nouvelle exposition: " + str(exposition))
        self.logger.info("Nouveau gain: " + str(gain[0].value))  
        if gain <> self.device.getNumber("CCD_GAIN"):
            self.sendNewNumber(gain)
        # on passe l'image en noir et blanc
        processedImage = cv2.cvtColor(scidata, cv2.COLOR_BAYER_GR2RGB)   
        cv2.imwrite(CHARTPATH+"dev/allsky.jpg" , processedImage)
  
logging.basicConfig(format='%(asctime)s %(message)s', level=logging.INFO)
 
# instantiate the client
indiclient=IndiClient()
# set indi server localhost and port 7624
indiclient.setServer("allsky.sadr",7624)
# connect to indi server
print("Connecting to indiserver")
if (not(indiclient.connectServer())):
     print("No indiserver running on "+indiclient.getHost()+":"+str(indiclient.getPort())+" - Try to run")
     print("  indiserver indi_simulator_telescope indi_simulator_ccd")
     sys.exit(1)
  
# start endless loop, client works asynchron in background
while True:
    time.sleep(1)

