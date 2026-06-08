#include "sockets.h"
#include "common.h"

int main(int argc, char *argv[]) {
    int socket_fd;
    struct sockaddr_in server_address;
    char *response;
    char *message = "Hello, Server!";
    uint16_t data_len_16;


    memset(&server_address, 0, sizeof(server_address));
    server_address.sin_family = AF_INET;
    server_address.sin_port = htons(20000);

    if (inet_pton(AF_INET, "127.0.0.1", &server_address.sin_addr) <= 0){
        goto error;
    }
    
    socket_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (socket_fd < 0) {
        goto error;
    }

    if (
        connect(socket_fd, 
                (struct sockaddr *) &server_address, 
                sizeof(server_address)) < 0
    ) {
        goto error;
    }

    if (send_over_v4TCP(socket_fd, message, strlen(message)) < 0) {
        goto error;
    }

    response = recv_over_v4TCP(socket_fd, &data_len_16);
    if (response == NULL) {
        goto error;
    }

    if (
        strlen(message) != (size_t) data_len_16 
        || strncmp(response, message, strlen(message)) != 0
    ) {
        printf("Test Failed\n");
    }
    else {
        printf("Test Passed\n");
    }

    free(response);
    if (close(socket_fd) < 0) {
        goto error;
    }

    return EXIT_SUCCESS;

error:
    printf("Test Failed \nError: %s\n", strerror(errno));
    return EXIT_FAILURE;
}