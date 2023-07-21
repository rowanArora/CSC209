#include <stdio.h>
#include <stdlib.h>

/*
 * Return the number at the input index, the full string, or an error message.
 */
void phone() {
    char phone[11];
    int index;
    scanf("%s %d", phone, &index);

    if (index < -1 || index > 9) {
        printf("ERROR\n");
    }
    else if (index == -1) {
        printf("%s\n", phone);
    }
    else {
        printf("%c\n", phone[index]);
    }
}

/*
 * Sample usage:
 * $ gcc -Wall -std=gnu99 -g -o phone phone.c
 * $ ./phone
 * Entered: 4161234567 3
 * Result: 1
 */
int main(int argc, char **argv) {
    // Call phone to trigger scanf.
    phone();

    return 0;
}