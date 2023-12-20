#include <stdio.h>
#include <string.h>
#include <sys/socket.h>
#include <netdb.h>
#include <unistd.h>

#define PORT_NUMBER "69" // Define PORT_NUMBER in the function getaddrinfo
#define SOCKET_ERROR_HANDLER (-1) // Define SOCKET_ERROR_HANDLER value
#define SENDTO_ERROR_HANDLER (-1) // Define SENDTO_ERROR_HANDLER value

#define BUFFER_SIZE 516 // Define BUFFER_SIZE, here 512 bytes of data + 4 bytes for the header
#define BUFFER_OFFSET 2 // Define BUFFER_OFFSET to reach the filname-part
#define END_OF_FILE_BUFFER_OFFSET 1 // Define END_OF_FILE_BUFFER_OFFSET to reach the EOF-part

#define TFTP_OPCODE_WRQ 2 // WRQ Code for the TFTP Server
#define TFTP_OPCODE_DATA 3 // Data Code for the TFTP Server
#define TFTP_OPCODE_ACK 4 // ACK Code for the TFTP Server

#define FOPEN_MODE "wb" // Define the fopen function mode, "wb" is to open an empty file and write in it
#define RECV_FLAG_QUANTITY 0 // Define the number of flags waited on the received message, here none

int main(int argc, char *argv[]) {
    if (argc != 3) { // Error handler if not enough arguments
        fprintf(stderr, "Usage: %s <server> <file>\n", argv[0]);
        return 1;
    }

    //Retrieved values
    char *server = argv[1];
    char *file = argv[2];

    struct addrinfo hints, *res;
    int sockfd;

    // Setup hints for getaddrinfo
    memset(&hints, 0, sizeof hints);
    hints.ai_family = AF_INET; // AF_INET si to force IPV4 version
    hints.ai_socktype = SOCK_DGRAM; // TFTP is over UDP
    hints.ai_protocol= 0;

    // Get server address info
    if (getaddrinfo(server, PORT_NUMBER, &hints, &res) != 0) { // getaddrinfo error handler
        perror("getaddrinfo");
        return 2;
    }

    // Create a socket
    sockfd = socket(res->ai_family, res->ai_socktype, res->ai_protocol);
    if (sockfd == SOCKET_ERROR_HANDLER) { // Socket error handler
        perror("socket");
        freeaddrinfo(res);
        return 3;
    }

    printf("Uploading file '%s' to server '%s'\n", file, server);

    // Build and send WRQ packet
    char buffer[BUFFER_SIZE];
    int opcode = htons(TFTP_OPCODE_WRQ);
    memcpy(buffer, &opcode, sizeof(opcode));
    strcpy(buffer + BUFFER_OFFSET, file);
    strcpy(buffer + 3 + strlen(file), "octet");
    int len = 3 + strlen(file) + 1 + strlen("octet") + 1;

    if (sendto(sockfd, buffer, len, 0, res->ai_addr, res->ai_addrlen) == SENDTO_ERROR_HANDLER) {
        perror("sendto");
        close(sockfd);
        freeaddrinfo(res);
        return 4;
    }

    // Open file
    FILE *fp = fopen(file, "rb");
    if (!fp) {
        perror("fopen");
        close(sockfd);
        freeaddrinfo(res);
        return 5;
    }

    // Send file in blocks
    int block_num = 0;
    struct sockaddr_storage from_addr;
    socklen_t from_len = sizeof(from_addr);
    int read_bytes, sent_bytes, received_bytes;

    do {
        read_bytes = fread(buffer + 4, 1, 512, fp);
        block_num++;

        // Prepare and send data packet
        int opcode = htons(TFTP_OPCODE_DATA);
        memcpy(buffer, &opcode, sizeof(opcode));
        buffer[2] = (block_num >> 8) & 0xFF;
        buffer[3] = block_num & 0xFF;


        sent_bytes = sendto(sockfd, buffer, read_bytes + 4, 0, res->ai_addr, res->ai_addrlen);
        if (sent_bytes == SENDTO_ERROR_HANDLER) {
            perror("sendto");
            fclose(fp);
            close(sockfd);
            freeaddrinfo(res);
            return 6;
        }

        // Wait for ACK
        received_bytes = recvfrom(sockfd, buffer, BUFFER_SIZE, 0, (struct sockaddr *)&from_addr, &from_len);
        if (received_bytes == SOCKET_ERROR_HANDLER) {
            perror("recvfrom");
            fclose(fp);
            close(sockfd);
            freeaddrinfo(res);
            return 7;
        }

        if (buffer[1] != TFTP_OPCODE_ACK || buffer[BUFFER_OFFSET] != (block_num >> 8) || buffer[3] != (block_num & 0xFF)) {
            fprintf(stderr, "Error: Invalid ACK received\n");
            fclose(fp);
            close(sockfd);
            freeaddrinfo(res);
            return 8;
        }

    } while (read_bytes == 512);

    fclose(fp);
    close(sockfd);
    freeaddrinfo(res);

    return 0;
}

