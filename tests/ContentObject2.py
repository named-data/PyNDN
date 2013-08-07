import pyndn
import pyndn._pyndn as _pyndn

k = pyndn.NDN.getDefaultKey()

kl = pyndn.KeyLocator(k)

i = pyndn.Interest()
i.name = pyndn.Name('/chat')
i.minSuffixComponents = 3
i.maxSuffixComponents = 3
i.childSelector = 1

co = pyndn.ContentObject()
co.name = pyndn.Name('/chat/%FD%04%E6%93.%18K/%00')
co.content = "number 0"
co.signedInfo.publisherPublicKeyDigest = k.publicKeyID
co.signedInfo.finalBlockID = b'\x00'
co.sign(k)

print(str(co))

co2 = _pyndn.ContentObject_obj_from_ndn(co.ndn_data)
print(str(co2))

print(str(i))

print(co.matchesInterest(i))
