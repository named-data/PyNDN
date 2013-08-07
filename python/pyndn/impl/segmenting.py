#
# Copyright (c) 2012, Regents of the University of California
# BSD license, See the COPYING file for more information
# Written by: Derek Kulinski <takeda@takeda.tk>
#

import math
import pyndn

class Wrapper(object):
	def __init__(self, name, key):
		self.name = name
		self.key = key

		kl = pyndn.KeyLocator(key)
		self.signed_info = pyndn.SignedInfo(key_locator = kl, key_digest = key.publicKeyID)

	def __call__(self, chunk, segment, segments):
		name = self.name + pyndn.Name.num2seg(segment)
		self.signed_info.finalBlockID = pyndn.Name.num2seg(segments - 1)

		co = pyndn.ContentObject(name = name, content = chunk, signed_info = self.signed_info)
		co.sign(self.key)

		return co

def segmenter(data, wrapper = None, chunk_size = 4096):
	segment = 0
	segments = math.ceil(len(data) / float(chunk_size))

	while segment < segments:
		start = segment * chunk_size
		end = min(start + chunk_size, len(data))
		chunk = data[start : end]

		if wrapper is not None:
			chunk = wrapper(chunk, segment, segments)

		yield chunk

		segment += 1

