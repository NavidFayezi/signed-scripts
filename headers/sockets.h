#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <stdint.h>


int create_v4TCP_socket();
int send_over_v4TCP(int socket_fd, const void *data, size_t data_len);
char *recv_over_v4TCP(int socket_fd);
int recv_loop(int socket_fd, 
              void *buffer, 
              size_t buffer_size,
              ssize_t * bytes_received,
              ssize_t * total_bytes_received);
int send_loop(int socket_fd, 
              void *data, 
              size_t data_len);
