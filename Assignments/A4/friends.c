#include "friends.h"
#include <string.h>
#include <stdio.h>
#include <stdlib.h>


/*
 * Create a new user with the given name.  Insert it at the tail of the list
 * of users whose head is pointed to by *user_ptr_add.
 *
 * Return:
 *   - 0 on success.
 *   - 1 if a user by this name already exists in this list.
 *   - 2 if the given name cannot fit in the 'name' array
 *       (don't forget about the null terminator).
 */
int create_user(const char *name, User **user_ptr_add) {
    if (strlen(name) >= MAX_NAME) {
        return 2;
    }

    User *new_user = malloc(sizeof(User));
    if (new_user == NULL) {
        perror("malloc");
        exit(1);
    }
    strncpy(new_user->name, name, MAX_NAME); // name has max length MAX_NAME - 1

    for (int i = 0; i < MAX_NAME; i++) {
        new_user->profile_pic[i] = '\0';
    }

    new_user->first_post = NULL;
    new_user->next = NULL;
    for (int i = 0; i < MAX_FRIENDS; i++) {
        new_user->friends[i] = NULL;
    }

    // Add user to list
    User *prev = NULL;
    User *curr = *user_ptr_add;
    while (curr != NULL && strcmp(curr->name, name) != 0) {
        prev = curr;
        curr = curr->next;
    }

    if (*user_ptr_add == NULL) {
        *user_ptr_add = new_user;
        return 0;
    } else if (curr != NULL) {
        free(new_user);
        return 1;
    } else {
        prev->next = new_user;
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
    while (head != NULL && strcmp(name, head->name) != 0) {
        head = head->next;
    }
    
    return (User *)head;
}


/*
 * Add the names of all of the users in the list starting at curr
 * to a single string with the correct format.
 * Names should be followed by '\n'.
 */
char *list_users(const User *curr) {
    int string_size = 0;
    const User *size_counter = curr;
    while (size_counter != NULL) {
        string_size += strlen(size_counter->name) + strlen("\n");
        size_counter = size_counter->next;
    }
    char *list_of_users = malloc(sizeof(char) * (string_size + 1));
    list_of_users[0] = '\0';

    while (curr != NULL) {
        strcat(list_of_users, curr->name);
        if (curr->next == NULL) {
            strcat(list_of_users, "\n\0");
            break;
        }
        else {
            strcat(list_of_users, "\n");
        }
        curr = curr->next;
    }

    return list_of_users;
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
    User *user1 = find_user(name1, head);
    User *user2 = find_user(name2, head);

    if (user1 == NULL || user2 == NULL) {
        return 4;
    } else if (user1 == user2) { // Same user
        return 3;
    }

    int i, j;
    for (i = 0; i < MAX_FRIENDS; i++) {
        if (user1->friends[i] == NULL) { // Empty spot
            break;
        } else if (user1->friends[i] == user2) { // Already friends.
            return 1;
        }
    }

    for (j = 0; j < MAX_FRIENDS; j++) {
        if (user2->friends[j] == NULL) { // Empty spot
            break;
        }
    }

    if (i == MAX_FRIENDS || j == MAX_FRIENDS) { // Too many friends.
        return 2;
    }

    user1->friends[i] = user2;
    user2->friends[j] = user1;
    return 0;
}


/*
 *  Return a post as a singular string.
 *  Use localtime to print the time and date.
 */
char *print_post(const Post *post) {
    if (post == NULL) {
        return NULL;
    }

    int post_size = strlen("From: ") + strlen(post->author) + strlen("\n");
    post_size += strlen("Date: ") + strlen(asctime(localtime(post->date))) + strlen("\n");
    post_size += strlen(post->contents) + strlen("\n");
    char *post_string = malloc(sizeof(char) * (post_size + 1));

    strcpy(post_string, "From: ");
    strcat(post_string, post->author);
    strcat(post_string, "\n");

    strcat(post_string, "Date: ");
    strcat(post_string, asctime(localtime(post->date)));
    strcat(post_string, "\n");

    strcat(post_string, post->contents);
    strcat(post_string, "\n");

    return post_string;
    
}


/*
 * Return a post as a singular string.
 */
char *print_user(const User *user) {
    if (user == NULL) {
        return NULL;
    }
    int string_size = strlen("Name: ") + strlen(user->name) + strlen("\n\n");
    string_size += strlen("------------------------------------------\n");
    
    string_size += strlen("Friends:\n");
    for (int i = 0; i < MAX_FRIENDS && user->friends[i] != NULL; i++) {
        string_size += strlen(user->friends[i]->name) + strlen("\n");
    }
    string_size += strlen("------------------------------------------\n");
    
    string_size += strlen("Posts:\n");
    Post *curr_size = user->first_post;
    while (curr_size != NULL) {
        char *temp = print_post(curr_size);
        string_size += strlen(temp);
        curr_size = curr_size->next;
        if (curr_size != NULL) {
            string_size += strlen("\n===\n\n");
        }
    }
    string_size += strlen("------------------------------------------\n");
    char *user_string = malloc(sizeof(char) * (string_size + 1));

    strcpy(user_string, "Name: ");
    strcat(user_string, user->name);
    strcat(user_string, "\n\n");
    strcat(user_string, "------------------------------------------\n");

    strcat(user_string, "Friends:\n");
    for (int i = 0; i < MAX_FRIENDS && user->friends[i] != NULL; i++) {
        strcat(user_string, user->friends[i]->name);
        strcat(user_string, "\n");
    }
    strcat(user_string, "------------------------------------------\n");

    strcat(user_string, "Posts:\n");
    Post *curr_post = user->first_post;
    while (curr_post != NULL) {
        char *temp = print_post(curr_post);
        strcat(user_string, temp);
        curr_post = curr_post->next;
        if (curr_post != NULL) {
            strcat(user_string, "\n===\n\n");
        }
        free(temp);
    }
    strcat(user_string, "------------------------------------------\n");

    return user_string;
}


/*
 * Make a new post from 'author' to the 'target' user,
 * containing the given contents, IF the users are friends.
 *
 * Insert the new post at the *front* of the user's list of posts.
 *
 * Use the 'time' function to store the current time.
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
    if (target == NULL || author == NULL) {
        return 2;
    }

    int friends = 0;
    for (int i = 0; i < MAX_FRIENDS && target->friends[i] != NULL; i++) {
        if (strcmp(target->friends[i]->name, author->name) == 0) {
            friends = 1;
            break;
        }
    }

    if (friends == 0) {
        return 1;
    }

    // Create post
    Post *new_post = malloc(sizeof(Post));
    if (new_post == NULL) {
        perror("malloc");
        exit(1);
    }
    strncpy(new_post->author, author->name, MAX_NAME);
    new_post->contents = contents;
    new_post->date = malloc(sizeof(time_t));
    if (new_post->date == NULL) {
        perror("malloc");
        exit(1);
    }
    time(new_post->date);
    new_post->next = target->first_post;
    target->first_post = new_post;

    return 0;
}

