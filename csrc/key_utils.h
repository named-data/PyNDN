/*
 * Copyright (c) 2011, Regents of the University of California
 * BSD license, See the COPYING file for more information
 * Written by: Derek Kulinski <takeda@takeda.tk>
 *             Jeff Burke <jburke@ucla.edu>
 */

#ifndef _KEY_UTILS_H_
#  define _KEY_UTILS_H_

// Load these here to make it easier on the app

// On MacOS X, need to have the latest version from MacPorts
// and add /opt/local/include as an include path
#  include <openssl/rsa.h>
#  include <openssl/pem.h>
#  include <openssl/evp.h>
#  include <openssl/sha.h>
#  include <openssl/ossl_typ.h>

#  include <ndn/keystore.h>

// work with NDN_pkey structures directly
//

// This is a lot like ndn's keystore, but
// that is an opaque type, so we're not supposed
// to use it directly.

struct keypair {
	struct ndn_pkey* private_key;
	struct ndn_pkey* public_key;
	unsigned char* public_key_digest;
	size_t public_key_digest_len;
};

void initialize_crypto(void);
int create_public_key_digest(RSA *private_key_rsa,
		PyObject **py_public_key_digest, int *public_key_digest_len);
int ndn_keypair_from_rsa(int public_only, RSA *private_key_rsa,
		PyObject **py_private_key_ndn,
		PyObject **py_public_key_ndn);
PyObject *_pyndn_privatekey_dup(const struct ndn_pkey *key);
int generate_key(int length, PyObject **private_key_ndn,
		PyObject **public_key_ndn, PyObject ** public_key_digest,
		int *public_key_digest_len);
//int generate_keypair(int length, struct keypair** KP);

// We use "PEM" to make things "readable" for now
int write_key_pem_private(FILE *fp, struct ndn_pkey* private_key_ndn);
int write_key_pem_public(FILE *fp, struct ndn_pkey* private_key_ndn);

PyObject *get_key_pem_private(const struct ndn_pkey *private_key_ndn);
PyObject *get_key_pem_public(const struct ndn_pkey *key_ndn);
PyObject *get_key_der_private(struct ndn_pkey *private_key_ndn);
PyObject *get_key_der_public(struct ndn_pkey *public_key_ndn);
int read_key_pem(FILE *fp, PyObject **py_private_key_ndn,
		PyObject **public_key_ndn, PyObject **py_public_key_digest,
		int *public_key_digest_len);
int put_key_pem(int is_public_only, PyObject *py_key_pem,
		PyObject **py_private_key_ndn, PyObject **py_public_key_ndn,
		PyObject **py_public_key_digest);
int put_key_der(int is_public_only, PyObject *py_key_der,
		PyObject **py_private_key_ndn, PyObject **py_public_key_ndn,
		PyObject **py_public_key_digest, int *public_key_digest_len);
int read_keypair_pem(FILE *fp, struct keypair** KP);
int release_key(struct ndn_pkey** private_key_ndn, struct ndn_pkey** public_key_ndn, unsigned char** public_key_digest);
int release_keypair(struct keypair** KP);

int build_keylocator_from_key(struct ndn_charbuf** keylocator, struct ndn_pkey* key);

int get_ASN_public_key(unsigned char** public_key_der, int* public_key_der_len, struct ndn_pkey* private_key);
RSA *ndn_key_to_rsa(struct ndn_pkey *key_ndn);

#endif /* _KEY_UTILS_H_ */
