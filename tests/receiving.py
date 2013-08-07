import pyndn
from subprocess import Popen, PIPE
import threading
import sys

def arrgh(x):
	if sys.version_info.major >= 3:
		return bytes(x, "ascii")
	else:
		return bytes(x)

class sendMessage(threading.Thread):
	def run(self):
		po = Popen(['ndnput', '-x', '5', '-t', 'ENCR', 'ndn:/messages/hello'], stdin=PIPE)
		po.communicate(arrgh("Hello everyone"))
#		po.stdin.close()
		po.wait()

thread = sendMessage()

name = pyndn.Name("ndn:/messages/hello")
handle = pyndn.NDN()

thread.start()
co = handle.get(name)
thread.join()

print(co)
print(co.content)
print(type(co.content))

assert co.content == bytearray(b"Hello everyone")
print(co.name)
assert str(co.name) == "/messages/hello"

signedinfo = co.signedInfo
assert signedinfo.type == pyndn.CONTENT_ENCR

signature = co.signature

print(signedinfo)
