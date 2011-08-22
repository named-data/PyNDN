/*
 * Copyright (c) 2011, Regents of the University of California
 * All rights reserved.
 * Written by: Jeff Burke <jburke@ucla.edu>
 *             Derek Kulinski <takeda@takeda.tk>
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *     * Redistributions of source code must retain the above copyright
 *       notice, this list of conditions and the following disclaimer.
 *     * Redistributions in binary form must reproduce the above copyright
 *       notice, this list of conditions and the following disclaimer in the
 *       documentation and/or other materials provided with the distribution.
 *     * Neither the name of the Regents of the University of California nor
 *       the names of its contributors may be used to endorse or promote
 *       products derived from this software without specific prior written
 *       permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED. IN NO EVENT SHALL REGENTS BE LIABLE FOR ANY DIRECT, INDIRECT,
 * INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
 * LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA,
 * OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF
 * LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
 * NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE,
 * EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#include "python.h"

#include <ccn/ccn.h>
#include <ccn/uri.h>
#include <ccn/signing.h>
#include <ccn/keystore.h>

// On MacOS X, need to have the latest version from MacPorts
// and add /opt/local/include as an include path
#include <openssl/rsa.h>
#include <openssl/pem.h>
#include <openssl/evp.h>
#include <openssl/sha.h>
#include <openssl/rand.h>
#include <openssl/ossl_typ.h>

#include <assert.h>

#include "key_utils.h"
#include "util.h"

static char g_seed_file[200];
static int g_seeded = 0;

static void
save_seed()
{
	assert(*g_seed_file);

	if (!g_seeded)
		return; /* so we won't pollute the seed file*/

	RAND_write_file(g_seed_file);
}

static void
seed_prng()
{
	const char *file;
	int res;

	if (*g_seed_file)
		file = g_seed_file;
	else
		file = RAND_file_name(g_seed_file, sizeof(g_seed_file));

	if (!file)
		panic("Unable to obtain name for random seed file.");

	if (RAND_load_file(file, -1)) {
		g_seeded = 1;

		/* for a good measure */
		RAND_load_file("/dev/random", 8);

		return;
	}

	/*
	 * This is a special case, I hope it won't be normally used
	 * please review if this is sufficient, I'm not using purely /dev/random
	 * because some computers don't have enough enthropy
	 */
	res = RAND_load_file("/dev/urandom", 2048);
	if (res < 2048)
		panic("Unable to gather seed, /dev/urandom not available?");

	res = RAND_load_file("/dev/random", 32);
	if (res < 32)
		panic("Unable to gather enough entropy, /dev/random not available?");

	g_seeded = 1;
}

//
// Caller must free
//

int
generate_key(int length, struct ccn_pkey** private_key_ccn, struct ccn_pkey** public_key_ccn,
    unsigned char** public_key_digest, size_t *public_key_digest_len)
{
	RSA *private_key_rsa;

	seed_prng();
	private_key_rsa = RSA_generate_key(length, 65537, NULL, NULL);
	save_seed();

	if (!private_key_rsa)
		return -1;

	ccn_keypair_from_rsa(private_key_rsa, private_key_ccn, public_key_ccn);
	create_public_key_digest(private_key_rsa, public_key_digest, public_key_digest_len);

	return 0;
}

int
generate_keypair(int length, struct keypair** KP)
{
	(*KP) = (struct keypair*) calloc(sizeof(struct keypair), 1);
	generate_key(length, &(*KP)->private_key, &(*KP)->public_key,
	    &(*KP)->public_key_digest, &(*KP)->public_key_digest_len);
	return 0;
}

//
// Writes without encryption/password!
//

int
write_key_pem(FILE *fp, struct ccn_pkey* private_key_ccn)
{
	RSA* private_key_rsa = EVP_PKEY_get1_RSA((EVP_PKEY*) private_key_ccn);
	PEM_write_RSAPrivateKey(fp, private_key_rsa, NULL, NULL, 0, NULL, NULL);
	//PEM_write_RSAPublicKey(stderr, private_key_rsa);
	RSA_free(private_key_rsa);
	return 0;
}

int
write_key_pem_public(FILE *fp, struct ccn_pkey* private_key_ccn)
{
	RSA* private_key_rsa = EVP_PKEY_get1_RSA((EVP_PKEY*) private_key_ccn);
	//PEM_write_RSAPrivateKey(fp, private_key_rsa, NULL, NULL, 0, NULL, NULL);
	PEM_write_RSAPublicKey(fp, private_key_rsa);
	RSA_free(private_key_rsa);
	return 0;
}

int
get_key_pem_public(char** buf, int* length, struct ccn_pkey* private_key_ccn)
{
	RSA* private_key_rsa = EVP_PKEY_get1_RSA((EVP_PKEY*) private_key_ccn);
	BIO* bio = BIO_new(BIO_s_mem());
	BUF_MEM *bufmem;
	PEM_write_bio_RSAPublicKey(bio, private_key_rsa);
	BIO_get_mem_ptr(bio, &bufmem);
	*buf = bufmem->data;
	*length = bufmem->length;
	char zero = 0; // zero terminate in memory for easier string printing
	BIO_write(bio, &zero, 1);
	BIO_set_close(bio, BIO_NOCLOSE); // don't destroy
	BIO_free(bio);
	RSA_free(private_key_rsa);
	return 0;
}


//
// Reads without decryption
//

int
read_key_pem(FILE *fp, struct ccn_pkey** private_key_ccn, struct ccn_pkey** public_key_ccn,
    unsigned char** public_key_digest, size_t *public_key_digest_len)
{
	RSA *private_key_rsa;
	PEM_read_RSAPrivateKey(fp, &private_key_rsa, NULL, NULL);
	ccn_keypair_from_rsa(private_key_rsa, private_key_ccn, public_key_ccn);
	create_public_key_digest(private_key_rsa, public_key_digest, public_key_digest_len);
	RSA_free(private_key_rsa);
	return 0;
}

