#include "utils.h"
#include "common.h"


static const int MAX_FILE_SIZE = 20000;    // Not sending a file 
                                           // larger than this

int binary_to_base64(unsigned char *input, int input_len, unsigned char *output) {
    EVP_ENCODE_CTX *ctx;
    int out_len = 0;
    int final_len = 0;
    int total_len;
    
    if (input == NULL || output == NULL || input_len <= 0) {
        return -1;
    }

    ctx = EVP_ENCODE_CTX_new();
    if (ctx == NULL) {
        printf("Failed to create encoding context\n");
        return -1;
    }
    
    EVP_EncodeInit(ctx);

    if (EVP_EncodeUpdate(ctx, output, &out_len, input, input_len) == 0) {
        printf("Failed to encode data\n");
        EVP_ENCODE_CTX_free(ctx);
        return -1;
    }
    
    EVP_EncodeFinal(ctx, output + out_len, &final_len);
    total_len = out_len + final_len;

    EVP_ENCODE_CTX_free(ctx);
    
    return total_len;
}

int read_file(const char * filename, char **file_data){
    int fd;
    off_t file_size;
    ssize_t bytes_read = 0;
    ssize_t total_bytes_read = 0;

    fd = open(filename, O_RDONLY);
    if (fd < 0){
        fprintf(stderr, "Failed to open the file\n");
        return -1;
    }
    
    file_size = get_file_size(fd);
    if (file_size < 0){
        fprintf(stderr, "Error getting the size of the file\n");
        close(fd);
        return -1;
    }

    if (file_size > MAX_FILE_SIZE){
        fprintf(stderr, "File is too large\n");
        close(fd);
        return -1;
    }

    *file_data = (char*) malloc(sizeof(char) * file_size);
    if (*file_data == NULL){
        fprintf(stderr, "Failed to allocate memory\n");
        close(fd);
        return -1;
    }

    while (total_bytes_read < file_size){
        bytes_read = read(fd, *file_data + total_bytes_read, 4096);
        if (bytes_read <= 0 && total_bytes_read < file_size){
            fprintf(stderr, "Error reading from the file\n");
            free(*file_data);
            close(fd);
            return -1;
        }
        total_bytes_read += bytes_read;
    }

    close(fd);
    return file_size;
}

off_t get_file_size(int fd) {

    struct stat file_stat;
    if (fstat(fd, &file_stat) == -1) {
        perror("Error getting file stats");
        return -1;
    }

    return file_stat.st_size; 
}