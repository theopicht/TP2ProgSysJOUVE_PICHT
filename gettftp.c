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

#define TFTP_OPCODE_RRQ 1 // RRQ Code for the TFTP Server
#define TFTP_OPCODE_DATA 3 // Data Code for the TFTP Server
#define TFTP_OPCODE_ACK 4 // ACK Code for the TFTP Server

#define FOPEN_MODE "wb" // Define the fopen function mode, "wb" is to open an empty file and write in it
#define RECV_FLAG_QUANTITY 0 // Define the number of flags waited on the received message, here none

int main(int argc, char *argv[]) {
    if (argc != 3) { // Error handler if not enough arguments
        fprintf(stderr, "Usage: %s <server> <file>\n", argv[0]);
        return 1;
    }

    // Retrieved values
    char *server = argv[1];
    char *file = argv[2];

    // Create an struct which handle our hints
    struct addrinfo hints, *res;

    // Setup hints for getaddrinfo
    memset(&hints, 0, sizeof hints);
    hints.ai_family = AF_INET; // AF_INET si to force IPV4 version
    hints.ai_socktype = SOCK_DGRAM; // TFTP is over UDP
    hints.ai_protocol= 0;

    // Get server address info
    if (getaddrinfo(server, PORT_NUMBER, &hints, &res) != 0) {
        perror("getaddrinfo");
        return 2;
    }

    // Create a socket
    int sockfd;

    sockfd = socket(res->ai_family, res->ai_socktype, res->ai_protocol);
    if (sockfd == SOCKET_ERROR_HANDLER) { // Socket error handler
        perror("socket");
        freeaddrinfo(res);
        return 3;
    }

    printf("Downloading file '%s' from server '%s'\n", file, server);
    // Implement the TFTP download logic here

    // Build and send RRQ packet
    char buffer[BUFFER_SIZE];

    // Manually construct the RRQ packet
    int opcode = htons(TFTP_OPCODE_RRQ); // Convert the opcode to network byte order
    memcpy(buffer, &opcode, sizeof(opcode)); // Copy opcode to the buffer
    strcpy(buffer + BUFFER_OFFSET, file); // Copy file name to the buffer
    strcpy(buffer + BUFFER_OFFSET + strlen(file) + END_OF_FILE_BUFFER_OFFSET, "octet"); // Copy mode to the buffer after the file name

    int len = BUFFER_OFFSET + strlen(file) + END_OF_FILE_BUFFER_OFFSET + strlen("octet") + 1;

    if (sendto(sockfd, buffer, len, 0, res->ai_addr, res->ai_addrlen) == SENDTO_ERROR_HANDLER) { // Sendto error handler
        perror("sendto");
        close(sockfd);
        freeaddrinfo(res);
        return 4;
    }

    // Receive data and send ACK
    struct sockaddr_storage from_addr;
    socklen_t from_len = sizeof(from_addr);
    int received_bytes, block_num = 0;

    FILE *received_file = fopen(file, FOPEN_MODE);
    if (!received_file) { // Fopen error handler
        perror("fopen");
        close(sockfd);
        freeaddrinfo(res);
        return 5;
    }

    do {
        received_bytes = recvfrom(sockfd, buffer, BUFFER_SIZE, RECV_FLAG_QUANTITY, (struct sockaddr *)&from_addr, &from_len);
        if (received_bytes < 0) {
            perror("recvfrom");
            fclose(received_file);
            close(sockfd);
            freeaddrinfo(res);
            return 6;
        }

        if (buffer[1] == TFTP_OPCODE_DATA) { // If TFTP data are sent 
            int received_block_num = (buffer[BUFFER_OFFSET] << 8) | buffer[3];
            if (received_block_num == block_num + 1) {
                fwrite(buffer + 4, 1, received_bytes - 4, received_file);
                block_num++;

                // Send ACK
                buffer[1] = TFTP_OPCODE_ACK; // Assignation of ACK OPCODE
                if (sendto(sockfd, buffer, 4, 0, (struct sockaddr *)&from_addr, from_len) < 0) { // All error code are below 0 value
                    perror("sendto");
                    fclose(received_file);
                    close(sockfd);
                    freeaddrinfo(res);
                    return 7;
                }
            }
        }
    } while (received_bytes == BUFFER_SIZE);

    fclose(received_file);
    close(sockfd);
    freeaddrinfo(res);

    return 0;
}