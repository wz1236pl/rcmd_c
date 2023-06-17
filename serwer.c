#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>

#define PORT 9000
#define BUFFER_SIZE 1024

int execute_command(char* command, char* output) {
    FILE* fp;
    char line[BUFFER_SIZE];
    char* result = output;

    fp = popen(command, "r");
    if (fp == NULL) {
        return -1;
    }

    while (fgets(line, sizeof(line), fp) != NULL) {
        strcat(result, line);
    }

    pclose(fp);
    return 0;
}

int main() {
    int sockfd;
    char buffer[BUFFER_SIZE];
    struct sockaddr_in serv_addr, cli_addr;
    socklen_t cli_addr_len;
    int n;

    if ((sockfd = socket(AF_INET, SOCK_DGRAM, 0)) < 0) {
        perror("Błąd przy otwieraniu socketu");
        exit(1);
    }

    memset(&serv_addr, 0, sizeof(serv_addr));

    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = INADDR_ANY;
    serv_addr.sin_port = htons(PORT);

    if (bind(sockfd, (struct sockaddr*)&serv_addr, sizeof(serv_addr)) < 0) {
        perror("Błąd nawiązania połączenia (bind)!");
        exit(1);
    }

    printf("Serwer nasłuchuje portu %d...\n", PORT);

    while (1) {
        memset(buffer, 0, BUFFER_SIZE);
        cli_addr_len = sizeof(cli_addr);
        n = recvfrom(sockfd, buffer, BUFFER_SIZE - 1, 0, (struct sockaddr*)&cli_addr, &cli_addr_len);
        if (n < 0) {
            perror("Błąd odczytu z socketu");
            exit(1);
        }

        printf("Nawiązano połączenie\n");

        char output[BUFFER_SIZE];
        memset(output, 0, BUFFER_SIZE);
        execute_command(buffer, output);

        n = sendto(sockfd, output, strlen(output), 0, (struct sockaddr*)&cli_addr, sizeof(cli_addr));
        if (n < 0) {
            perror("Błąd zapisu do socketu");
            exit(1);
        }

        printf("Polecenie wykonane, wysyłanie odpowiedzi!\n");
    }

    close(sockfd);
    return 0;
}