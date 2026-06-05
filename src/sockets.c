#include "sockets.h"

int create_v4TCP_socket() {
    int socket_fd;
    socket_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (socket_fd < 0) {
        printf("Error creating socket\n");
        return -1;
    }

    return socket_fd;
}

int send_over_v4TCP(int socket_fd, void *data, size_t data_len) {

    uint16_t data_len_16;
    
    if (data_len > UINT16_MAX) {
        printf("Data length exceeds maximum allowed size\n");
        return -1;
    }

    // Convert data length to network byte order
    // and send it before sending the actual data
    // data length is only two bytes
    data_len_16 = htons((uint16_t) data_len);
    if (
        send_loop(socket_fd,
                  &data_len_16, 
                  sizeof(data_len_16)) < 0
                ) {
        printf("Error sending data length\n");
        return -1;
    }

    // Sending the actual data
    if (send_loop(socket_fd, data, data_len) < 0) {
        printf("Error sending data\n");
        return -1;
    }

    return 0;
}

char *recv_over_v4TCP(int socket_fd){
    uint16_t data_len_16;
    ssize_t bytes_received = 0;
    ssize_t total_bytes_received = 0;
    char *data_buffer = NULL;

    // First, receive the length of the incoming data (represented in 2 bytes)
    if (recv_loop(socket_fd, 
                  (char *) &data_len_16, 
                  sizeof(data_len_16), 
                  &bytes_received, 
                  &total_bytes_received) < 0) {
        printf("Error receiving data length\n");
        return NULL;
    }

    data_len_16 = ntohs(data_len_16);
    data_buffer = (char *) malloc(data_len_16);
    if (data_buffer == NULL) {
        printf("Memory allocation failed\n");
        return NULL;
    }

    // Receive the actual data 
    total_bytes_received = 0;
    bytes_received = 0;
    if (
        recv_loop(socket_fd, 
                  data_buffer, 
                  data_len_16, 
                  &bytes_received, 
                  &total_bytes_received) < 0
                ) {
        printf("Error receiving data\n");
        free(data_buffer);
        return NULL;
    }

    return data_buffer;
}


int send_loop(int socket_fd, 
              void *data, 
              size_t data_len) {

    ssize_t bytes_sent = 0;
    ssize_t total_bytes_sent = 0;

    while ((size_t) total_bytes_sent < data_len) {
        bytes_sent = send(socket_fd,
                          ((char *)data + total_bytes_sent),
                          data_len - total_bytes_sent, 
                          0);

        if (bytes_sent < 0) {
            return -1;
        }
        total_bytes_sent += bytes_sent;
    }

    return 0;
}


int recv_loop(int socket_fd, 
              void *buffer, 
              size_t buffer_size,
              ssize_t * bytes_received,
              ssize_t * total_bytes_received) {


    while ((size_t) *total_bytes_received < buffer_size) {
        *bytes_received = recv(socket_fd, 
                              (char *)buffer + (*total_bytes_received), 
                              buffer_size - (*total_bytes_received), 
                              0);
        if (*bytes_received < 0) {
            return -1;
        }
        *total_bytes_received += (*bytes_received);
    }

    return 0;
}