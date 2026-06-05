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

int send_over_v4TCP(int sockfd, const void *data, size_t data_len) {

    ssize_t bytes_sent = 0;
    ssize_t total_bytes_sent = 0;
    uint16_t data_len_16;
    
    if (data_len > UINT16_MAX) {
        printf("Data length exceeds maximum allowed size\n");
        return -1;
    }

    // Convert data length to network byte order
    // and send it before sending the actual data
    // data length is only two bytes
    data_len_16 = htons((uint16_t) data_len);
    while (total_bytes_sent < sizeof(data_len_16)) {
        bytes_sent = send(sockfd,
                           ((char *) &data_len_16) + bytes_sent, 
                           sizeof(data_len_16) - bytes_sent, 
                           0);

        if (bytes_sent < 0) {
            printf("Error sending data length\n");
            return -1;
        }
        total_bytes_sent += bytes_sent;
    }

    total_bytes_sent = 0;
    bytes_sent = 0;

    // Sending the actual data
    while ((size_t) total_bytes_sent < data_len) {
        bytes_sent = send(sockfd, (data + bytes_sent), data_len - bytes_sent, 0);
        if (bytes_sent < 0) {
            printf("Error sending data\n");
            return -1;
        }
        total_bytes_sent += bytes_sent;
    }

    return 0;
}