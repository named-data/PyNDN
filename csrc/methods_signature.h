/*
 * Copyright (c) 2011, Regents of the University of California
 * BSD license, See the COPYING file for more information
 * Written by: Derek Kulinski <takeda@takeda.tk>
 *             Jeff Burke <jburke@ucla.edu>
 */

#ifndef METHODS_SIGNATURE_H
#  define	METHODS_SIGNATURE_H

PyObject *Signature_obj_from_ndn(PyObject *py_signature);
PyObject *_pyndn_cmd_Signature_obj_to_ndn(PyObject* self, PyObject* args);
PyObject *_pyndn_cmd_Signature_obj_from_ndn(PyObject* self, PyObject* args);

#endif	/* METHODS_SIGNATURE_H */
