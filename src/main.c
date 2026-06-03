#include "common.h"
#include "utils.h"
#include "hash.h"


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
