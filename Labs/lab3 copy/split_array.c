#include <stdio.h>
#include <stdlib.h>

/* Return a pointer to an array of two dynamically allocated arrays of ints.
   The first array contains the elements of the input array s that are
   at even indices.  The second array contains the elements of the input
   array s that are at odd indices.

   Do not allocate any more memory than necessary. You are not permitted
   to include math.h.  You can do the math with modulo arithmetic and integer
   division.
*/
int **split_array(const int *s, int length) {

  int **ptr;
  size_t even = 0;
  int evenCounter = 0;
  size_t odd = 1;
  int oddCounter = 0;

  ptr = malloc(sizeof(int *) * 2);

  if (ptr) {

    if (length % 2 == 0) {

      int arrayLength = length/2;

      ptr[even] = malloc(sizeof(*ptr[even]) * arrayLength);
      ptr[odd] = malloc(sizeof(*ptr[odd]) * arrayLength);

    }
    else {

      int evenArrayLength = (length/2) + 1;
      int oddArrayLength = length/2;

      ptr[even] = malloc(sizeof(*ptr[even]) * evenArrayLength);
      ptr[odd] = malloc(sizeof(*ptr[odd]) * oddArrayLength);

    }

    for (int x = 0; x < length; ++x) {

      if (x % 2 == 0) {

        ptr[even][evenCounter] = s[x];
        evenCounter++;

      }
      else {

        ptr[odd][oddCounter] = s[x];
        oddCounter++;

      }

    }

  }

  return ptr;

}

/* Return a pointer to an array of ints with size elements.
   - strs is an array of strings where each element is the string
     representation of an integer.
   - size is the size of the array
 */

int *build_array(char **strs, int size) {

  int *ptr = malloc(sizeof(int) * size);

  for (int x = 0; x < size; ++x) {

    int current = atoi(strs[x + 1]);
    ptr[x] = current;

  }

  return ptr;

}


int main(int argc, char **argv) {
    /* Replace the comments in the next two lines with the appropriate
       arguments.  Do not add any additional lines of code to the main
       function or make other changes.
     */
    int *full_array = build_array(argv, argc - 1);
    int **result = split_array(full_array, argc - 1);

    printf("Original array:\n");
    for (int i = 0; i < argc - 1; i++) {
        printf("%d ", full_array[i]);
    }
    printf("\n");

    printf("result[0]:\n");
    for (int i = 0; i < argc / 2; i++) {
        printf("%d ", result[0][i]);
    }
    printf("\n");

    printf("result[1]:\n");
    for (int i = 0; i < (argc - 1) / 2; i++) {
        printf("%d ", result[1][i]);
    }
    printf("\n");
    free(full_array);
    free(result[0]);
    free(result[1]);
    free(result);
    return 0;
}
