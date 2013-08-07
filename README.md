PyNDN - NDN bindings for Python

This is intended to be a rather "thin" implementation, which supports Python
objects corresponding to the major NDNx entities - Interest, ContentObject, and
so on, as well as some support objects.  The C code is mostly just responsible
for marshaling data back and forth between the formats, though there are some
useful functions for key generation/access included.

These are mapped more or less directly from the NDNx wire format, and the
Python objects are, in fact, backed by a cached version of the wire format
or native c object, a Python CObject kept in self.ndn_data. Accessing the
attribute regenerates this backing CObject if necessary - those mechanics
are in the Python code.

The Interest and ContentObject objects also cache their parsed versions
as well

See the file INSTALL for build and install instructions.

Please submit any bugs or issues to the NDN-JS issue tracker:
http://redmine.named-data.net/projects/pyndn/issues

Using the Python bindings
----------------------------

All of the files are contained inside of pyndn package. To use you can call:

  import pyndn

  or

  from pyndn import NDN, Name, Key, ... and so on ...


