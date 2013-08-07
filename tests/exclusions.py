import pyndn
from pyndn import _pyndn

e = pyndn.ExclusionFilter()
e.add_any()
e.add_names([pyndn.Name('/one'), pyndn.Name('/two'), pyndn.Name('/three'), pyndn.Name('/four')])
e.add_any()
e.add_name(pyndn.Name('/forty/two'))

str(e)
d = _pyndn.ExclusionFilter_obj_from_ndn(e.ndn_data)
str(d)

# I believe separation of /forty/two into /forty and /two is a correct behavior
# since it doesn't make sense to have more than one level in exclusions
result = ['<any>', '/one', '/two', '/four', '/three', '<any>', '/forty', '/two']

for a,b in zip(d.components, result):
	if str(a) == b:
		continue
	else:
		raise AssertionError("%s != %s" % (str(a), b))
