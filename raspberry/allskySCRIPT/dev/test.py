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
from meteollskyconfig import *
  
class IndiClient(PyIndi.BaseClient):
    global exposition
    exposition = 1
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
    def newNumber(self, nvp):
        self.logger.info("new Number "+ nvp.name + " for device "+ nvp.device)
    def newText(self, tvp):
        self.logger.info("new Text "+ tvp.name + " for device "+ tvp.device)
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
        if moyenne > 100:
            if gain[0].value == 1:            
                exposition = float(exposition) / 2
                if exposition < 0.000001:
                    exposition = 0.000001
                    #COLOR =1
            else :
                gain[0].value /= 2
                if gain[0].value < 1:
                    gain[0].value = 1
                    #COLOR = 1
        else:
            if exposition < 120:            
                exposition *= 2
                if exposition > 120:
                    exposition = 120
                    #COLOR = 0
            else :
                gain[0].value *= 2
                if gain[0].value > 50:
                    gain[0].value = 50
                    #COLOR = 1
        # send new gain to server/device
        if gain <> self.device.getNumber("CCD_GAIN"):
            self.sendNewNumber(gain)
        self.logger.info("Nouvelle exposition: " + str(exposition))
        self.logger.info("Nouveau gain: " + str(gain[0].value))        
  
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

