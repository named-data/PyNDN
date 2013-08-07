#! /usr/bin/env python

#
# Copyright (c) 2011, Regents of the University of California
# BSD license, See the COPYING file for more information
# Written by: Derek Kulinski <takeda@takeda.tk>
#

import sys
import pyndn
from pyndn.impl.enumeration import ndnb_enumerate
from pyndn.impl.segmenting import segmenter, Wrapper

def generate_names():
	names = ["/Hello", "/World", "/This", "/is", "/an", "/enumeration", "/example"]
	return map(lambda x: pyndn.Name(x), names)

def main(args):
	if len(sys.argv) != 2:
		usage()

	name = pyndn.Name(sys.argv[1])
	data = ndnb_enumerate(generate_names())

	key = pyndn.NDN.getDefaultKey()
	name = name.append('\xc1.E.be').appendKeyID(key).appendVersion()

	wrapper = Wrapper(name, key)
	sgmtr = segmenter(data, wrapper)

	handle = pyndn.NDN()
	for seg in sgmtr:
		handle.put(seg)

	return 0

def usage():
	print("Usage: %s <uri>" % sys.argv[0])
	sys.exit(1)

if __name__ == '__main__':
	sys.exit(main(sys.argv))
