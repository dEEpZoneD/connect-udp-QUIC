#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <arpa/inet.h>

#define PORT 8888
#define MAXLINE 1024

int sockfd;

static void signal_handler(int signum) {
	if (signum == SIGINT || signum == SIGTERM) {
		printf("\n\nSignal %d received, preparing to exit...\n", signum);
        close(sockfd);
        exit(0);
	}
}

void reverse_string(char *words) {
    int j = strlen(words);
    int i=0;
    j--;
    char temp;
    while (i < j) {
        temp = words[i];
        words[i] = words[j];
        words[j] = temp;
        i++;
        j--;
    }
}

int main() { 
    char buffer[MAXLINE];
    struct sockaddr_in servaddr, cliaddr;
    socklen_t len;

    signal(SIGINT, signal_handler);
    signal(SIGTERM, signal_handler);

    // Create a UDP socket
    if ((sockfd = socket(AF_INET, SOCK_DGRAM, 0)) < 0) {
        perror("socket creation failed");
        exit(EXIT_FAILURE);
    }

    memset(&servaddr, 0, sizeof(servaddr));
    memset(&cliaddr, 0, sizeof(cliaddr));

    // Server information
    servaddr.sin_family = AF_INET; // IPv4
    servaddr.sin_addr.s_addr = INADDR_ANY; // Accept packets on any interface
    servaddr.sin_port = htons(PORT); // Server port

    // Bind the socket to the server address
    if (bind(sockfd, (const struct sockaddr *)&servaddr, sizeof(servaddr)) < 0) {
        perror("bind failed");
        exit(EXIT_FAILURE);
    }

    char ip_str[INET_ADDRSTRLEN]; // Buffer to store IP as string
    uint16_t portf = ntohs(servaddr.sin_port); // Convert port to host byte order

    inet_ntop(AF_INET, &(servaddr.sin_addr), ip_str, INET_ADDRSTRLEN); // Convert IP to string

    printf("sockaddr_in Information:\n");
    printf("  Family: AF_INET (IPv4)\n");
    printf("  Port: %hu\n", portf);
    printf("  Address: %s\n", ip_str);


    printf("UDP Server is listening on port %s:%d...\n", ip_str, portf);

    while (1) {
        // Receive a UDP packet
        len = sizeof(cliaddr);
        int n = recvfrom(sockfd, (char *)buffer, MAXLINE, 0, (struct sockaddr *)&cliaddr, &len);

        if (n > 0) {
            buffer[n] = '\0'; // Null-terminate the received data
            printf("Received packet from %s:%d:\n %s\n", inet_ntoa(cliaddr.sin_addr), ntohs(cliaddr.sin_port), buffer);
        } 
        else {
            perror("recvfrom");
        }
        
        reverse_string(buffer);

        n = sendto(sockfd, buffer, strlen(buffer), 0, (struct sockaddr *) &cliaddr, len);
        if (n < 0) {
            perror("sendto");
            exit(1);
        }

        memset(buffer, 0, sizeof(buffer));
    }

    close(sockfd);

    return 0;
}
