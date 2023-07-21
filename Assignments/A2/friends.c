#include "friends.h"
#include <string.h>
#include <stdio.h>
#include <stdlib.h>

/*
 * Create a new user with the given name.  Insert it at the tail of the list
 * of users whose head is pointed to by *user_ptr_add.
 *
 * Return:
 *   - 0 if successful
 *   - 1 if a user by this name already exists in this list
 *   - 2 if the given name cannot fit in the 'name' array
 *       (don't forget about the null terminator)
 */
int create_user(const char *name, User **user_ptr_add) {

    if (*user_ptr_add == NULL) {

        User *new_user = malloc(sizeof(User));
        memset(new_user, 0, sizeof(User));
        strcpy(new_user->name, name);
        *user_ptr_add = new_user;

        return 0;

    }

    User *current = *user_ptr_add;
    User *num_of_name = find_user(name, current);

    if (strlen(name) + 1 > MAX_NAME) {

        return 2;

    }

    else if (num_of_name != NULL) {

        return 1;

    }

    else {

        User *last = *user_ptr_add;
        while (last->next != NULL) {

            last = last->next;

        }

        User *new_user = malloc(sizeof(User));
        memset(new_user, 0, sizeof(User));
        strcpy(new_user->name, name);
        last->next = new_user;

        return 0;

    }

}


/*
 * Return a pointer to the user with this name in
 * the list starting with head. Return NULL if no such user exists.
 *
 * NOTE: You'll likely need to cast a (const User *) to a (User *)
 * to satisfy the prototype without warnings.
 */
User *find_user(const char *name, const User *head) {

    User *current = (User *) head;
    
    if (current == NULL) {

        return NULL;

    }
    
    while (current != NULL) {

        if (strcmp(current->name, name) == 0) {

            return current;

        }

        current = current->next;

    }

    return NULL;
}


/*
 * Print the usernames of all users in the list starting at curr.
 * Names should be printed to standard output, one per line.
 */
void list_users(const User *curr) {

    User *current = (User *) curr;

    if (current == NULL) {

        printf("No users in the list.\n");
        exit(1);

    }


    while (current != NULL) {

        printf("%s\n", current->name);
        current = current->next;

    }

}


/*
 * Change the filename for the profile pic of the given user.
 *
 * Return:
 *   - 0 on success.
 *   - 1 if the file does not exist or cannot be opened.
 *   - 2 if the filename is too long.
 */
int update_pic(User *user, const char *filename) {
    
    if (strlen(filename) + 1 > MAX_NAME) {

        return 2;

    } 
    
    else if (fopen(filename, "rb") == NULL) {

        return 1;

    }

    else {

        strcpy(user->profile_pic, filename);
        return 0;

    }

}


/*
 * Make two users friends with each other.  This is symmetric - a pointer to
 * each user must be stored in the 'friends' array of the other.
 *
 * New friends must be added in the first empty spot in the 'friends' array.
 *
 * Return:
 *   - 0 on success.
 *   - 1 if the two users are already friends.
 *   - 2 if the users are not already friends, but at least one already has
 *     MAX_FRIENDS friends.
 *   - 3 if the same user is passed in twice.
 *   - 4 if at least one user does not exist.
 *
 * Do not modify either user if the result is a failure.
 * NOTE: If multiple errors apply, return the *largest* error code that applies.
 */
int make_friends(const char *name1, const char *name2, User *head) {
    
    User *current = head;

    if (current == NULL) {

        return 4;

    }

    User *user1 = find_user(name1, head);
    User *user2 = find_user(name2, head);

    if (user1 == NULL || user2 == NULL) {

        return 4;

    }

    else if (strcmp(user1->name, user2->name) == 0) {

        return 3;

    }

    User *friend1[MAX_FRIENDS]; 
    for (int x = 0; x < MAX_FRIENDS; ++x) {

        friend1[x] = user1->friends[x];

    }
    int f1 = 0;
    int f1_num = 0;

    User *friend2[MAX_FRIENDS];
    for (int x = 0; x < MAX_FRIENDS; ++x) {

        friend2[x] = user2->friends[x];

    }
    int f2 = 0;
    int f2_num = 0;

    if (friend1[f1] != NULL) {

        f1++;
        f1_num++;

    }

    while (friend1[f1] != NULL) {

        if (friend1[f1] == user2) {

            return 2;

        }

        f1++;
        f1_num++;
        
    }

    if (friend2[f2] != NULL) {

        f2++;
        f2_num++;

    }

    while (friend2[f2] != NULL) {

        if (friend2[f2] == user1) {

            return 2;

        }

        f2++;
        f2_num++;
        
    }

    if (f1_num == MAX_FRIENDS || f2_num == MAX_FRIENDS) {

        return 3;

    }

    user1->friends[f1] = user2;
    user2->friends[f2] = user1;

    return 0;

}


