#
# Copyright (c) 2011, Regents of the University of California
# BSD license, See the COPYING file for more information
# Written by: Derek Kulinski <takeda@takeda.tk>
# Updated by: Wentao Shang <wentao@cs.ucla.edu>
#

from . import _pyndn, Name

def new_state():
	return _pyndn.nc_new_state()

def generate_application_key(fixture_key, app_name):
	app_id = _pyndn.nc_app_id(app_name)
	app_key = _pyndn.nc_app_key(fixture_key, app_id)
	return app_key

def authenticate_command(state, name, app_name, app_key):
	signed_name = _pyndn.nc_authenticate_command(state, name.ndn_data, app_name, app_key)
	return Name(ndn_data = signed_name)

def authenticate_command_sig(state, name, app_name, key):
	signed_name = _pyndn.nc_authenticate_command_sig(state, name.ndn_data, app_name, key.ndn_data_private)
	return Name(ndn_data = signed_name)

def verify_command(state, name, max_time, **args):
	if args.has_key('pub_key'): # TODO: use magic bytes to detect signature type, instead of asking caller to explicitly specify key type
		args['pub_key'] = args['pub_key'].ndn_data_public
	return _pyndn.nc_verify_command(state, name.ndn_data, max_time, **args)
