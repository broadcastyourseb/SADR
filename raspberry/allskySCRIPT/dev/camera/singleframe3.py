import sys, time, logging
import PyIndi
import pyfits
import cv2
import cStringIO
from datetime import datetime
import numpy as np

if len(sys.argv) <> 3:
    print "Usage: python singleFrame.py <<time exposure>> <<gain>>"
    sys.exit()

DEVICE_NAME = "QHY CCD QHY5LII-C-6127d"
DEVICE_ADDRESS = "allsky.sadr"
DEVICE_PORT = 7624
EXP_TIME = sys.argv[1]
EXP_GAIN = sys.argv[2]
 
class IndiClient(PyIndi.BaseClient):

    device = None
    roi = None
    
    def __init__(self):
        super(IndiClient, self).__init__()
        self.logger = logging.getLogger('PyQtIndi.IndiClient')
        self.logger.info('creating an instance of PyQtIndi.IndiClient')
    def newDevice(self, d):
        #self.logger.info("new device " + d.getDeviceName())
        if d.getDeviceName() == DEVICE_NAME:
            self.logger.info("Set new device %s!" % DEVICE_NAME)
            # save reference to the device in member variable
            self.device = d
    def newProperty(self, p):
        #self.logger.info("new property "+ p.getName() + " for device "+ p.getDeviceName())
        if self.device is not None and p.getName() == "CONNECTION" and p.getDeviceName() == self.device.getDeviceName():
            self.logger.info("Got property CONNECTION for %s!" % DEVICE_NAME)
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
        #hdulist.writeto("output.fit")
        convertedImage = cv2.cvtColor(scidata, cv2.COLOR_BAYER_GR2RGB)
        # add some text information
        cv2.putText(convertedImage, "%s" % datetime.now(), (5, 25), cv2.FONT_HERSHEY_SIMPLEX, 0.4, (255,255,255))
        #cv2.putText(convertedImage, EXP_TIME+"s", (5, 50), cv2.FONT_HERSHEY_SIMPLEX, 0.6, (255,255,255))
        
        # load the watermark image, making sure we retain the 4th channel
        # which contains the alpha transparency
        #watermark = cv2.imread("telescope.png", cv2.IMREAD_UNCHANGED)
        #(wH, wW) = watermark.shape[:2]
        
        # split the watermark into its respective Blue, Green, Red, and
        # Alpha channels; then take the bitwise AND between all channels
        # and the Alpha channels to construct the actaul watermark
        # NOTE: I'm not sure why we have to do this, but if we don't,
        # pixels are marked as opaque when they shouldn't be
        #(B, G, R, A) = cv2.split(watermark)
        #B = cv2.bitwise_and(B, B, mask=A)
        #G = cv2.bitwise_and(G, G, mask=A)
        #R = cv2.bitwise_and(R, R, mask=A)
        #watermark = cv2.merge([B, G, R, A])
        
        # load the input image, then add an extra dimension to the
	    # image (i.e., the alpha transparency)
        #image = convertedImage.copy()
        #(h, w) = image.shape[:2]
        #image = np.dstack([image, np.ones((h, w), dtype="uint8") * 255])
        
        # construct an overlay that is the same size as the input
	    # image, (using an extra dimension for the alpha transparency),
	    # then add the watermark to the overlay in the bottom-right
	    # corner
        #overlay = np.zeros((h, w, 4), dtype="uint8")
        #overlay[h - wH - 10:h - 10, w - wW - 10:w - 10] = watermark
        
        #print "Fin des traitements"
        
        # blend the two images together using transparent overlays
        #output = image.copy()
        #cv2.addWeighted(overlay, 0.9, output, 1.0, 0, output)
        
        # write the output image to disk
        output = convertedImage.copy()
        cv2.imwrite("/var/www/html/output.png" , output)
        cv2.imwrite("output.png" , output)
        cv2.imwrite("/var/www/html/output.jpg" , output)
        cv2.imwrite("output.jpg" , output)
        sys.exit(1)
       
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

if __name__ == '__main__':
    # instantiate the client
    indiclient=IndiClient()
    indiclient.roi = (206, 30, 890, 820) # region of interest for my allsky cam
    # set indi server localhost and port 7624
    indiclient.setServer(DEVICE_ADDRESS, DEVICE_PORT)
    # connect to indi server
    print("Connecting and waiting 2secs")
    if (not(indiclient.connectServer())):
         print("No indiserver running on "+indiclient.getHost()+":"+str(indiclient.getPort())+" - Try to run")
         print("  indiserver indi_simulator_telescope indi_simulator_ccd")
         sys.exit(1)
    time.sleep(1)
    #indiclient.setBLOBMode(1, DEVICE_NAME, None)
    #time.sleep(10)
    #sys.exit(1)
     
    # start endless loop, client works asynchron in background, loop stops after disconnect
    while indiclient.connected:
    # while True:
        time.sleep(1)
