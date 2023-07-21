#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <sys/socket.h>
#include "friends.h"

#ifndef PORT
  #define PORT 58873
#endif
#define BUFSIZE 32
#define BUF_SIZE 800
#define INPUT_ARG_MAX_NUM 12
#define MAX_BACKLOG 5

typedef struct client {
    int sock_fd;
    struct client *next_client;
    User *client_user;
    char buf[BUF_SIZE];
    int num_read;
} Client;

struct sockaddr_in *init_server_addr(int port);
int set_up_server_socket(struct sockaddr_in *self, int num_queue);
int accept_connection(int fd, Client **clients);
void get_name_from_client(int fd, User **user_list, Client **client_list);
int read_from(Client *curr_client, User **user_list, Client **client_list);
int tokenize(char *cmd, char **cmd_argv);
int process_args(int cmd_argc, char **cmd_argv, User **user_list_ptr, char *client_name, int fd);
void create_client(char *username, int fd, User **head, Client **client_list);
int find_network_newline(const char *buf, int n);


int main() {
    User *user_list = NULL;
    Client *client_list = NULL;

    struct sockaddr_in *server = init_server_addr(PORT);
    int sock_fd = set_up_server_socket(server, MAX_BACKLOG);

    int max_fd = sock_fd;
    fd_set all_fds;
    FD_ZERO(&all_fds);
    FD_SET(sock_fd, &all_fds);

    while (1) {
        fd_set listen_fds = all_fds;
        if (select(max_fd + 1, &listen_fds, NULL, NULL, NULL) == -1) {
            perror("server: select");
            exit(1);
        }

        if (FD_ISSET(sock_fd, &listen_fds)) {
            // the only responsibilities of this branch
            //  - accept the connection
            //  - create a Client object and add to the linked list
            //  - tell the user to enter a name (no read calls)
            int client_fd = accept_connection(sock_fd, &client_list);
            if (client_fd > max_fd) {
                max_fd = client_fd;
            }
            FD_SET(client_fd, &all_fds);
            write(client_fd, "What is your user name?\n", strlen("What is your user name?\n"));
            printf("Accepted connection\n");
        }

        Client *curr_client = client_list;
        while (curr_client != NULL) {
            if (curr_client->sock_fd > -1 && FD_ISSET(curr_client->sock_fd, &listen_fds)) {
                int client_closed = read_from(curr_client, &user_list, &client_list);
                if (client_closed != 0) {
                    FD_CLR(client_closed, &all_fds);
                    printf("Client %d disconnected\n", curr_client->sock_fd);
                }
            }
            curr_client = curr_client->next_client;
        }
    }

    return 1;
}


/*
 * Initialize a server address associated with the given port.
 */
struct sockaddr_in *init_server_addr(int port) {
    struct sockaddr_in *addr = malloc(sizeof(struct sockaddr_in));

    // Allow sockets across machines.
    addr->sin_family = AF_INET;

    // The port the process will listen on.
    addr->sin_port = htons(port);

    // Clear this field; sin_zero is used for padding for the struct.
    memset(&(addr->sin_zero), 0, 8);

    // Listen on all network interfaces.
    addr->sin_addr.s_addr = INADDR_ANY;

    return addr;
}


/*
 * Create and set up a socket for a server to listen on.
 */
int set_up_server_socket(struct sockaddr_in *self, int num_queue) {
    int soc = socket(AF_INET, SOCK_STREAM, 0);

    if (soc < 0) {
        perror("socket");
        exit(1);
    }

    // Make sure we can reuse the port immediately after the
    // server terminates. Avoids the "address in use" error
    int on = 1;
    int status = setsockopt(soc, SOL_SOCKET, SO_REUSEADDR, (const char *) &on, sizeof(on));

    if (status < 0) {
        perror("setsockopt");
        exit(1);
    }

    // Associate the process with the address and a port
    if (bind(soc, (struct sockaddr *)self, sizeof(*self)) < 0) {
        // bind failed; could be because port is in use.
        perror("bind");
        close(soc);
        exit(1);
    }

    // Set up a queue in the kernel to hold pending connections.
    if (listen(soc, num_queue) < 0) {
        // listen failed
        perror("listen");
        close(soc);
        exit(1);
    }

    return soc;
}


/*
 * Accept a connection. Note that a new file descriptor is created for
 * communication with the client. The initial socket descriptor is used
 * to accept connections, but the new socket is used to communicate.
 * Return the new client's file descriptor or -1 on error.
 */
int accept_connection(int fd, Client **clients) {
    Client *curr_client = *clients;


    int client_fd = accept(fd, NULL, NULL);
    if (client_fd < 0) {
        perror("server: accept");
        close(fd);
        exit(1);
    }

    Client *new_client = malloc(sizeof(Client));
    memset(new_client, 0, sizeof(Client));
    new_client->sock_fd = client_fd;
    if (curr_client == NULL) {
        *clients = new_client;
    }
    else {
        while (curr_client->next_client != NULL) {
            curr_client = curr_client->next_client;
        }
        curr_client->next_client = new_client;
    }

    return client_fd;
}


