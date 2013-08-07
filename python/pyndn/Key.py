#
# Copyright (c) 2011, Regents of the University of California
# BSD license, See the COPYING file for more information
# Written by: Derek Kulinski <takeda@takeda.tk>
#             Jeff Burke <jburke@ucla.edu>
#

# Fronts ndn_pkey.
from . import _pyndn
from . import Name

class Key(object):
	def __init__(self):
		self.type = None
		self.publicKeyID = None # SHA256 hash
		# pyndn
		self.ndn_data_dirty = False
		self.ndn_data_public = None  # backing pkey
		self.ndn_data_private = None # backing pkey

	def __get_ndn(self):
		pass

	def generateRSA(self, numbits):
		_pyndn.generate_RSA_key(self, numbits)

	def privateToDER(self):
		if not self.ndn_data_private:
			raise _pyndn.NDNKeyError("Key is not private")
		return _pyndn.DER_write_key(self.ndn_data_private)

	def publicToDER(self):
		return _pyndn.DER_write_key(self.ndn_data_public)

	def privateToPEM(self, filename = None):
		if not self.ndn_data_private:
			raise _pyndn.NDNKeyError("Key is not private")

		if filename:
			f = open(filename, 'w')
			_pyndn.PEM_write_key(self.ndn_data_private, file=f)
			f.close()
		else:
			return _pyndn.PEM_write_key(self.ndn_data_private)

	def publicToPEM(self, filename = None):
		if filename:
			f = open(filename, 'w')
			_pyndn.PEM_write_key(self.ndn_data_public, file=f)
			f.close()
		else:
			return _pyndn.PEM_write_key(self.ndn_data_public)

	def fromDER(self, private = None, public = None):
		if private:
			(self.ndn_data_private, self.ndn_data_public, self.publicKeyID) = \
				_pyndn.DER_read_key(private=private)
			return
		if public:
			(self.ndn_data_private, self.ndn_data_public, self.publicKeyID) = \
				_pyndn.DER_read_key(public=public)
			return

	def fromPEM(self, filename = None, private = None, public = None):
		if filename:
			f = open(filename, 'r')
			(self.ndn_data_private, self.ndn_data_public, self.publicKeyID) = \
				_pyndn.PEM_read_key(file=f)
			f.close()
		elif private:
			(self.ndn_data_private, self.ndn_data_public, self.publicKeyID) = \
				_pyndn.PEM_read_key(private=private)
		elif public:
			(self.ndn_data_private, self.ndn_data_public, self.publicKeyID) = \
				_pyndn.PEM_read_key(public=public)

# plus library helper functions to generate and serialize keys?

class KeyLocator(object):
	def __init__(self, arg=None):
		#whichever one is not none will be used
		#if multiple set, checking order is: keyName, key, certificate
		self.key = arg if type(arg) is Key else None
		self.keyName = arg if type(arg) is Name.Name else None
		self.certificate = None

		# pyndn
		self.ndn_data_dirty = True
		self.ndn_data = None  # backing charbuf

	def __setattr__(self, name, value):
		if name != "ndn_data" and name != "ndn_data_dirty":
			self.ndn_data_dirty = True
		object.__setattr__(self, name, value)

	def __getattribute__(self, name):
		if name=="ndn_data":
			if object.__getattribute__(self, 'ndn_data_dirty'):
				if object.__getattribute__(self, 'keyName'):
					self.ndn_data = _pyndn.KeyLocator_to_ndn(
						name=self.keyName.ndn_data)
				elif object.__getattribute__(self, 'key'):
					self.ndn_data = _pyndn.KeyLocator_to_ndn(
						key=self.key.ndn_data_public)
				elif object.__getattribute__(self, 'certificate'):
					#same but with cert= arg
					raise NotImplementedError("certificate support is not implemented")
				else:
					raise TypeError("No name, key nor certificate defined")

				self.ndn_data_dirty = False
		return object.__getattribute__(self, name)
