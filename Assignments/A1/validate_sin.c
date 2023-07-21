#include <stdio.h>
#include <stdlib.h>

int populate_array(int, int *);
int check_sin(int *);


int main(int argc, char **argv) {
    if (argc != 2) {
        return 2;
    }

    int sin = strtol(argv[1], NULL, 10);
    int sin_array[9];

    populate_array(sin, sin_array);
    int valid = check_sin(sin_array);

    if(valid == 0) {
        printf("Valid SIN\n");
    }
    else {
        printf("Invalid SIN\n");
    }

    return valid;
}
