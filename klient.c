#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>

#define PORT 9000
#define BUFFER_SIZE 1024

int main() {
    int sockfd;
    struct sockaddr_in serv_addr;
    struct hostent* server;
    char buffer[BUFFER_SIZE];
    int n;

    sockfd = socket(AF_INET, SOCK_STREAM, 0);
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

    if (connect(sockfd, (struct sockaddr*)&serv_addr, sizeof(serv_addr)) < 0) {
        perror("Błąd nawiązania połączenia (connect)!");
        exit(1);
    }

    printf("Pomyślnie zawiązano połączenie\n");

    memset(buffer, 0, BUFFER_SIZE);
    printf("Wpisz polecenie: ");
    fgets(buffer, BUFFER_SIZE - 1, stdin);

    n = write(sockfd, buffer, strlen(buffer));
    if (n < 0) {
        perror("Błąd odczytu z socketu");
        exit(1);
    }

    memset(buffer, 0, BUFFER_SIZE);
    n = read(sockfd, buffer, BUFFER_SIZE - 1);
    if (n < 0) {
        perror("Error reading from socket");
        exit(1);
    }

    printf("Odpowiedź z serwera:\n%s", buffer);

    close(sockfd);
    return 0;
}