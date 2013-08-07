/*
 * Copyright (c) 2011, Regents of the University of California
 * BSD license, See the COPYING file for more information
 * Written by: Derek Kulinski <takeda@takeda.tk>
 *             Jeff Burke <jburke@ucla.edu>
 */

#include "python_hdr.h"
#include <ndn/ndn.h>
#include <ndn/uri.h>

#include "methods_name.h"
#include "pyndn.h"
#include "objects.h"
#include "util.h"

// Can be called directly from c library
// For now, everything is a bytearray

static PyObject *
name_comps_from_ndn(PyObject *py_cname)
{
	struct ndn_charbuf *name;
	struct ndn_indexbuf *comp_index;
	int r;
	PyObject *py_component_list = NULL, *py_component;

	assert(NDNObject_IsValid(NAME, py_cname));

	name = NDNObject_Get(NAME, py_cname);

	comp_index = ndn_indexbuf_create();
	JUMP_IF_NULL_MEM(comp_index, error);

	r = ndn_name_split(name, comp_index);
	if (r < 0) {
		PyErr_SetString(PyExc_TypeError, "The argument is not a valid NDN"
				" name");
		goto error;
	}

	// Create component list
	py_component_list = PyList_New(0);
	JUMP_IF_NULL(py_component_list, error);

	/* I wish I could understand this code -dk */
	for (size_t n = 0; n < comp_index->n - 1; n++) { // not the implicit digest component
		size_t h; // header size
		int size;
		unsigned char *component;

		debug("name_comps_from_ndn component %d of %d \n", n, comp_index->n - 2);

		component = &(name->buf[comp_index->buf[n]]) + 1; // What is the first byte? (250?)
		//debug("\t%s\n", component);

		for (h = 2; h < (comp_index->buf[n + 1] - comp_index->buf[n]); h++) { // walk through the header until the terminators is found
			if (*(component++) > 127)
				break;
		}

		size = (comp_index->buf[n + 1] - comp_index->buf[n]) - 1 - h; // don't include the DTAG Component

		py_component = PyBytes_FromStringAndSize((char *) component, size);
		JUMP_IF_NULL(py_component, error);

		r = PyList_Append(py_component_list, py_component);
		Py_DECREF(py_component);
		JUMP_IF_NEG(r, error);
	}
	// TODO: Add implicit digest component?
	// TODO: Parse version & segment?

	ndn_indexbuf_destroy(&comp_index);

	return py_component_list;

error:
	ndn_indexbuf_destroy(&comp_index);
	Py_XDECREF(py_component_list);
	return NULL;
}

static PyObject *
name_comps_to_ndn(PyObject *py_name_components)
{
	struct ndn_charbuf *name;
	PyObject *py_name, *iterator, *item = NULL;
	PyObject *py_o;
	int r;

	if (!PyList_Check(py_name_components)) {
		PyErr_SetString(PyExc_TypeError, "Must pass a components of the Name");
		return NULL;
	}

	iterator = PyObject_GetIter(py_name_components);
	if (!iterator)
		return NULL;

	py_name = NDNObject_New_charbuf(NAME, &name);
	JUMP_IF_NULL(name, error);

	r = ndn_name_init(name);
	JUMP_IF_NEG_MEM(r, error);

	// Parse the list of components and
	// convert them to C objects
	//
	while ((item = PyIter_Next(iterator))) {
		if (PyUnicode_Check(item)) {
			char *s;
			Py_ssize_t len;

			py_o = _pyndn_unicode_to_utf8(item, &s, &len);
			JUMP_IF_NULL(py_o, error);

			r = ndn_name_append(name, s, len);
			Py_DECREF(py_o);
			JUMP_IF_NEG_MEM(r, error);
		} else if (PyBytes_Check(item)) {
			char *b;
			Py_ssize_t n;

			r = PyBytes_AsStringAndSize(item, &b, &n);
			JUMP_IF_NEG(r, error);

			r = ndn_name_append(name, b, n);
			JUMP_IF_NEG_MEM(r, error);
		} else if (PyByteArray_Check(item)) {
			Py_ssize_t n = PyByteArray_Size(item);
			char *b = PyByteArray_AsString(item);
			r = ndn_name_append(name, b, n);
			JUMP_IF_NEG_MEM(r, error);

			// Note, we choose to convert numbers to their string
			// representation; if we want numeric encoding, use a
			// byte array and do it explicitly.
		} else if (PyFloat_Check(item) || PyLong_Check(item) ||
				_pyndn_Int_Check(item)) {
			char *s;
			PyObject *py_o2;

			py_o = PyObject_Str(item);
			JUMP_IF_NULL(py_o, error);

			py_o2 = _pyndn_unicode_to_utf8(py_o, &s, NULL);
			Py_DECREF(py_o);
			if (!py_o2)
				goto error;

			r = ndn_name_append_str(name, s);
			Py_DECREF(py_o2);
			JUMP_IF_NEG_MEM(r, error);
		} else {
			PyErr_SetString(PyExc_TypeError, "Unknown value type in the list");
			goto error;
		}
		Py_DECREF(item);
	}
	Py_CLEAR(iterator);

	return NDNObject_New(NAME, name);

error:
	Py_XDECREF(item);
	Py_DECREF(iterator);
	Py_XDECREF(py_name);
	return NULL;
}

PyObject *
_pyndn_cmd_name_comps_to_ndn(PyObject *UNUSED(self), PyObject *py_name_components)
{
	return name_comps_to_ndn(py_name_components);
}

// From within python
//

