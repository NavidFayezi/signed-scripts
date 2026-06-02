#include <openssl/evp.h>
#include <stdio.h>
#include <openssl/err.h>
#include <openssl/rand.h>
#include <string.h>
#include <openssl/types.h>
#include <openssl/core.h>
#include <openssl/core_names.h>
#include "hash.h"


#define KEY_SIZE 16
#define IV_SIZE 12
#define TAG_SIZE 16

int binary_to_base64(unsigned char *input, 
                     int input_len, 
                     unsigned char *output);

int encrypt_aes_gcm(unsigned char *plaintext, 
                    int plaintext_len, 
                    unsigned char *key, 
                    unsigned char *iv, 
                    unsigned char *ciphertext, 
                    int *ciphertext_len,
                    unsigned char *auth_tag);

int decrypt_aes_gcm(unsigned char *ciphertext,
                    int ciphertext_len,
                    unsigned char *key,
                    unsigned char *iv,
                    unsigned char *plaintext,
                    int *plaintext_len,
                    unsigned char *auth_tag);



int main (int argc, char *argv[]){
    unsigned char data[] = {'l', 'k', 'd', 'f', 'j', 's', 'k', 'l', 'a', 'd'};
    unsigned char digest[64]; // SHA3-512 produces a 64-byte digest
    unsigned char base64_output[128]; // Base64 output buffer should be larger than binary input

    if (hash_sha3_512(data, sizeof(data), digest) == 0) {
        printf("Hashing failed\n");
        return EXIT_FAILURE;
    }

    printf("SHA3-512 Digest: ");
    binary_to_base64(digest, sizeof(digest), base64_output);
    printf("%s\n", base64_output);

    return EXIT_SUCCESS;
}





int old_main(int argc, char *argv[]){

    unsigned char key[KEY_SIZE];
    unsigned char iv[IV_SIZE];
    unsigned char plaintext[] = "Hello, World!";
    unsigned char ct_buffer[1024];
    unsigned char decrypted_text[1024];
    unsigned char auth_tag[16];
    unsigned char base64_output[2024]; // Base64 output buffer should be larger than binary input
    int ct_len;
    int base64_len;
    int decrypted_len;


    if (RAND_priv_bytes(key, KEY_SIZE) != 1) {
        printf("Failed to generate random key\n");
        ERR_print_errors_fp(stderr);
    }

    if (RAND_priv_bytes(iv, IV_SIZE) != 1) {
        printf("Failed to generate random IV\n");
        ERR_print_errors_fp(stderr);
    }

    if (encrypt_aes_gcm(plaintext, sizeof(plaintext), key, iv, ct_buffer, &ct_len, auth_tag) == 0) {
        printf("Encryption failed\n");
        return 1;
    }

    if (decrypt_aes_gcm(ct_buffer, ct_len, key, iv, decrypted_text, &decrypted_len, auth_tag) == 0) {
        printf("Decryption failed\n");
        return 1;   
    }
    


    base64_len = binary_to_base64(decrypted_text, decrypted_len, base64_output);
    if (base64_len < 0) {
        printf("Failed to encode to base64\n");
    }
    else{
        printf("Base64 Encoded Plaintext: %.*s\n", base64_len, base64_output);
    }

    return 0;
}

int decrypt_aes_gcm(unsigned char *ciphertext,
                    int ciphertext_len,
                    unsigned char *key,
                    unsigned char *iv,
                    unsigned char *plaintext,
                    int *plaintext_len, 
                    unsigned char *auth_tag) {
    EVP_CIPHER *cipher;
    EVP_CIPHER_CTX *ctx;
    int final_block_len;
    OSSL_PARAM params[2] = {
        OSSL_PARAM_END, OSSL_PARAM_END
    };

    ctx = EVP_CIPHER_CTX_new();
    if (ctx == NULL) {
        printf("Failed to create cipher context\n");
        ERR_print_errors_fp(stderr);
        return 0;
    }

    cipher = EVP_CIPHER_fetch(NULL, "AES-128-GCM", NULL);
    if (cipher == NULL) {
        printf("Failed to fetch cipher\n");
        ERR_print_errors_fp(stderr);
        EVP_CIPHER_CTX_free(ctx);
        return 0;
    }

    if (EVP_DecryptInit_ex2(ctx, cipher, key, iv, NULL) == 0) {
        printf("Failed to initialize decryption\n");
        ERR_print_errors_fp(stderr);
        EVP_CIPHER_CTX_free(ctx);
        return 0;
    }

    if (EVP_DecryptUpdate(ctx, plaintext, plaintext_len, ciphertext, ciphertext_len) == 0) {
        printf("Failed to decrypt data\n");
        ERR_print_errors_fp(stderr);
        EVP_CIPHER_CTX_free(ctx);
        return 0;
    }

    // Set the tag extracted from the input buffer
    params[0] = OSSL_PARAM_construct_octet_string(OSSL_CIPHER_PARAM_AEAD_TAG,
                                                  auth_tag, TAG_SIZE);
    if (!EVP_CIPHER_CTX_set_params(ctx, params)){
        printf("Failed to set the integrity tag\n");
        ERR_print_errors_fp(stderr);
        EVP_CIPHER_CTX_free(ctx);
        return 0;
    }


    if (EVP_DecryptFinal_ex(ctx, plaintext + (*plaintext_len), &final_block_len) == 0) {
        printf("Failed to finalize decryption\n");
        ERR_print_errors_fp(stderr);
        EVP_CIPHER_CTX_free(ctx);
        return 0;
    }

    *plaintext_len += final_block_len;

    return 1;
}


