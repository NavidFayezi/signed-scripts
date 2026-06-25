#include "common.h"
#include "signature.h"


int main(int argc, char *argv[]){
    const char * sk_filename = "keys/private_key.pem";
    const char * pk_filename = "keys/public_key.pem";
    const char * test_data = "This is the data to be signed\n";
    unsigned char * signature = NULL;
    size_t signature_length = 0;
    int rc = 1;
    EVP_PKEY * secret_key = NULL;
    EVP_PKEY * public_key = NULL;


    secret_key = load_key_from_file(sk_filename);
    if (secret_key == NULL){
        fprintf(stderr, "Failed to load the secret key\n");
        rc = 0;
        goto end;
    }

    public_key = load_key_from_file(pk_filename);
    if (public_key == NULL){
        fprintf(stderr, "Failed to load the public key\n");
        rc = 0;
        goto end;
    }


    if (sign_RSA_PSS_SHA3_512(test_data, 
                              strlen(test_data), 
                              secret_key, 
                              &signature_length, 
                              &signature) != 1){
        fprintf(stderr, "Could not sign\n");
        rc = 0;
        goto end;
    }
    if (verify_signature_RSA_PSS_sha3_512(test_data,
                                          strlen(test_data),
                                          signature,
                                          signature_length,
                                          public_key)
                                        != 1){
        fprintf(stderr, "Could not verify signature\n");
        rc = 0;
        goto end;

    }

    if (verify_signature_RSA_PSS_sha3_512(test_data,
                                          strlen(test_data) - 1,
                                          signature,
                                          signature_length,
                                          public_key)
                                        != 0){
        fprintf(stderr, "Verified wrong signature\n");
        rc = 0;
        goto end;

    }

end:
    EVP_PKEY_free(secret_key);
    EVP_PKEY_free(public_key);
    OPENSSL_free(signature);
    if (rc == 1){
        fprintf(stdout, "Test Passed\n");
        return EXIT_SUCCESS;
    }
    else{
        fprintf(stdout, "Test Failed\n");
        return EXIT_FAILURE;
    }
}