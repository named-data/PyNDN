/*
 * Copyright (c) 2011, Regents of the University of California
 * BSD license, See the COPYING file for more information
 * Written by: Derek Kulinski <takeda@takeda.tk>
 *             Jeff Burke <jburke@ucla.edu>
 */

#include "python_hdr.h"

#include <ndn/ndn.h>
#include <ndn/hashtb.h>
#include <ndn/uri.h>
#include <ndn/signing.h>

#include "pyndn.h"
#include "util.h"
#include "key_utils.h"
#include "methods.h"
#include "methods_contentobject.h"
#include "methods_handle.h"
#include "methods_interest.h"
#include "methods_key.h"
#include "methods_name.h"
#include "methods_signature.h"
#include "methods_signedinfo.h"

#ifdef NAMECRYPTO
#    include "methods_namecrypto.h"
#endif

#if PY_MAJOR_VERSION >= 3
#    define INITERROR return NULL
#    define MODINIT(name) \
            PyMODINIT_FUNC \
            PyInit_ ## name(void)
#else
struct pyndn_state _pyndn_state;
#    define INITERROR return
#    define MODINIT(name) \
            PyMODINIT_FUNC \
            init ## name(void)
#endif

MODINIT(_pyndn);

PyObject *_pyndn_module;

// Exceptions
PyObject *g_PyExc_NDNError;
PyObject *g_PyExc_NDNNameError;
PyObject *g_PyExc_NDNKeyLocatorError;
PyObject *g_PyExc_NDNSignatureError;
PyObject *g_PyExc_NDNSignedInfoError;
PyObject *g_PyExc_NDNInterestError;
PyObject *g_PyExc_NDNExclusionFilterError;
PyObject *g_PyExc_NDNKeyError;
PyObject *g_PyExc_NDNContentObjectError;

