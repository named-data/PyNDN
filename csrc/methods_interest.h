/*
 * Copyright (c) 2011, Regents of the University of California
 * BSD license, See the COPYING file for more information
 * Written by: Derek Kulinski <takeda@takeda.tk>
 *             Jeff Burke <jburke@ucla.edu>
 */

#ifndef METHODS_INTERESTS_H
#  define	METHODS_INTERESTS_H

PyObject *Interest_obj_from_ndn(PyObject *py_interest);
struct ndn_parsed_interest *_pyndn_interest_get_pi(PyObject *py_interest);
void _pyndn_interest_set_pi(PyObject *py_interest,
		struct ndn_parsed_interest *pi);
PyObject *_pyndn_cmd_Interest_obj_to_ndn(PyObject *UNUSED(self),
		PyObject *py_interest);
PyObject *_pyndn_cmd_Interest_obj_from_ndn(PyObject *UNUSED(self), PyObject *args);
PyObject *_pyndn_cmd_ExclusionFilter_names_to_ndn(PyObject *UNUSED(self),
		PyObject* args);
PyObject *_pyndn_cmd_ExclusionFilter_obj_from_ndn(PyObject *UNUSED(self),
		PyObject* args);

#endif	/* METHODS_INTERESTS_H */

