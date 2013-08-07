#
# Copyright (c) 2011, Regents of the University of California
# BSD license, See the COPYING file for more information
# Written by: Derek Kulinski <takeda@takeda.tk>
#             Jeff Burke <jburke@ucla.edu>
#

__all__ = ['NDN', 'Closure', 'ContentObject', 'Interest', 'Key', 'Name']

import sys as _sys

try:
	from pyndn.NDN import *
	from pyndn.Closure import *
	from pyndn.ContentObject import *
	from pyndn.Interest import *
	from pyndn.Key import *
	from pyndn.Name import *
	from pyndn import NameCrypto
except ImportError:
	del _sys.modules[__name__]
	raise

#def name_compatibility():
#	global _name_immutable
#
#	_name_immutable = 1