static PyMethodDef g_module_methods[] = {
	{"create", _pyndn_cmd_create, METH_NOARGS, NULL},
	{"connect", _pyndn_cmd_connect, METH_O, NULL},
	{"disconnect", _pyndn_cmd_disconnect, METH_O, NULL},
	{"get_connection_fd", _pyndn_get_connection_fd, METH_O, NULL},
	{"process_scheduled_operations", _pyndn_cmd_process_scheduled_operations,
		METH_O, NULL},
	{"output_is_pending", _pyndn_cmd_output_is_pending, METH_O, NULL},
	{"run", _pyndn_cmd_run, METH_VARARGS, NULL},
	{"set_run_timeout", _pyndn_cmd_set_run_timeout, METH_VARARGS, NULL},
	{"is_run_executing", _pyndn_cmd_is_run_executing, METH_O, NULL},
	{"express_interest", _pyndn_cmd_express_interest, METH_VARARGS, NULL},
	{"set_interest_filter", _pyndn_cmd_set_interest_filter, METH_VARARGS, NULL},
	{"get", _pyndn_cmd_get, METH_VARARGS, NULL},
	{"put", _pyndn_cmd_put, METH_VARARGS, NULL},
	{"get_default_key", _pyndn_cmd_get_default_key, METH_NOARGS, NULL},
	{"generate_RSA_key", _pyndn_cmd_generate_RSA_key, METH_VARARGS, NULL},
	{"PEM_read_key", (PyCFunction) _pyndn_cmd_PEM_read_key,
		METH_VARARGS | METH_KEYWORDS, NULL},
	{"PEM_write_key", (PyCFunction) _pyndn_cmd_PEM_write_key,
		METH_VARARGS | METH_KEYWORDS, NULL},
	{"DER_read_key", (PyCFunction) _pyndn_cmd_DER_read_key,
		METH_VARARGS | METH_KEYWORDS, NULL},
	{"DER_write_key", (PyCFunction) _pyndn_cmd_DER_write_key,
		METH_VARARGS | METH_KEYWORDS, NULL},

	// ** Methods of ContentObject
	//
	{"content_to_bytearray", _pyndn_cmd_content_to_bytearray, METH_O, NULL},
	{"content_to_bytes", _pyndn_cmd_content_to_bytes, METH_O, NULL},
	{"verify_content", _pyndn_cmd_verify_content, METH_VARARGS, NULL},
	{"verify_signature", _pyndn_cmd_verify_signature, METH_VARARGS, NULL},
#if 0
	{"_pyndn_ndn_chk_signing_params", _pyndn_ndn_chk_signing_params, METH_VARARGS,
		""},
	{"_pyndn_ndn_signed_info_create", _pyndn_ndn_signed_info_create, METH_VARARGS,
		""},
#endif

	// Naming
	{"name_from_uri", _pyndn_cmd_name_from_uri, METH_O, NULL},
	{"name_to_uri", _pyndn_cmd_name_to_uri, METH_O, NULL},
	{"compare_names", _pyndn_cmd_compare_names, METH_VARARGS, NULL},

#if 0
	{"_pyndn_ndn_name_append_nonce", _pyndn_ndn_name_append_nonce, METH_VARARGS,
		""},
#endif

	// Converters
	{"name_comps_to_ndn", _pyndn_cmd_name_comps_to_ndn, METH_O, NULL},
	{"name_comps_from_ndn", _pyndn_cmd_name_comps_from_ndn, METH_O, NULL},
	{"Interest_obj_to_ndn", _pyndn_cmd_Interest_obj_to_ndn, METH_O, NULL},
	{"Interest_obj_from_ndn", _pyndn_cmd_Interest_obj_from_ndn, METH_O, NULL},
	{"encode_ContentObject", _pyndn_cmd_encode_ContentObject, METH_VARARGS,
		NULL},
	{"ContentObject_obj_from_ndn", _pyndn_cmd_ContentObject_obj_from_ndn,
		METH_O, NULL},
	{"digest_contentobject", _pyndn_cmd_digest_contentobject, METH_VARARGS,
		NULL},
	{"content_matches_interest", _pyndn_cmd_content_matches_interest,
		METH_VARARGS, NULL},
	{"Key_obj_from_ndn", _pyndn_cmd_Key_obj_from_ndn, METH_O, NULL},
	{"KeyLocator_to_ndn", (PyCFunction) _pyndn_cmd_KeyLocator_to_ndn,
		METH_VARARGS | METH_KEYWORDS, NULL},
	{"KeyLocator_obj_from_ndn", _pyndn_cmd_KeyLocator_obj_from_ndn, METH_O,
		NULL},
	{"Signature_obj_to_ndn", _pyndn_cmd_Signature_obj_to_ndn, METH_O, NULL},
	{"Signature_obj_from_ndn", _pyndn_cmd_Signature_obj_from_ndn, METH_O, NULL},
	{"SignedInfo_to_ndn", (PyCFunction) _pyndn_cmd_SignedInfo_to_ndn,
		METH_VARARGS | METH_KEYWORDS, NULL},
	{"SignedInfo_obj_from_ndn", _pyndn_cmd_SignedInfo_obj_from_ndn, METH_O,
		NULL},
#if 0
	{"_pyndn_SigningParams_to_ndn", _pyndn_SigningParams_to_ndn, METH_VARARGS,
		""},
	{"_pyndn_SigningParams_from_ndn", _pyndn_SigningParams_from_ndn, METH_O, NULL},
#endif
	{"ExclusionFilter_names_to_ndn", _pyndn_cmd_ExclusionFilter_names_to_ndn,
		METH_O, NULL},
	{"ExclusionFilter_obj_from_ndn", _pyndn_cmd_ExclusionFilter_obj_from_ndn,
		METH_O, NULL},
	{"dump_charbuf", _pyndn_cmd_dump_charbuf, METH_O, NULL},
	{"new_charbuf", _pyndn_cmd_new_charbuf, METH_VARARGS, NULL},

#ifdef NAMECRYPTO
	{"nc_new_state", _pyndn_cmd_nc_new_state, METH_NOARGS, NULL},
	{"nc_authenticate_command", _pyndn_cmd_nc_authenticate_command, METH_VARARGS,
		NULL},
	{"nc_authenticate_command_sig", _pyndn_cmd_nc_authenticate_command_sig,
		METH_VARARGS, NULL},
	{"nc_verify_command", (PyCFunction) _pyndn_cmd_nc_verify_command,
		METH_VARARGS | METH_KEYWORDS, NULL},
	{"nc_app_id", _pyndn_cmd_nc_app_id, METH_O, NULL},
	{"nc_app_key", _pyndn_cmd_nc_app_key, METH_VARARGS, NULL},
#endif
	{NULL, NULL, 0, NULL} /* Sentinel */
};

