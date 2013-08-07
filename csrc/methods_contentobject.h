/*
 * Copyright (c) 2011, Regents of the University of California
 * BSD license, See the COPYING file for more information
 * Written by: Derek Kulinski <takeda@takeda.tk>
 *             Jeff Burke <jburke@ucla.edu>
 */

#ifndef MEDHODS_CONTENTOBJECT_H
#  define	MEDHODS_CONTENTOBJECT_H

struct ndn_parsed_ContentObject *_pyndn_content_object_get_pco(
		PyObject *py_content_object);
void _pyndn_content_object_set_pco(PyObject *py_content_object,
		struct ndn_parsed_ContentObject *pco);
struct ndn_indexbuf *_pyndn_content_object_get_comps(
		PyObject *py_content_object);
void _pyndn_content_object_set_comps(PyObject *py_content_object,
		struct ndn_indexbuf *comps);
PyObject *ContentObject_obj_from_ndn(PyObject *py_content_object);
PyObject *_pyndn_cmd_content_to_bytes(PyObject *self, PyObject *arg);
PyObject *_pyndn_cmd_content_to_bytearray(PyObject *self, PyObject *arg);
PyObject *_pyndn_cmd_encode_ContentObject(PyObject *self, PyObject *args);
PyObject *_pyndn_cmd_ContentObject_obj_from_ndn(PyObject *self, PyObject *py_co);
PyObject *_pyndn_cmd_digest_contentobject(PyObject *self, PyObject *args);
PyObject *_pyndn_cmd_content_matches_interest(PyObject *self, PyObject *args);
PyObject *_pyndn_cmd_verify_content(PyObject *self, PyObject *args);
PyObject *_pyndn_cmd_verify_signature(PyObject *self, PyObject *args);

#endif	/* MEDHODS_CONTENTOBJECT_H */

