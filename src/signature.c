#include "signature.h"
#include "hash.h"
#include "common.h"


EVP_PKEY* load_private_key_from_file(const char* filename) {
    BIO* bio;
    OSSL_DECODER_CTX* decoder_ctx;
    EVP_PKEY* pkey = NULL;

    bio = BIO_new_file(filename, "rb");
    if (bio == NULL) {
        fprintf(stderr, "Error opening private key file: %s\n", filename);
        return NULL;
    }
    decoder_ctx = OSSL_DECODER_CTX_new_for_pkey(&pkey, NULL, NULL, NULL, 0, NULL, NULL);
    if (decoder_ctx == NULL){
        fprintf(stderr, "Error creating decoder context\n");
        BIO_free(bio);
        return NULL;
    }
    if (OSSL_DECODER_from_bio(decoder_ctx, bio) != 1) {
        fprintf(stderr, "Error decoding private key from file\n");
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
