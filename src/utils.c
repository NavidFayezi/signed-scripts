#include "utils.h"
#include "common.h"

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