#
# Copyright (c) 2012, Regents of the University of California
# BSD license, See the COPYING file for more information
# Written by: Derek Kulinski <takeda@takeda.tk>
#

import pyndn
from pyndn.impl import ndnb

def ndnb_enumerate(names):
	out = bytearray()

	for name in names:
		out += ndnb.dtag(ndnb.DTAG_LINK, name.get_ndnb())

	return ndnb.dtag(ndnb.DTAG_COLLECTION, out)