int
read_keypair_pem(FILE *fp, struct keypair** KP)
{
	(*KP) = (struct keypair*) calloc(sizeof(struct keypair), 1);
	RSA *private_key_rsa;
	PEM_read_RSAPrivateKey(fp, &private_key_rsa, NULL, NULL);
	ccn_keypair_from_rsa(private_key_rsa, &(*KP)->private_key, &(*KP)->public_key);
	create_public_key_digest(private_key_rsa, &(*KP)->public_key_digest, &(*KP)->public_key_digest_len);
	RSA_free(private_key_rsa);
	return 0;
}

int
release_key(struct ccn_pkey** private_key_ccn, struct ccn_pkey** public_key_ccn, unsigned char** public_key_digest)
{
	if (public_key_ccn != NULL && *public_key_ccn != NULL)
		EVP_PKEY_free((EVP_PKEY*) * public_key_ccn);
	if (private_key_ccn != NULL && private_key_ccn != NULL)
		EVP_PKEY_free((EVP_PKEY*) * private_key_ccn);
	if (public_key_digest != NULL && *public_key_digest != NULL)
		free(*public_key_digest);
	return 0;
}

int
release_keypair(struct keypair** KP)
{
	if (KP != NULL && (*KP) != NULL)
		free(*KP);
	return 0;
}

int
build_keylocator_from_key(struct ccn_charbuf** keylocator, struct ccn_pkey* key)
{
	int res = 0;
	*keylocator = ccn_charbuf_create();
	ccn_charbuf_append_tt(*keylocator, CCN_DTAG_KeyLocator, CCN_DTAG);
	ccn_charbuf_append_tt(*keylocator, CCN_DTAG_Key, CCN_DTAG);
	res = ccn_append_pubkey_blob(*keylocator, key);
	ccn_charbuf_append_closer(*keylocator); /* </Key> */
	ccn_charbuf_append_closer(*keylocator); /* </KeyLocator> */
	return(res);
}

int
create_public_key_digest(RSA* private_key_rsa, unsigned char** public_key_digest, size_t *public_key_digest_len)
{
	// Generate digest:
	*public_key_digest = (unsigned char*) calloc(1, SHA256_DIGEST_LENGTH);
	int der_len = i2d_RSAPublicKey(private_key_rsa, 0);
	unsigned char *public_key_der, *pub;
	public_key_der = pub = (unsigned char*) calloc(1, der_len);
	i2d_RSAPublicKey(private_key_rsa, &pub); // pub is altered in this call
	SHA256(public_key_der, der_len, *public_key_digest);
	*public_key_digest_len = SHA256_DIGEST_LENGTH;
	free(public_key_der);
	return 0;
}

int
ccn_keypair_from_rsa(RSA* private_key_rsa, struct ccn_pkey** private_key_ccn, struct ccn_pkey** public_key_ccn)
{
	if (private_key_ccn != NULL) {
		*private_key_ccn = (struct ccn_pkey*) EVP_PKEY_new();
		EVP_PKEY_set1_RSA((EVP_PKEY*) * private_key_ccn, private_key_rsa);
	}
	if (public_key_ccn != NULL) {
		RSA* public_key_rsa = RSAPublicKey_dup(private_key_rsa);
		*public_key_ccn = (struct ccn_pkey*) EVP_PKEY_new();
		EVP_PKEY_set1_RSA((EVP_PKEY*) * public_key_ccn, public_key_rsa);
		RSA_free(public_key_rsa);
	}
	return 0;
}

int
get_ASN_public_key(unsigned char** public_key_der, int* public_key_der_len, struct ccn_pkey* private_key)
{
	unsigned char *pub;
	//DER encode / pkcs#1
	RSA* private_key_rsa = EVP_PKEY_get1_RSA((EVP_PKEY*) private_key);
	*public_key_der_len = i2d_RSAPublicKey(private_key_rsa, 0);
	*public_key_der = pub = (unsigned char*) calloc(*public_key_der_len, 1);
	i2d_RSAPublicKey(private_key_rsa, &pub);
	return 0;
}

int
generate_RSA_keypair(unsigned char** private_key_der, size_t *private_key_len,
    unsigned char** public_key_der, size_t *public_key_len)
{
	RSA *private_key;
	private_key = RSA_generate_key(1024, 65537, NULL, NULL);
	unsigned char *priv, *pub;
	//DER encode / pkcs#1
	*private_key_len = i2d_RSAPrivateKey(private_key, 0);
	*public_key_len = i2d_RSAPublicKey(private_key, 0);
	*private_key_der = priv = (unsigned char*) calloc(*private_key_len, 1);
	*public_key_der = pub = (unsigned char*) calloc(*public_key_len, 1);
	i2d_RSAPrivateKey(private_key, &priv);
	i2d_RSAPublicKey(private_key, &pub);
	RSA_free(private_key);

	/*
       // Check that all is well, DER decode
       fprintf(stderr, "decoded:\n");
       RSA *public_key_rsa, *private_key_rsa;
       public_key_rsa = d2i_RSAPublicKey(0, (const unsigned char**) &public_key_der, public_key_len);
       private_key_rsa = d2i_RSAPrivateKey(0, (const unsigned char**) &private_key_der, private_key_len);
	PEM_write_RSAPrivateKey(stderr, private_key_rsa, NULL, NULL, 0, NULL, NULL);
	PEM_write_RSAPublicKey(stderr, private_key_rsa);
	 */
	return(0);
}

