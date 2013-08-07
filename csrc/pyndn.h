/*
 * Copyright (c) 2011, Regents of the University of California
 * BSD license, See the COPYING file for more information
 * Written by: Derek Kulinski <takeda@takeda.tk>
 *             Jeff Burke <jburke@ucla.edu>
 */

#ifndef _PYNDN_H_
#  define _PYNDN_H_

#define NAMECRYPTO 1

enum e_class_type {
	NDN,
	Closure,
	ContentObject,
	ExclusionFilter,
	Interest,
	Key,
	KeyLocator,
	Name,
	Signature,
	SignedInfo,
	SigningParams,
	UpcallInfo,
	CLASS_TYPE_COUNT
};

#define MAX_RUN_STATES 5

struct pyndn_run_state {
	struct pyndn_run_state *next;
	struct ndn *handle;
};

struct pyndn_state {
	struct pyndn_run_state *run_state;
	PyObject *class_type[CLASS_TYPE_COUNT];
};

extern PyObject *_pyndn_module;

#  if PY_MAJOR_VERSION >= 3
#    define GETSTATE(m) ((struct pyndn_state *)PyModule_GetState(m))
#  else
extern struct pyndn_state _pyndn_state;
#    define GETSTATE(m) (&_pyndn_state)
#  endif

PyObject *_pyndn_get_type(enum e_class_type type);

#  define g_type_NDN              _pyndn_get_type(NDN)
#  define g_type_Closure          _pyndn_get_type(Closure)
#  define g_type_ContentObject    _pyndn_get_type(ContentObject)
#  define g_type_ExclusionFilter  _pyndn_get_type(ExclusionFilter)
#  define g_type_Interest         _pyndn_get_type(Interest)
#  define g_type_Key              _pyndn_get_type(Key)
#  define g_type_KeyLocator       _pyndn_get_type(KeyLocator)
#  define g_type_Name             _pyndn_get_type(Name)
#  define g_type_Signature        _pyndn_get_type(Signature)
#  define g_type_SignedInfo       _pyndn_get_type(SignedInfo)
#  define g_type_SigningParams    _pyndn_get_type(SigningParams)
#  define g_type_UpcallInfo       _pyndn_get_type(UpcallInfo)

extern PyObject *g_PyExc_NDNError;
extern PyObject *g_PyExc_NDNNameError;
extern PyObject *g_PyExc_NDNKeyLocatorError;
extern PyObject *g_PyExc_NDNSignatureError;
extern PyObject *g_PyExc_NDNSignedInfoError;
extern PyObject *g_PyExc_NDNInterestError;
extern PyObject *g_PyExc_NDNExclusionFilterError;
extern PyObject *g_PyExc_NDNKeyError;
extern PyObject *g_PyExc_NDNContentObjectError;

#endif /* _PYNDN_H_ */
