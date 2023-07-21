#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>

#define MAXLINE 256

#define SUCCESS "Password verified\n"
#define INVALID "Invalid password\n"
#define NO_USER "No such user\n"

int main(void) {
  char user_id[MAXLINE];
  memset(user_id, '\0', MAXLINE);
  char password[MAXLINE];
  memset(password, '\0', MAXLINE);

  /* The user will type in a user name on one line followed by a password 
     on the next.
     DO NOT add any prompts.  The only output of this program will be one 
	 of the messages defined above.
     Please read the comments in validate carefully
   */

  if(fgets(user_id, MAXLINE, stdin) == NULL) {
      perror("fgets");
      exit(1);
  }
  if(fgets(password, MAXLINE, stdin) == NULL) {
      perror("fgets");
      exit(1);
  }
  
  if (strlen(user_id) - 1 > 10) {
    printf("%s", NO_USER);
    exit(0);
  }
  else if (strlen(password) - 1 > 10) {
    printf("%s", INVALID);
    exit(0);
  }

  int fd[2], r;

  if (pipe(fd) == -1) {
    perror("pipe");
    exit(1);
  }

  r = fork();

  if (r > 0) {
    close(fd[0]);

    write(fd[1], user_id, 10);
    write(fd[1], password, 10);

    close(fd[1]);
    int status;

    if (wait(&status) != -1) {
      if (WEXITSTATUS(status) == 3) {
        printf("%s", NO_USER);
      }
      else if (WEXITSTATUS(status) == 2) {
        printf("%s", INVALID);
      }
      else if (WEXITSTATUS(status) == 1) {
        printf("%s", "Other error\n");
      }
      else {
        printf("%s", SUCCESS);
      }
    }
  }
  else if (r == 0) {
    close(fd[1]);
    
    dup2(fd[0], STDIN_FILENO);
    close(fd[0]);
    execl("./validate", "validate", NULL);

    exit(0);
  }
  else {
    perror("Fork");
    exit(1);
  }

  return 0;
}