PyObject *
_pyndn_cmd_name_comps_from_ndn(PyObject *UNUSED(self), PyObject *py_cname)
{
	if (!NDNObject_IsValid(NAME, py_cname)) {
		PyErr_SetString(PyExc_TypeError, "Must pass a NDN name");
		return NULL;
	}

	return name_comps_from_ndn(py_cname);
}

PyObject *
Name_obj_from_ndn(PyObject *py_cname)
{
	PyObject *py_Name = NULL, *py_kargs;
	int r;

	assert(g_type_Name);
	assert(NDNObject_IsValid(NAME, py_cname));

	py_kargs = PyDict_New();
	JUMP_IF_NULL(py_kargs, error);

	r = PyDict_SetItemString(py_kargs, "ndn_data", py_cname);
	JUMP_IF_NEG(r, error);

	py_Name = PyEval_CallObjectWithKeywords(g_type_Name, NULL, py_kargs);
	JUMP_IF_NULL(py_Name, error);
	Py_CLEAR(py_kargs);

	return py_Name;

error:
	Py_XDECREF(py_kargs);
	return NULL;
}

PyObject *
Name_obj_to_ndn(PyObject *py_obj_Name)
{
	PyObject *comps, *py_name;

	comps = PyObject_GetAttrString(py_obj_Name, "components");
	if (!comps)
		return NULL;

	py_name = name_comps_to_ndn(comps);
	Py_DECREF(comps);

	return py_name;
}

// Takes a byte array with DTAG
//

PyObject *
Name_from_ndn_tagged_bytearray(const unsigned char *buf, size_t size)
{
	PyObject *py_name, *py_cname;
	struct ndn_charbuf *name;
	int r;

	name = ndn_charbuf_create();
	if (!name)
		return PyErr_NoMemory();

	py_cname = NDNObject_New(NAME, name);
	if (!py_cname) {
		ndn_charbuf_destroy(&name);
		return NULL;
	}

	r = ndn_charbuf_append(name, buf, size);
	py_name = r < 0 ? PyErr_NoMemory() : Name_obj_from_ndn(py_cname);
	Py_DECREF(py_cname);

	return py_name;
}

PyObject *
_pyndn_cmd_name_from_uri(PyObject *UNUSED(self), PyObject *py_uri)
{
	struct ndn_charbuf *name;
	PyObject *py_name = NULL, *py_o;
	char *buf;
	int r;

	if (!_pyndn_STRING_CHECK(py_uri)) {
		PyErr_SetString(PyExc_TypeError, "Expected string");
		return NULL;
	}

	py_name = NDNObject_New_charbuf(NAME, &name);
	JUMP_IF_NULL(py_name, error);

	py_o = _pyndn_unicode_to_utf8(py_uri, &buf, NULL);
	JUMP_IF_NULL(py_o, error);

	r = ndn_name_from_uri(name, buf);
	Py_DECREF(py_o);
	if (r < 0) {
		PyErr_SetString(g_PyExc_NDNNameError, "Error parsing URI");
		goto error;
	}

	return py_name;

error:
	Py_XDECREF(py_name);
	return NULL;
}

PyObject *
_pyndn_cmd_name_to_uri(PyObject *UNUSED(self), PyObject *py_name)
{
	struct ndn_charbuf *cb, *uri = NULL;
	enum _pyndn_capsules type;
	int r;
	PyObject *py_o;

	if (NDNObject_IsValid(NAME, py_name)) {
		type = NAME;
		goto correct_type;
	}

	/*
		if (NDNObject_IsValid(INTEREST, py_name)) {
			type = INTEREST;
			goto correct_type;
		}

		if (NDNObject_IsValid(CONTENT_OBJECT, py_name)) {
			type = CONTENT_OBJECT;
			goto correct_type;
		}
	 */

	PyErr_SetString(PyExc_TypeError, "Expected NDN name");
	return NULL;

correct_type:
	cb = NDNObject_Get(type, py_name);

	uri = ndn_charbuf_create();
	JUMP_IF_NULL_MEM(uri, error);

	r = ndn_uri_append(uri, cb->buf, cb->length, 0);
	if (r < 0) {
		PyErr_SetString(g_PyExc_NDNNameError, "Error while converting name");
		goto error;
	}

	py_o = PyUnicode_FromStringAndSize((char *) uri->buf, uri->length);
	ndn_charbuf_destroy(&uri);
	JUMP_IF_NULL(py_o, error);

	return py_o;

error:
	ndn_charbuf_destroy(&uri);
	return NULL;
}

PyObject *
_pyndn_cmd_compare_names(PyObject *UNUSED(self), PyObject *args)
{
	PyObject *py_name1, *py_name2;
	struct ndn_charbuf *name1, *name2;
	int diff;

	if (!PyArg_ParseTuple(args, "OO", &py_name1, &py_name2))
		return NULL;

	if (!NDNObject_IsValid(NAME, py_name1)) {
		PyErr_SetString(PyExc_TypeError, "Must pass a NDN name as 1st"
				" argument");
		return NULL;
	}

	if (!NDNObject_IsValid(NAME, py_name2)) {
		PyErr_SetString(PyExc_TypeError, "Must pass a NDN name as 2nd"
				" argument");
		return NULL;
	}

	name1 = NDNObject_Get(NAME, py_name1);
	name2 = NDNObject_Get(NAME, py_name2);

	diff = ndn_compare_names(name1->buf, name1->length, name2->buf,
			name2->length);

	return Py_BuildValue("i", diff);
}
