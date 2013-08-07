from pyndn import Key, Name, _pyndn, NDN

n = Name("/this/is/a/name")

ndn_name1 = n.ndn_data
name1 = _pyndn.name_comps_from_ndn(ndn_name1)

locator1 = _pyndn.KeyLocator_to_ndn(name=ndn_name1)
print(locator1)

locator1_obj = _pyndn.KeyLocator_obj_from_ndn(locator1)
print(locator1_obj)
print(locator1_obj.keyName)

name2 = _pyndn.name_comps_from_ndn(locator1_obj.keyName.ndn_data)
print(name2)

for comp1, comp2 in zip(name1, name2):
	if comp1 != comp2:
		raise AssertionError("Got a different output: '%s' != '%s'" % (comp1, comp2))

key1 = NDN.getDefaultKey()

locator2 = _pyndn.KeyLocator_to_ndn(key=key1.ndn_data_public)
print(locator2)

locator2_obj = _pyndn.KeyLocator_obj_from_ndn(locator2)
key2 = locator2_obj.key
print(key2)

print(key1.ndn_data_public)
print(key2.ndn_data_public)

assert(key1.publicToDER() == key2.publicToDER())

del key2
key2 = _pyndn.Key_obj_from_ndn(key1.ndn_data_private)

assert(key1.publicKeyID == key2.publicKeyID)
assert(key1.publicToDER() == key2.publicToDER())
assert(key1.privateToDER() == key2.privateToDER())

del key2
key2 = _pyndn.Key_obj_from_ndn(key1.ndn_data_public)

assert(key1.publicKeyID == key2.publicKeyID)
assert(key1.publicToDER() == key2.publicToDER())
assert(key2.ndn_data_private == None)
