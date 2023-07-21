#include <stdio.h>
#include <string.h>


/*
    This program has two arguments: the first is a greeting message, and the
    second is a name.

    The message is an impersonal greeting, such as "Hi" or "Good morning".
    name is set to refer to a string holding a friend's name, such as
    "Emmanuel" or "Xiao".

    First copy the first argument to the array greeting. (Make sure it is
    properly null-terminated.)

    Write code to personalize the greeting string by appending a space and
    then the string pointed to by name.
    So, in the first example, greeting should be set to "Hi Emmanuel", and
    in the second it should be "Good morning Xiao".

    If there is not enough space in greeting, the resulting greeting should be
    truncated, but still needs to hold a proper string with a null terminator.
    For example, "Good morning" and "Emmanuel" should result in greeting
    having the value "Good morning Emmanu" and "Top of the morning to you" and
    "Patrick" should result in greeting having the value "Top of the morning ".

    Do not make changes to the code we have provided other than to add your
    code where indicated.
*/

int main(int argc, char **argv) {
    if (argc != 3) {
        fprintf(stderr, "Usage: greeting message name\n");
        return 1;
    }
    char greeting[20];
    char *name = argv[2];

    // Your code goes here

    int greeting_size = strlen(argv[1]);
    int name_size = strlen(argv[2]);

    if (greeting_size <= 20) {
        strcpy(greeting, argv[1]);

        greeting[greeting_size] = '\0';

        char append[1 + name_size];
        append[0] = ' ';
        append[1] = '\0';
        strcat(append, name);
        append[1 + name_size] = '\0';

        int total_size = strlen(argv[1]) + strlen(append);

        if (total_size <= 20) {
            strcat(greeting, append);
        }
        else {
            char temp[total_size];
            strcpy(temp, greeting);
            strcat(temp, append);

            for (int x = 0; x < 19; ++x) {
                greeting[x] = temp[x];
            }
            greeting[19] = '\0';
        }
    }
    else {
        char temp[greeting_size];
        strcpy(temp, argv[1]);
        for (int x = 0; x < 19; ++x) {
            greeting[x] = temp[x];
        }
        greeting[19] = '\0';
    }

    printf("%s\n", greeting);
    return 0;
}


