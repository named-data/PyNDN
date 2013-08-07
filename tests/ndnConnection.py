from pyndn import NDN, _pyndn

handle = _pyndn.create()

#this should fail
try:
	_pyndn.disconnect(handle)
except _pyndn.NDNError:
	pass
else:
	raise AssertionError("Closing an unopened connection should fail")

_pyndn.connect(handle)
_pyndn.disconnect(handle)

try:
	_pyndn.disconnect(handle)
except _pyndn.NDNError:
	pass
else:
	raise AssertionError("Closing handle twice shouldn't work")

del handle

c = NDN()
_pyndn.disconnect(c.ndn_data)
del c
