/*
 * Copyright (c) 2011, Regents of the University of California
 * BSD license, See the COPYING file for more information
 * Written by: Derek Kulinski <takeda@takeda.tk>
 *             Jeff Burke <jburke@ucla.edu>
 */

#include "python_hdr.h"
#include <ndn/ndn.h>
#include <ndn/charbuf.h>
#include <ndn/signing.h>

#include <stdlib.h>

#include "pyndn.h"
#include "objects.h"
#include "util.h"

/*
static struct completed_closure *g_completed_closures;
 */

static struct type_to_name {
	enum _pyndn_capsules type;
	const char *name;
} g_types_to_names[] = {
	{CLOSURE, "Closure_ndn_data"},
	{CONTENT_OBJECT, "ContentObject_ndn_data"},
	{EXCLUSION_FILTER, "ExclusionFilter_ndn_data"},
	{HANDLE, "NDN_ndn_data"},
	{INTEREST, "Interest_ndn_data"},
	{KEY_LOCATOR, "KeyLocator_ndn_data"},
	{NAME, "Name_ndn_data"},
	{PKEY_PRIV, "PKEY_PRIV_ndn_data"},
	{PKEY_PUB, "PKEY_PUB_ndn_data"},
	{SIGNATURE, "Signature_ndn_data"},
	{SIGNED_INFO, "SignedInfo_ndn_data"},
	{SIGNING_PARAMS, "SigningParams_ndn_data"},
#ifdef NAMECRYPTO
	{NAMECRYPTO_STATE, "Namecrypto_state"},
#endif
	{0, NULL}
};

static inline const char *
type2name(enum _pyndn_capsules type)
{
	struct type_to_name *p;

	assert(type > 0);
	assert(type < sizeof(g_types_to_names) / sizeof(struct type_to_name));


	p = &g_types_to_names[type - g_types_to_names[0].type];
	assert(p->type == type);
	return p->name;
}

static inline enum _pyndn_capsules
name2type(const char *name)
{
	struct type_to_name *p;

	assert(name);

	for (p = g_types_to_names; p->type; p++)
		if (!strcmp(p->name, name))
			return p->type;

	debug("name = %s", name);
	panic("Got unknown type name");

	return 0; /* this shouldn't be reached */
}

static void
pyndn_Capsule_Destructor(PyObject *capsule)
{
	const char *name;
	void *pointer;
	enum _pyndn_capsules type;

	assert(PyCapsule_CheckExact(capsule));

	name = PyCapsule_GetName(capsule);
	type = name2type(name);

	pointer = PyCapsule_GetPointer(capsule, name);
	assert(pointer);

	switch (type) {
	case CLOSURE:
	{
		PyObject *py_obj_closure;
		struct ndn_closure *p = pointer;

		py_obj_closure = PyCapsule_GetContext(capsule);
		assert(py_obj_closure);
		Py_DECREF(py_obj_closure); /* No longer referencing Closure object */

		/* If we store something else, than ourselves, it probably is a bug */
		assert(capsule == p->data);

		free(p);
	}
		break;
	case CONTENT_OBJECT:
	{
		struct content_object_data *context;
		struct ndn_charbuf *p = pointer;

		context = PyCapsule_GetContext(capsule);
		if (context) {
			if (context->pco)
				free(context->pco);
			ndn_indexbuf_destroy(&context->comps);
			free(context);
		}
		ndn_charbuf_destroy(&p);
	}
		break;
	case HANDLE:
	{
		struct ndn *p = pointer;
		ndn_disconnect(p);
		ndn_destroy(&p);
	}
		break;
	case INTEREST:
	{
		struct interest_data *context;
		struct ndn_charbuf *p = pointer;

		context = PyCapsule_GetContext(capsule);
		if (context) {
			if (context->pi)
				free(context->pi);
			free(context);
		}
		ndn_charbuf_destroy(&p);
	}
		break;
	case PKEY_PRIV:
	case PKEY_PUB:
	{
		struct ndn_pkey *p = pointer;
		ndn_pubkey_free(p);
	}
		break;
	case EXCLUSION_FILTER:
	case KEY_LOCATOR:
	case NAME:
	case SIGNATURE:
	case SIGNED_INFO:
	{
		struct ndn_charbuf *p = pointer;
		ndn_charbuf_destroy(&p);
	}
		break;
	case SIGNING_PARAMS:
	{
		struct ndn_signing_params *p = pointer;

		if (p->template_ndnb)
			ndn_charbuf_destroy(&p->template_ndnb);

		free(p);
	}
		break;
#ifdef NAMECRYPTO
	case NAMECRYPTO_STATE:
		free(pointer);
		break;
#endif
	default:
		debug("Got capsule: %s\n", PyCapsule_GetName(capsule));
		panic("Unable to destroy the object: got an unknown capsule");
	}
}

