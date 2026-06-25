#ifndef SIGNATURE_H
#define SIGNATURE_H
#include "common.h"

EVP_PKEY* load_key_from_file(const char* filename);

int sign_RSA_PSS_SHA3_512(const char *message,
                          size_t message_len, 
                          EVP_PKEY *private_key,
                          size_t *signature_len,
                          unsigned char **signature);

int verify_signature_RSA_PSS_sha3_512(const char * message,
                                      size_t message_len,
                                      unsigned char * signature, 
                                      size_t signature_len,
                                      EVP_PKEY * public_key);

#endif // SIGNATURE_H