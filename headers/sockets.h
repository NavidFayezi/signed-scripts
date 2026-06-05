#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <stdint.h>


int create_v4TCP_socket();
int send_over_v4TCP(int sockfd, const void *data, size_t data_len);