PyObject *
NDNObject_New(enum _pyndn_capsules type, void *pointer)
{
	PyObject *capsule;
	int r;

	assert(pointer);
	capsule = PyCapsule_New(pointer, type2name(type), pyndn_Capsule_Destructor);
	if (!capsule)
		return NULL;

	switch (type) {
	case CONTENT_OBJECT:
	{
		struct content_object_data *context;

		context = calloc(1, sizeof(*context));
		JUMP_IF_NULL_MEM(context, error);

		r = PyCapsule_SetContext(capsule, context);
		if (r < 0) {
			free(context);
			goto error;
		}
		break;
	}
	case INTEREST:
	{
		struct interest_data *context;

		context = calloc(1, sizeof(*context));
		JUMP_IF_NULL_MEM(context, error);

		r = PyCapsule_SetContext(capsule, context);
		if (r < 0) {
			free(context);
			goto error;
		}
		break;
	}
	default:
		break;
	}

	return capsule;

error:
	Py_XDECREF(capsule);
	return NULL;
}

PyObject *
NDNObject_Borrow(enum _pyndn_capsules type, void *pointer)
{
	PyObject *r;

	assert(pointer);
	r = PyCapsule_New(pointer, type2name(type), NULL);
	assert(r);

	return r;
}

int
NDNObject_ReqType(enum _pyndn_capsules type, PyObject *capsule)
{
	int r;
	const char *t = type2name(type);

	r = PyCapsule_IsValid(capsule, t);
	if (!r)
		PyErr_Format(PyExc_TypeError, "Argument needs to be of %s type", t);

	return r;
}

int
NDNObject_IsValid(enum _pyndn_capsules type, PyObject *capsule)
{
	return PyCapsule_IsValid(capsule, type2name(type));
}

void *
NDNObject_Get(enum _pyndn_capsules type, PyObject *capsule)
{
	void *p;

	assert(NDNObject_IsValid(type, capsule));
	p = PyCapsule_GetPointer(capsule, type2name(type));
	assert(p);

	return p;
}

PyObject *
NDNObject_New_Closure(struct ndn_closure **closure)
{
	struct ndn_closure *p;
	PyObject *result;

	p = calloc(1, sizeof(*p));
	if (!p)
		return PyErr_NoMemory();

	result = NDNObject_New(CLOSURE, p);
	if (!result) {
		free(p);
		return NULL;
	}

	if (closure)
		*closure = p;

	return result;
}

PyObject *
NDNObject_New_charbuf(enum _pyndn_capsules type,
		struct ndn_charbuf **charbuf)
{
	struct ndn_charbuf *p;
	PyObject *py_o;

	assert(type == CONTENT_OBJECT ||
			type == EXCLUSION_FILTER ||
			type == INTEREST ||
			type == KEY_LOCATOR ||
			type == NAME ||
			type == SIGNATURE ||
			type == SIGNED_INFO);

	p = ndn_charbuf_create();
	if (!p)
		return PyErr_NoMemory();

	py_o = NDNObject_New(type, p);
	if (!py_o) {
		ndn_charbuf_destroy(&p);
		return NULL;
	}

	if (charbuf)
		*charbuf = p;

	return py_o;
}
