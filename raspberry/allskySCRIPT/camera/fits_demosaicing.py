import numpy as np
from astropy.io import fits

import colour
import colour_demosaicing


image = fits.getdata('/media/freebox/Allsky/2_Travail/2_Raspberry/test.fits', ext=0).astype(np.float_) / 255.0
# image = colour.orient(image, '90 CW')

colour.write_image(image, '/media/freebox/Allsky/2_Travail/2_Raspberry/test.jpg')
colour.write_image(colour_demosaicing.demosaicing_CFA_Bayer_Malvar2004(image, pattern='GRBG'), '/media/freebox/Allsky/2_Travail/2_Raspberry/test_demosaiced.jpg')
