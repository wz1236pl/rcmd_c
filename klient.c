#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <time.h>

#define PORT 9000
#define BUFFER_SIZE 1024

int main() {
    int sockfd;
    struct sockaddr_in serv_addr;
    struct hostent* server;
    char buffer[BUFFER_SIZE];
    int n;
    time_t now;
    struct tm *timeinfo;
    char timeStr[80];

    sockfd = socket(AF_INET, SOCK_DGRAM, 0);
    if (sockfd < 0) {
        perror("Błąd przy otwieraniu socketu");
        exit(1);
    }

    server = gethostbyname("localhost");
    if (server == NULL) {
        perror("Error: Nie ma takiego hosta");
        exit(1);
    }

    memset(&serv_addr, 0, sizeof(serv_addr));

    serv_addr.sin_family = AF_INET;
    memcpy(&serv_addr.sin_addr.s_addr, server->h_addr, server->h_length);
    serv_addr.sin_port = htons(PORT);

    printf("Wpisz polecenie: ");
    fgets(buffer, BUFFER_SIZE - 1, stdin);

    n = sendto(sockfd, buffer, strlen(buffer), 0, (struct sockaddr*)&serv_addr, sizeof(serv_addr));
    if (n < 0) {
        perror("Błąd przy wysyłaniu do socketu");
        exit(1);
    }

    memset(buffer, 0, BUFFER_SIZE);
    socklen_t serv_addr_len = sizeof(serv_addr);
    n = recvfrom(sockfd, buffer, BUFFER_SIZE - 1, 0, (struct sockaddr*)&serv_addr, &serv_addr_len);
    if (n < 0) {
        perror("Błąd przy odczycie z socketu");
        exit(1);
    }

    printf("Odpowiedź z serwera:\n%s", buffer);


    time(&now);
    timeinfo = localtime(&now);
    strftime(timeStr, sizeof(timeStr), "%Y-%m-%d %H:%M:%S", timeinfo);

    FILE *file = fopen(strcat(timeStr, "-Wynik.txt"), "w");
    if (file == NULL) {
        printf("Nie można otworzyć pliku.\n");
        close(sockfd);
        return 1;
    }
    fwrite(buffer, sizeof(char), sizeof(buffer), file);
    fclose(file);

    close(sockfd);
    return 0;
}