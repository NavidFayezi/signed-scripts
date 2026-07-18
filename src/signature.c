#include "signature.h"
#include "common.h"


EVP_PKEY* load_key_from_file(const char* filename) {
    BIO* bio;
    OSSL_DECODER_CTX* decoder_ctx;
    EVP_PKEY* pkey = NULL;

    bio = BIO_new_file(filename, "rb");
    if (bio == NULL) {
        fprintf(stderr, "Error opening key file: %s\n", filename);
        return NULL;
    }
    decoder_ctx = OSSL_DECODER_CTX_new_for_pkey(&pkey, NULL, NULL, NULL, 0, NULL, NULL);
    if (decoder_ctx == NULL) {
        fprintf(stderr, "Error creating decoder context\n");
        BIO_free(bio);
        return NULL;
    }
    if (OSSL_DECODER_from_bio(decoder_ctx, bio) != 1) {
        fprintf(stderr, "Error decoding key from file\n");
        OSSL_DECODER_CTX_free(decoder_ctx);
        EVP_PKEY_free(pkey);
        pkey = NULL;
        BIO_free(bio);
        return NULL;
    }
    OSSL_DECODER_CTX_free(decoder_ctx);
    BIO_free(bio);
    return pkey;
}

int sign_RSA_PSS_SHA3_512(const char *message,
                          size_t message_len, 
                          EVP_PKEY *private_key,
                          size_t *signature_len,
                          unsigned char **signature) { // *signature should be NULL
    
    
    EVP_MD_CTX *mdctx;
    OSSL_PARAM params[2];

    *signature = NULL;

    mdctx = EVP_MD_CTX_new();
    if (mdctx == NULL) {
        fprintf(stderr, "Error creating digest context\n");
        return 0;
    }
    params[0] = OSSL_PARAM_construct_utf8_string(OSSL_SIGNATURE_PARAM_PAD_MODE,
        OSSL_PKEY_RSA_PAD_MODE_PSS, 0);
    params[1] = OSSL_PARAM_construct_end();

    if (EVP_DigestSignInit_ex(mdctx, NULL, "SHA3-512", 
            NULL, NULL, private_key, params)
            != 1) {
            
            fprintf(stderr, "Error Initializing the signing context\n");
            goto error;
            }

    if (EVP_DigestSignUpdate(mdctx, message, message_len) != 1){
        fprintf(stderr, "Error updating the context with hash of message\n");
        goto error;
    }

    if (EVP_DigestSignFinal(mdctx, NULL, signature_len) != 1) {
        fprintf(stderr, "Error obtaining the length of the signature\n");
        goto error;
    }
    *signature = OPENSSL_malloc(*signature_len);
    if (*signature == NULL) {
        fprintf(stderr, "Error allocating memory for signature\n");
        goto error;
    }

    if(EVP_DigestSignFinal(mdctx, *signature, signature_len) != 1){
        fprintf(stderr, "Error signing the data\n");
        goto error;
    }
    
    EVP_MD_CTX_free(mdctx);
    return 1;

error:
    ERR_print_errors_fp(stderr);
    EVP_MD_CTX_free(mdctx);
    OPENSSL_free(*signature);
    return 0;
}

int verify_signature_RSA_PSS_sha3_512(const char * message,
                                      size_t message_len,
                                      unsigned char * signature, 
                                      size_t signature_len,
                                      EVP_PKEY * public_key) {
    EVP_MD_CTX* mdctx = NULL;
    OSSL_PARAM params [2];
    int verification_res = 0;

    mdctx = EVP_MD_CTX_new();
    if (mdctx == NULL) {
        fprintf(stderr, "Error creating context\n");
        goto error;
    }
    params[0] = OSSL_PARAM_construct_utf8_string(OSSL_SIGNATURE_PARAM_PAD_MODE,
        OSSL_PKEY_RSA_PAD_MODE_PSS, 0);
    params[1] = OSSL_PARAM_construct_end();
    
    if (EVP_DigestVerifyInit_ex(mdctx, NULL, "SHA3-512", 
                                NULL, NULL, public_key, 
                                params) 
                                != 1) {
        fprintf(stderr, "Error initializing the signing context\n");
        goto error;
    }

    if (EVP_DigestVerifyUpdate(mdctx, message, message_len) != 1) {
        fprintf(stderr, "Error hashing message into signing context\n");
        goto error;
    }
    verification_res = EVP_DigestVerifyFinal(mdctx, signature, signature_len);
    if (verification_res != 1) {
        if (verification_res == 0) {
            //fprintf(stdout, "Bad signature, verification failed\n");
            goto end;
        }
        else {
            fprintf(stderr, "Failed to verify the message, signature may be invalid\n");
            goto error;
        }
    }

end:
    EVP_MD_CTX_free(mdctx);
    return verification_res;

error:
    ERR_print_errors_fp(stderr);
    EVP_MD_CTX_free(mdctx);
    return 0;
}