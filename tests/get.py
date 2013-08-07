from pyndn import NDN, Name

c = NDN()
print(c)

n = Name()
print(n)

n = Name("ndn:/ndnx/ping")
print(n)

co = c.get(n)
print(co)

#this shouldn't cause segfault
print(n)

n = Name("ndn:/ndnx/some_nonexisting_name")
co = c.get(n, None, 100)

#this shouldn't cause segfault!
print(co)
