#include <stdlib.h>
#include <stdio.h>

// TODO: Implement populate_array
/*
 * Convert a 9 digit int to a 9 element int array.
 */
int populate_array(int sin, int *sin_array) {
    int size = 0;
    int n = sin;
    int temp[9];

    while (n != 0)
    {
        n /= 10;
        size++;
    }

    if (size != 9) {
        return 1;
    }

    // sin_array = malloc(sizeof(int) * size);

    size = 0;
    n = sin;

    while (n != 0) {
        // printf("%d %d\n", n, sin_array[size]);
        temp[size] = n % 10;
        n /= 10;
        size++;
    }

    
    int counter = 0;

    for (int x = 8; x >= 0; --x) {
        sin_array[counter] = temp[x];
        counter++;
        
    }

    return 0;
}

// TODO: Implement check_sin
/*
 * Return 0 if the given sin_array is a valid SIN, and 1 otherwise.
 */
int check_sin(int *sin_array) {
    int auth[9];
    int sum = 0;
    for (int x = 0; x < 9; ++x) {
        if(x % 2 == 0) {
            auth[x] = sin_array[x];
        }

        else {
            int temp = sin_array[x] * 2;
            if (temp >= 10) {
                auth[x] = 1 + (temp % 10);
            }
            else {
                auth[x] = temp;
            }
        }

        sum += auth[x];
    }

    if (sum % 10 == 0) {
        return 0;
    }

    return 1;
}
