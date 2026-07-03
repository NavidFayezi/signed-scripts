#include "common.h"
#include "signature.h"
#include "utils.h"


int main(int argc, char *argv[]){
    const char * sk_filename = "keys/private_key.pem";
    const char * pk_filename = "keys/public_key.pem";
    const char * test_data = NULL;
    unsigned char * signature = NULL;
    size_t signature_length = 0;
    int rc = 1;
    EVP_PKEY * secret_key = NULL;
    EVP_PKEY * public_key = NULL;
    ssize_t data_length = 0;

    data_length = read_file("scripts/script1.sh", (char **) &test_data);
    if (data_length == -1){
        fprintf(stderr, "Failed to read the script\n");
        rc = 0;
        goto end;
    }

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
                              data_length, 
                              secret_key, 
                              &signature_length, 
                              &signature) != 1){
        fprintf(stderr, "Could not sign\n");
        rc = 0;
        goto end;
    }
    if (verify_signature_RSA_PSS_sha3_512(test_data,
                                          data_length,
                                          signature,
                                          signature_length,
                                          public_key)
                                        != 1){
        fprintf(stderr, "Could not verify signature\n");
        rc = 0;
        goto end;

    }

    if (verify_signature_RSA_PSS_sha3_512(test_data,
                                          data_length - 1,
                                          signature,
                                          signature_length,
                                          public_key)
                                        != 0){
        fprintf(stderr, "Verified wrong signature\n");
        rc = 0;
        goto end;

    }

end:
    free((void *) test_data);
    EVP_PKEY_free(secret_key);
    EVP_PKEY_free(public_key);
    OPENSSL_free(signature);
    if (rc == 1){
        fprintf(stdout, "=====Test Passed=====\n");
        return EXIT_SUCCESS;
    }
    else{
        fprintf(stdout, "=====Test Failed=====\n");
        return EXIT_FAILURE;
    }
}