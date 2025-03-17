#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <netinet/in.h>
#include <unistd.h>
#include <fcntl.h>
#include "include/sans.h"


int http_server(const char* iface, int port) 
{
    // Buffer to store incoming data
    char buffer[1024];
    // Accept connection from the client
    int sockfd = sans_accept(iface, port, IPPROTO_TCP);
    // if (client_fd < 0) {
    //     fprintf(stderr, "Error: could not accept connection\n");
    //     return -1;
    // }

    // Receive data packet from the client
    int number_of_bytes_received=sans_recv_pkt(sockfd, buffer, sizeof(buffer)-1);
    buffer[number_of_bytes_received]='\0';
    char file_path[1000];
    sscanf(buffer, "GET %s HTTP/1.1", file_path);
    //open requested file
    FILE *file_fd;
    char response[1024];
    file_fd = fopen(file_path + 1, "r");
    if (file_fd  == NULL) {
        snprintf(response, sizeof(response),
                 "HTTP/1.1 404 Not Found\r\n"
                 "Content-Type: text/html; charset=utf-8\r\n"
                 "Content-Length: 0\r\n"
                 "\r\n");
        sans_send_pkt(sockfd, response, strlen(response));
        sans_disconnect(sockfd);
        return -1;
    }
    struct stat file_stat;
    stat(file_path + 1, &file_stat);
    // if (fstat(file_fd, &file_stat) < 0) {
    //     perror("fstat");
    //     close(file_fd);
    //     sans_disconnect(sockfd);
    //     return;
    // }
    ssize_t file_size;
    file_size = file_stat.st_size;

 // Send HTTP response with file content
    ssize_t bytes_sent;
    snprintf(response, sizeof(response),
             "HTTP/1.1 200 OK\r\n"
             "Content-Type: text/html; charset=utf-8\r\n"
             "Content-Length: %ld\r\n"
             "\r\n", file_size);
    bytes_sent = sans_send_pkt(sockfd, response, strlen(response));
    if (bytes_sent < 0) {
        perror("sans_send_pkt");
        sans_disconnect(sockfd);
        return -1;
    }
   char file_buffer[1024];
    ssize_t read_bytes;
    while ((read_bytes = fread(file_buffer, 1, sizeof(file_buffer) - 1, file_fd)) > 0) 
    {
        file_buffer[read_bytes] = '\0';  // Null-terminate the buffer
        if (sans_send_pkt(sockfd, file_buffer, read_bytes) < 0) 
        {
            perror("sans_send_pkt failed");
            fclose(file_fd);
            sans_disconnect(sockfd);
            return 0;
        }
    }
    sans_disconnect(sockfd);
    return 0;
}