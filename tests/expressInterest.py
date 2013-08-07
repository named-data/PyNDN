from pyndn import NDN, Name, Interest, Closure
from time import sleep

worked = False

class MyClosure(Closure):
	def upcall(self, kind, upcallInfo):
		global worked

		print("Got response")
		print(kind)
		print(upcallInfo)
		worked = True

n = Name("ndn:/ndnx/ping")

i = Interest()
closure = MyClosure()

c = NDN()
res = c.expressInterest(n, closure, i)
print(res)

#causes crashes
c.run(10)
print("Ha!")
assert(worked)
