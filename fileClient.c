#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>

#define PORT 8080
#define BUFFER_SIZE 1024

void send_file(int socket_fd, const char *filename) {
    char buffer[BUFFER_SIZE];
    FILE *fp = fopen(filename, "rb");
    if (fp == NULL) {
        perror("File opening failed");
        exit(1);
    }

    while (1) {
        int bytes_read = fread(buffer, 1, BUFFER_SIZE, fp);
        if (bytes_read <= 0) break;
        send(socket_fd, buffer, bytes_read, 0);
    }

    fclose(fp);
    printf("File sent successfully\n");
}

int main(int argc, char *argv[]) {
    if (argc != 3) {
        fprintf(stderr, "Usage: %s <IP address> <filename>\n", argv[0]);
        exit(1);
    }

    int sock = 0;
    struct sockaddr_in serv_addr;

    if ((sock = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        printf("\n Socket creation error \n");
        return -1;
    }

    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(PORT);

    if (inet_pton(AF_INET, argv[1], &serv_addr.sin_addr) <= 0) {
        printf("\nInvalid address/ Address not supported \n");
        return -1;
    }

    if (connect(sock, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0) {
        printf("\nConnection Failed \n");
        return -1;
    }

    send_file(sock, argv[2]);

    close(sock);
    return 0;
}
