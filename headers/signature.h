#ifndef SIGNATURE_H
#define SIGNATURE_H

EVP_PKEY* load_key_from_file(const char* filename);
int sign_RSA_PSS_SHA3_512(const char *message,
                          size_t message_len, 
                          EVP_PKEY *private_key,
                          size_t *signature_len,
                          unsigned char **signature);
#endif // SIGNATURE_H