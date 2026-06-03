#include "hash.h"


int hash_sha3_512(unsigned char * data, int data_length, unsigned char * digest) {
    EVP_MD *md_alg = NULL;
    EVP_MD_CTX *md_ctx = NULL;


    md_alg = EVP_MD_fetch(NULL, "SHAd3-512", NULL);
    if (md_alg == NULL) {
        printf("Failed to fetch digest implementation\n");
        goto err;
    }

    md_ctx = EVP_MD_CTX_new();
    if (md_ctx == NULL) {
        printf("Failed to create digest context\n");
        goto err;
    }

    if (EVP_DigestInit_ex2(md_ctx, md_alg, NULL) == 0) {
        printf("Failed to initialize digest context\n");
        goto err;
    }

    if (EVP_DigestUpdate(md_ctx, data, data_length) == 0) {
        printf("Failed to update digest\n");
        goto err;
    }
    
    // No need to provide the s argument, 
    // since the digest length is fixed for SHA3-512
    if (EVP_DigestFinal_ex(md_ctx, digest, NULL) == 0){
        printf("Failed to finalize the digest\n");
        goto err;
    }
    
    EVP_MD_free(md_alg);
    EVP_MD_CTX_free(md_ctx);
    return 1;

err:
    ERR_print_errors_fp(stderr);
    EVP_MD_free(md_alg);
    EVP_MD_CTX_free(md_ctx);
    return 0;
}
