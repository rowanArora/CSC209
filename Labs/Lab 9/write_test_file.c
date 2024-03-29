#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>

/* Write 100 integers (in binary) to a file with the name given by the command-line
 * argument.  You can write them from 0 to 99 or use random to write random numbers.
 * This program creates a data file for use by the time_reads program.
 */

int main(int argc, char **argv) {
    if (argc != 2) {
        fprintf(stderr, "Usage: write_test_file filename\n");
        exit(1);
    }

    FILE *fp;
    if ((fp = fopen(argv[1], "w")) == NULL) {
        perror("fopen");
        exit(1);
    }

    // TODO: complete this program according its description above.
    int *num = malloc(sizeof(int) * 100);

    for (int x = 0; x < 100; ++x) {
        num[x] = rand() % 100;
    }
    fwrite(num, sizeof(int), 100, fp);
    fclose(fp);
    return 0;
}