#if PY_MAJOR_VERSION >= 3
static struct PyModuleDef g_moduledef = {
	PyModuleDef_HEAD_INIT,
	"pyndn._pyndn",
	NULL,
	sizeof(struct pyndn_state),
	g_module_methods,
	NULL,
	NULL,
	NULL,
	NULL
};
#endif

#define NEW_EXCEPTION(NAME, DESC, BASE) \
do { \
	g_PyExc_ ## NAME = \
		PyErr_NewExceptionWithDoc("_pyndn." #NAME, DESC, BASE, NULL); \
	Py_INCREF(g_PyExc_ ## NAME); /* PyModule_AddObject steals reference */ \
	PyModule_AddObject(_pyndn_module, #NAME, g_PyExc_ ## NAME); \
} while(0)

static int
initialize_exceptions(void)
{
	NEW_EXCEPTION(NDNError, "General NDN Exception", NULL);
	NEW_EXCEPTION(NDNNameError, "NDN Name Exception", g_PyExc_NDNError);
	NEW_EXCEPTION(NDNKeyLocatorError, "NDN KeyLocator Exception",
			g_PyExc_NDNError);
	NEW_EXCEPTION(NDNSignatureError, "NDN Signature Exception",
			g_PyExc_NDNError);
	NEW_EXCEPTION(NDNSignedInfoError, "NDN SignedInfo Exception",
			g_PyExc_NDNError);
	NEW_EXCEPTION(NDNInterestError, "NDN Interest Exception",
			g_PyExc_NDNError);
	NEW_EXCEPTION(NDNExclusionFilterError, "NDN ExclusionFilter Exception",
			g_PyExc_NDNInterestError);
	NEW_EXCEPTION(NDNKeyError, "NDN Key Exception", g_PyExc_NDNKeyError);
	NEW_EXCEPTION(NDNContentObjectError, "NDN ContentObject Error",
			g_PyExc_NDNError);

	return 0;
}

PyObject *
_pyndn_get_type(enum e_class_type type)
{
	PyObject *py_module, *py_dict, *py_type;
	struct pyndn_state *state;

	static struct modules {
		enum e_class_type type;
		const char *module;
		const char *class;
	} modules[] = {
		{NDN, "pyndn.NDN", "NDN"},
		{Closure, "pyndn.Closure", "Closure"},
		{ContentObject, "pyndn.ContentObject", "ContentObject"},
		{ExclusionFilter, "pyndn.Interest", "ExclusionFilter"},
		{Interest, "pyndn.Interest", "Interest"},
		{Key, "pyndn.Key", "Key"},
		{KeyLocator, "pyndn.Key", "KeyLocator"},
		{Name, "pyndn.Name", "Name"},
		{Signature, "pyndn.ContentObject", "Signature"},
		{SignedInfo, "pyndn.ContentObject", "SignedInfo"},
		{SigningParams, "pyndn.ContentObject", "SigningParams"},
		{UpcallInfo, "pyndn.Closure", "UpcallInfo"},
		{CLASS_TYPE_COUNT, NULL, NULL}
	};
	struct modules *p;

	assert(_pyndn_module);

	state = GETSTATE(_pyndn_module);
	assert(state);

	p = &modules[type];
	assert(p->type == type);

	if (state->class_type[type])
		return state->class_type[type];

	py_module = PyImport_ImportModule(p->module);
	if (!py_module)
		return NULL;

	py_dict = PyModule_GetDict(py_module);
	assert(py_dict);

	py_type = PyDict_GetItemString(py_dict, p->class);
	if (!py_type) {
		PyErr_Format(PyExc_SystemError, "Error obtaining type for %s [%d]",
				p->class, type);
		return NULL;
	}

	Py_INCREF(py_type);
	state->class_type[type] = py_type;

	debug("Successfully obtained type for %s [%d]\n", p->class, type);
	return py_type;
}

MODINIT(_pyndn)
{
#if PY_MAJOR_VERSION >= 3
	_pyndn_module = PyModule_Create(&g_moduledef);
#else
	_pyndn_module = Py_InitModule("pyndn._pyndn", g_module_methods);
#endif
	if (!_pyndn_module) {
		fprintf(stderr, "Unable to initialize PyNDN module\n");
		INITERROR;
	}

	initialize_exceptions();

	initialize_crypto();

#if PY_MAJOR_VERSION >= 3
	return _pyndn_module;
#else
	return;
#endif
}
