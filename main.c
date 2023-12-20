#include <stdio.h>
#include <string.h>

void gettftp(const char *server, const char *file);
void puttftp(const char *server, const char *file);

int main(int argc, char *argv[]) {
    if (argc != 4) {
        fprintf(stderr, "Usage: %s <get/put> <server> <file>\n", argv[0]);
        return 1;
    }

    if (strcmp(argv[1], "get") == 0) {
        gettftp(argv[2], argv[3]);
    } else if (strcmp(argv[1], "put") == 0) {
        puttftp(argv[2], argv[3]);
    } else {
        fprintf(stderr, "Invalid command. Use 'get' or 'put'.\n");
        return 1;
    }

    return 0;
}