from pyndn import NDN, _pyndn
from threading import Timer
from datetime import datetime

handle = _pyndn.create()

try:
	_pyndn.run(handle, 100)
except _pyndn.NDNError:
	pass
else:
	raise AssertionError("ndn_run() should fail when not connected")

del handle

c = NDN()
c.run(0)

def change_timeout():
	print("Changing timeout!")
	c.setRunTimeout(1000)

t = Timer(0.1, change_timeout)
t.start()

org_start = datetime.now()
while True:
	if (datetime.now() - org_start).seconds > 3:
		raise AssertionError("setRunTimeout() failed")

	start = datetime.now()
	c.run(5)
	diff = datetime.now() - start

	if diff.seconds * 1000000 + diff.microseconds > 500000:
		break
	print("working: ", diff)

del c
