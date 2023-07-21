#include <stdio.h>
#include <stdlib.h>

/*
 *  Return the number at the current index where the index is constantly being updated.
*/
int phone_loop() {
    char phone[11];
    int current_index;
    int contains_error = 0;
    scanf("%s", phone);

    while (scanf("%d", &current_index) != EOF) {
        if (current_index < -1 || current_index > 9) {
            printf("ERROR\n");
            contains_error = 1;
            break;
        }
        else if (current_index == -1) {
            printf("%s\n", phone);
        }
        else {
            printf("%c\n", phone[current_index]);
        }
    }

    return contains_error;

}

/*
 * Sample usage:
 * $ gcc -Wall -std=gnu99 -g -o phone_loop phone_loop.c
 * $ ./phone_loop
 * Entered: 4147891234
 *          3
 * Result:  7
 * Entered: 6
 * Result:  1
 */
int main(int argc, char **argv) {
    // Call phone to trigger scanf.
    int contains_error = phone_loop();

    return contains_error;
}