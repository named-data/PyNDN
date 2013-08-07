import pyndn

k = pyndn.NDN.getDefaultKey()

co = pyndn.ContentObject()
try:
	co.sign(k)
except AttributeError:
	pass
else:
	raise AssertionError("this should fail!")

co.name = pyndn.Name("/foo/foo")
co.signedInfo = pyndn.SignedInfo()
co.signedInfo.publisherPublicKeyDigest = k.publicKeyID
co.signedInfo.type = pyndn.CONTENT_DATA
co.signedInfo.freshness = -1
co.content = "hello!"
co.sign(k)

