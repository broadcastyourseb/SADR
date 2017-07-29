import numpy as np
import cv2
import pyfits

# We load the fits file
hdulist = pyfits.open('/home/seb/Telechargements/test/test.fits')
scidata = hdulist[0].data

# We demosaic input image
convertedImage = cv2.cvtColor(scidata, cv2.COLOR_BAYER_GR2RGB)
#convertedImage8 = (convertedImage16/256).astype('uint8')

# Display the resulting frame
cv2.imshow('image', convertedImage)
cv2.waitKey(0)
cv2.destroyAllWindows()

#print "test.png saved"

#cv2.imwrite("/home/seb/Telechargements/test/output.jpg" , img)

