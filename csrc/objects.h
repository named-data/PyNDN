/*
 * Copyright (c) 2011, Regents of the University of California
 * BSD license, See the COPYING file for more information
 * Written by: Derek Kulinski <takeda@takeda.tk>
 *             Jeff Burke <jburke@ucla.edu>
 */

#ifndef OBJECTS_H
#  define	OBJECTS_H

#  if 0

struct completed_closure {
	PyObject *closure;
	struct completed_closure *next;
};
#  endif

enum _pyndn_capsules {
	CLOSURE = 1,
	CONTENT_OBJECT,
	EXCLUSION_FILTER,
	HANDLE,
	INTEREST,
	KEY_LOCATOR,
	NAME,
	PKEY_PRIV,
	PKEY_PUB,
	SIGNATURE,
	SIGNED_INFO,
	SIGNING_PARAMS,
#  ifdef NAMECRYPTO
	NAMECRYPTO_STATE,
#  endif
};

struct content_object_data {
	struct ndn_parsed_ContentObject *pco;
	struct ndn_indexbuf *comps;
};

struct interest_data {
	struct ndn_parsed_interest *pi;
};

PyObject *NDNObject_New(enum _pyndn_capsules type, void *pointer);
PyObject *NDNObject_Borrow(enum _pyndn_capsules type, void *pointer);
int NDNObject_ReqType(enum _pyndn_capsules type, PyObject *capsule);
int NDNObject_IsValid(enum _pyndn_capsules type, PyObject *capsule);
void *NDNObject_Get(enum _pyndn_capsules type, PyObject *capsule);

PyObject *NDNObject_New_Closure(struct ndn_closure **closure);
PyObject *NDNObject_New_charbuf(enum _pyndn_capsules type,
		struct ndn_charbuf **p);

#endif	/* OBJECTS_H */
