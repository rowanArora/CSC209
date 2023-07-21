#include <stdio.h>
#include <stdlib.h>

int check_permissions(char *current, char *required);

int main(int argc, char** argv) {
    if (!(argc == 2 || argc == 3)) {
        fprintf(stderr, "USAGE: count_large size [permissions]\n");
        return 1;
    }

    int cutoff = strtol(argv[1], NULL, 10);
    char excess[256];
    char permissions[11];
    int foo;
    char owner[32];
    char group[32];
    int size;
    int lineNum = 0;
    int counter = 0;

    while (1 == 1) {
        if (lineNum == 0) {
            scanf("%[^\n]\n", excess);
        }
        else {
            if (scanf("%s %d %s %s %d%[^\n]\n", permissions, &foo, owner, group, &size, excess) <= 0) {
                break;
            }
        }
        if (size > cutoff && argc == 3) {
                counter = counter + check_permissions(permissions, argv[2]);
        }
        else if (size > cutoff && argc == 2) {
            counter++;
        }
        lineNum++;
    }
    printf("%d\n", counter);
    return 0;
}

int check_permissions(char *current, char *required) {

    for (int x = 0; x < 8; ++x) {
        if (required[x] != '-' && current[x] != required[x]) {
            return 1;
        }
    }
    return 0;
}
