#include "common.h"
#include "utils.h"
#include "hash.h"
#include "signature.h"
#include "sockets.h"

#define SERVER_PORT 9999
#define SERVER_ADDRESS "127.0.0.1"


int load_key_and_sign(char * private_key_file, 
                       char * message, 
                       size_t message_len, 
                       size_t * signature_len,
                       unsigned char ** signature){

    EVP_PKEY * private_key = NULL;
    private_key = load_key_from_file(private_key_file);
    if (private_key == NULL){
        goto error;
    }

    if (sign_RSA_PSS_SHA3_512(message, 
                          message_len, 
                          private_key, 
                          signature_len, 
                          signature) != 1) {
        
        goto error;
    }

    EVP_PKEY_free(private_key);
    return 1;

error:
    // TODO
    EVP_PKEY_free(private_key);
    return 0;
}


int create_and_connect_socket(){
    int socket_fd = -1;
    struct sockaddr_in server_address;
    
    memset(&(server_address), 0, sizeof(server_address));
    server_address.sin_family = AF_INET;
    server_address.sin_port = hton(SERVER_PORT);
    if (inet_pton(AF_INET, SERVER_ADDRESS, &server_address.sin_addr) != 1){
        perror("creating address");
        goto error;
    }

    socket_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (socket_fd == -1){
        perror("creating socket");
    }

    if (
        connect(socket_fd, 
                (struct sockaddr *) &server_address, 
                sizeof(server_address)) != 0
    ) {
        goto error;
    }

    return socket_fd;

error:
    return -1;
}


int main (int argc, char *argv[]){
    char * scriptfile_name = NULL;
    int scriptfile_size;
    char * filedata = NULL;
    int socket_fd = -1;
    struct sockaddr_in server_address;

    if (argc < 2){
        fprintf(stderr, "Usage:\n./send_script filename\n");
        goto error;
    }

    scriptfile_name = argv[1]; // Assuming safe inputs for now
    scriptfile_size = read_file(scriptfile_name, &filedata);
    if (scriptfile_size == -1) {
        goto error;
    }

    if (create_and_connect_socket() == -1){
        goto error;
    }

    

error:
    if (socket_fd != -1){
        close(socket_fd);
    }
    return EXIT_FAILURE;
}