/*
 * Print a user profile.
 * For an example of the required output format, see the example output
 * linked from the handout.
 * Return:
 *   - 0 on success.
 *   - 1 if the user is NULL.
 */
int print_user(const User *user) {
    
    if (user == NULL) {

        return 1;

    }

    if (strlen(user->profile_pic) != 0) {

        FILE *image;
        image = fopen(user->profile_pic, "r");
        char c_image[8001];
        int count = 0;
        
        while ((count = (fread(c_image, sizeof(char), 8000, image))) > 0) {

            c_image[count] = '\0';
            printf("%s", c_image);

        }

        printf("\n");
        fclose(image);

    }

    printf("Name: %s\n", user->name);
    printf("------------------------------------------\n");

    printf("Friends: \n");
    for (int x = 0; x < MAX_FRIENDS; ++x) {

        if (user->friends[x] != NULL) {

            printf("%s\n", user->friends[x]->name);

        }

    }
    printf("------------------------------------------\n");

    printf("Posts: \n");
    Post *current = user->first_post;
    while (current != NULL) {

        if (current->next != NULL) {

            printf("From: %s\n", current->author);
            printf("Date: %lld\n", (long long) current->date);
            printf("\n");
            printf("%s\n", current->contents);
            printf("\n");
            printf("===\n");
            printf("\n");

            current = current->next;

        }

        else {

            printf("From: %s\n", current->author);
            printf("Date: %s\n", ctime(current->date));
            printf("\n");
            printf("%s\n", current->contents);

            return 0;
        }

    }

    return 0;

}


/*
 * Make a new post from 'author' to the 'target' user,
 * containing the given contents, IF the users are friends.
 *
 * Insert the new post at the *front* of the user's list of posts.
 *
 * 'contents' is a pointer to heap-allocated memory - you do not need
 * to allocate more memory to store the contents of the post.
 *
 * Return:
 *   - 0 on success
 *   - 1 if users exist but are not friends
 *   - 2 if either User pointer is NULL
 */
int make_post(const User *author, User *target, char *contents) {
    
    if (author == NULL || target == NULL) {

        return 2;

    }

    int are_friends = 0;

    for (int x = 0; x < MAX_FRIENDS; ++x) {

        if (author->friends[x] == target) {

            are_friends = 1;

        }

    }

    if (are_friends == 0) {

        return 1;

    }

    Post *new_post = malloc(sizeof(Post));
    memset(new_post, 0, sizeof(Post));
    strcpy(new_post->author, author->name);
    new_post->contents = contents;
    time_t *rawtime = malloc(sizeof(time_t));
    memset(rawtime, 0, sizeof(time_t));
    time(rawtime);
    new_post->date = rawtime;
    new_post->next = target->first_post;
    target->first_post = new_post;

    return 0;

}


/*
 * From the list pointed to by *user_ptr_del, delete the user
 * with the given name.
 * Remove the deleted user from any lists of friends.
 *
 * Return:
 *   - 0 on success.
 *   - 1 if a user with this name does not exist.
 */
int delete_user(const char *name, User **user_ptr_del) {
    
    User *user = find_user(name, *user_ptr_del);

    if (user == NULL) {

        return 1;

    }

    for (int x = 0; x < MAX_FRIENDS; ++x) {

        if (user->friends[x] != NULL) {

            User *current = find_user(user->friends[x]->name, *user_ptr_del);

            for (int y = 0; y < MAX_FRIENDS; ++y) {

                if (current->friends[y] == user && y == 0) {

                    current->friends[y] = current->friends[y + 1];

                }

                else if (current->friends[y] == user) {

                    current->friends[y] = NULL;

                }

            }

        }

    }

    User *tail = NULL;
    User *head = user_ptr_del[0];

    while (head != NULL) {

        if (strcmp(head->name, name) == 0) {

            break;

        }

        tail = head;
        head = head->next;

    }

    if (user_ptr_del[0] == user) {

        user_ptr_del[0] = user->next;
        
    }

    else {

        tail->next = head->next;

    }

    free(user);

    return 0;
}