int encrypt_aes_gcm(unsigned char *plaintext, 
                    int plaintext_len, 
                    unsigned char *key, 
                    unsigned char *iv, 
                    unsigned char *ciphertext, 
                    int *ciphertext_len,
                    unsigned char *auth_tag) {
    EVP_CIPHER *cipher;
    EVP_CIPHER_CTX *ctx;
    int final_block_len;
    OSSL_PARAM params[2] = {
        OSSL_PARAM_END, OSSL_PARAM_END
    };


    ctx = EVP_CIPHER_CTX_new();
    if (ctx == NULL) {
        printf("Failed to create cipher context\n");
        ERR_print_errors_fp(stderr);
        return 0;
    }

    cipher = EVP_CIPHER_fetch(NULL, "AES-128-GCM", NULL);
    if (cipher == NULL) {
        printf("Failed to fetch cipher\n");
        ERR_print_errors_fp(stderr);
        EVP_CIPHER_CTX_free(ctx);
        return 0;
    }

    if (EVP_EncryptInit_ex2(ctx, cipher, key, iv, NULL) == 0) {
        printf("Failed to initialize encryption\n");
        ERR_print_errors_fp(stderr);
        EVP_CIPHER_CTX_free(ctx);
        return 0;
    }

    if (EVP_EncryptUpdate(ctx, ciphertext, ciphertext_len, plaintext, plaintext_len) == 0) {
        printf("Failed to encrypt data\n");
        ERR_print_errors_fp(stderr);
        EVP_CIPHER_CTX_free(ctx);
        return 0;
    }

    if (EVP_EncryptFinal_ex(ctx, ciphertext + (*ciphertext_len), &final_block_len) == 0) {
        printf("Failed to finalize encryption\n");
        ERR_print_errors_fp(stderr);
        EVP_CIPHER_CTX_free(ctx);
        return 0;
    }

    params[0] = OSSL_PARAM_construct_octet_string(OSSL_CIPHER_PARAM_AEAD_TAG,
        auth_tag, TAG_SIZE);

    if (!EVP_CIPHER_CTX_get_params(ctx, params)){
        printf("Failed to get the integrity tag\n");
        ERR_print_errors_fp(stderr);
        EVP_CIPHER_CTX_free(ctx);
        return 0;
    }

    *ciphertext_len += final_block_len;

    return 1;
}


int binary_to_base64(unsigned char *input, int input_len, unsigned char *output) {
    EVP_ENCODE_CTX *ctx;
    int out_len = 0;
    int final_len = 0;
    int total_len;
    
    if (input == NULL || output == NULL || input_len <= 0) {
        return -1;
    }
    
    // Create and initialize encoding context
    ctx = EVP_ENCODE_CTX_new();
    if (ctx == NULL) {
        printf("Failed to create encoding context\n");
        return -1;
    }
    
    EVP_EncodeInit(ctx);
    
    // Update with input data
    if (EVP_EncodeUpdate(ctx, output, &out_len, input, input_len) == 0) {
        printf("Failed to encode data\n");
        EVP_ENCODE_CTX_free(ctx);
        return -1;
    }
    
    // Finalize encoding
    EVP_EncodeFinal(ctx, output + out_len, &final_len);
    total_len = out_len + final_len;
    
    // Clean up
    EVP_ENCODE_CTX_free(ctx);
    
    return total_len;
}
