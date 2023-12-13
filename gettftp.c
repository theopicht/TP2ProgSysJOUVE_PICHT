//
// Created by Théo PICHT on 13/12/2023.
//
#include <stdio.h>
#include <stdlib.h>

int main(int argc, char *argv[]) {
    if (argc != 3) { // Error handler if not enough arguments
        fprintf(stderr, "Usage: %s <server> <file>\n", argv[0]);
        return 1;
    }

    //Retrieved values
    char *server = argv[1];
    char *file = argv[2];

    return 0;
}
