#ifndef HASH_H
#define HASH_H

#include <openssl/evp.h>
#include <stdio.h>
#include <openssl/err.h>

int hash_sha3_512(unsigned char * data, int data_length, unsigned char * digest);

#endif // HASH_H