int read_from(Client *curr_client, User **user_list, Client **client_list) {
    int fd = curr_client->sock_fd;
    int nbytes = read(fd, curr_client->buf + curr_client->num_read, BUF_SIZE - curr_client->num_read);

    if (nbytes > 0) {
        curr_client->num_read += nbytes;
        int where;
        while ((where = find_network_newline(curr_client->buf, curr_client->num_read)) > 0) {
            curr_client->buf[where - 2] = '\0';
            char *cmd_argv[INPUT_ARG_MAX_NUM];
            int cmd_argc = tokenize(curr_client->buf, cmd_argv);
            if (curr_client->client_user == NULL) {
//                int c_user = create_user(curr_client->buf, user_list);
//                curr_client->client_user = find_user(curr_client->buf, *user_list);

                if (strlen(curr_client->buf) > BUFSIZE) {
                    char *temp = malloc(sizeof(char) * (BUF_SIZE + 1));
                    strncpy(temp, curr_client->buf, BUFSIZE);
                    temp[BUFSIZE - 1] = '\0';
                    strcpy(curr_client->buf, temp);
                    curr_client->client_user = find_user(temp, *user_list);
                    write(fd, "Username too long, truncated to 31 chars.\n", strlen("Username too long, truncated to 31 chars.\n"));
                }
                else {
                    curr_client->client_user = find_user(curr_client->buf, *user_list);
                }

                if (curr_client->client_user == NULL) {
                    create_user(curr_client->buf, user_list);
                    curr_client->client_user = find_user(curr_client->buf, *user_list);
                    write(fd, "Welcome.\n", strlen("Welcome.\n"));
                }
                else {
                    write(fd, "Welcome back.\n", strlen("Welcome back.\n"));
                }
                write(fd, "Go ahead and enter user commands>\n", strlen("Go ahead and enter user commands>\n"));
            }
            else {
                int pr_args = process_args(cmd_argc, cmd_argv, user_list, curr_client->client_user->name, fd);
                if (pr_args == -1) {
                    return -1;
                }
            }
            memmove(curr_client->buf, curr_client->buf + where, curr_client->num_read - where);
            curr_client->num_read = curr_client->num_read - where;
        }
    }
    return 0;
}


int tokenize(char *cmd, char **cmd_argv) {
    int cmd_argc = 0;
    char *next_token = strtok(cmd, " ");    
    while (next_token != NULL) {
        if (cmd_argc >= INPUT_ARG_MAX_NUM - 1) {
            perror("Too many arguments!");
            cmd_argc = 0;
            break;
        }
        cmd_argv[cmd_argc] = next_token;
        cmd_argc++;
        next_token = strtok(NULL, " ");
    }

    return cmd_argc;
}


int process_args(int cmd_argc, char **cmd_argv, User **user_list_ptr, char *client_name, int fd) {
    User *user_list = *user_list_ptr;
    User *head = *user_list_ptr;
    User *client = find_user(client_name, head);

    if (cmd_argc <= 0) {
        return 0;
    }
    else if (strcmp(cmd_argv[0], "quit") == 0 && cmd_argc == 1) {
        return -1;
    } 
    else if (strcmp(cmd_argv[0], "list_users") == 0 && cmd_argc == 1) {
        char *buf = list_users(head);
        write(fd, buf, strlen(buf));
        free(buf);
        return 0;
    } 
    else if (strcmp(cmd_argv[0], "make_friends") == 0 && cmd_argc == 2) {
        switch (make_friends(client->name, cmd_argv[1], user_list)) {
            case 1:
                perror("users are already friends");
                break;
            case 2:
                perror("at least one user you entered has the max number of friends");
                break;
            case 3:
                perror("you must enter two different users");
                break;
            case 4:
                perror("at least one user you entered does not exist");
                break;
        }
    } 
    else if (strcmp(cmd_argv[0], "post") == 0 && cmd_argc >= 3) {
        // first determine how long a string we need
        int space_needed = 0;
        for (int i = 2; i < cmd_argc; i++) {
            space_needed += strlen(cmd_argv[i]) + 1;
        }

        // allocate the space
        char *contents = malloc(space_needed);
        if (contents == NULL) {
            perror("malloc");
            exit(1);
        }

        // copy in the bits to make a single string
        strcpy(contents, cmd_argv[2]);
        for (int i = 3; i < cmd_argc; i++) {
            strcat(contents, " ");
            strcat(contents, cmd_argv[i]);
        }

        User *author = client;
        User *target = find_user(cmd_argv[1], user_list);
        switch (make_post(author, target, contents)) {
            case 1:
                perror("the users are not friends");
                break;
            case 2:
                perror("at least one user you entered does not exist");
                break;
        }
    } 
    else if (strcmp(cmd_argv[0], "profile") == 0 && cmd_argc == 2) {
        User *user = find_user(cmd_argv[1], head);
        char *buf = print_user(user);
        if (buf == NULL) {
            perror("user not found");
        }
        else {
            write(fd, buf, strlen(buf));
        }
        free(buf);
    } 
    else {
        perror("Incorrect syntax");
    }
    return 0;
}


int find_network_newline(const char *buf, int n) {
    int index = 0;
    while (index < n - 1 && buf[index] != '\r' && buf[index + 1] != '\n') {
        index++;
    }
    if (buf[index] == '\r' && buf[index + 1] == '\n') {
        return index + 2;
    }

    return -1;
}
