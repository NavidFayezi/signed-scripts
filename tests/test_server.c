#include "sockets.h"
#include "common.h"


int main(int argc, char *argv[]){
    int socket_fd, client_socket_fd;
    struct sockaddr_in server_address, client_address;
    socklen_t client_address_len = sizeof(client_address);
    uint16_t received_data_len;
    char *received_data;

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
        bind(socket_fd, 
             (struct sockaddr *) &server_address, 
             sizeof(server_address)) < 0
    ) {
        goto error;
    }

    if (listen(socket_fd, 1) < 0) {
        goto error;
    }

    client_socket_fd = accept(socket_fd, 
                              (struct sockaddr *) &client_address, 
                              &client_address_len);
    if (
        client_socket_fd < 0
    ) {
        goto error;
    }
    received_data = recv_over_v4TCP(client_socket_fd, &received_data_len);
    if (received_data == NULL) {
        goto error;
    }
    if (
        send_over_v4TCP(client_socket_fd, 
                        received_data, 
                        received_data_len) < 0
    ) {
        goto error;
    }
    free(received_data);
    if (close(client_socket_fd) < 0) {
        goto error;
    }
    if (close(socket_fd) < 0) {
        goto error;
    }
    return EXIT_SUCCESS;


error:
    printf("Error: %s\n", strerror(errno));
    return EXIT_FAILURE;
}