#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <time.h>

#define PORT 8080
#define BUFFER_SIZE 1024

void receive_file(int socket_fd, int file_count) {
    char buffer[BUFFER_SIZE];
    char filename[100];
    time_t now = time(NULL);
    struct tm *t = localtime(&now);
    
    strftime(filename, sizeof(filename), "%Y%m%d_%H%M%S", t);
    sprintf(filename + strlen(filename), "_%d.txt", file_count);

    FILE *fp = fopen(filename, "wb");
    if (fp == NULL) {
        perror("File opening failed");
        return;
    }

    int bytes_received;
    while ((bytes_received = recv(socket_fd, buffer, BUFFER_SIZE, 0)) > 0) {
        fwrite(buffer, 1, bytes_received, fp);
    }

    fclose(fp);
    printf("File %d received and saved as: %s\n", file_count, filename);
}

int main() {
    int server_fd, new_socket;
    struct sockaddr_in address;
    int addrlen = sizeof(address);
    int file_count = 0;

    if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) == 0) {
        perror("Socket creation failed");
        exit(EXIT_FAILURE);
    }

    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(PORT);

    if (bind(server_fd, (struct sockaddr *)&address, sizeof(address)) < 0) {
        perror("Bind failed");
        exit(EXIT_FAILURE);
    }

    if (listen(server_fd, 3) < 0) {
        perror("Listen failed");
        exit(EXIT_FAILURE);
    }

    printf("Server listening on port %d\n", PORT);

    while (1) {
        if ((new_socket = accept(server_fd, (struct sockaddr *)&address, (socklen_t*)&addrlen)) < 0) {
            perror("Accept failed");
            continue;
        }

        file_count++;
        receive_file(new_socket, file_count);
        close(new_socket);
    }

    close(server_fd);
    return 0;
}
