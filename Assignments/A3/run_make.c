#include <stdio.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/stat.h>

#include "pmake.h"

/*
Helper method that takes a pointer to an action,
prints the action to stdout, executes the action,
and then moves onto the next action if it is not NULL.
*/
void print_and_execute_actions(Action *curr_act) {

  while (curr_act != NULL) {
    char *action_string = malloc(sizeof(char) * MAXLINE);
    action_string = args_to_string(curr_act->args, action_string, MAXLINE);
    printf("%s\n", action_string);
    free(action_string);

    int pid = fork();

    if (pid == 0) {
      int err = execvp(curr_act->args[0], curr_act->args);

      if (err == -1) {
        printf("Could not execute action with the given arguments.\n");
        exit(err);
      }
    }
    else {
      int wait_var = 0;
      wait(&wait_var);
      int exit_status = WEXITSTATUS(wait_var);

      if (exit_status != 0) {
        exit(exit_status);
      }
  }
    curr_act = curr_act->next_act;
  }
}

/*
Helper method that takes the head of the
linked list of rules and a string representing
the target of the rule we are looking for. If 
the rule exists, it is returned by the method.
Otherwise, it returns null.
*/
Rule *get_rule(Rule *head, char *target) {
  Rule *curr = head;

  while (curr != NULL && strcmp(curr->target, target) != 0) {
      curr = curr->next_rule;
  }
  return curr;
}

/* Evaluate the rule in rules corresponding to the given target.
If target is NULL, evaluate the first rule instead.
If pflag is 0, evaluate each dependency in sequence.
If pflag is 1, then evaluate each dependency in parallel (by creating one
new process per dependency). In this case, the parent process will wait until
all child processes have terminated before checking dependency modified times
to decide whether to execute the actions.
*/
void run_make(char *target, Rule *rules, int pflag) {
  Rule *evaluate = NULL;
  struct stat eval_buf;
  memset(&eval_buf, 0, sizeof(eval_buf));
  Dependency *curr_dep = NULL;
  Action *curr_act = NULL;

  // Checks to see if the input target is NULL.
  if (target == NULL) {
      evaluate = rules;
  }

  // If it is not null, then we will use the get_rule
  // helper method to check if a rule with the input
  // target exists, and if it does, we will set evaluate
  // equal to that. Otherwise, evaluate will be null.
  else {
    evaluate = get_rule(rules, target);
  }

  if (evaluate == NULL) {
    printf("This target does not exist within the inserted list of rules. Sorry!\n");
    exit(1);
  }
  curr_dep = evaluate->dependencies;
  curr_act = evaluate->actions;
  int stat_err = stat(evaluate->target, &eval_buf); 

  // When pflag is 0, we execute and print the action
  // statements in sequence.
  if (pflag == 0) {

    // We have our recursive call to run_make here.
    // It executes run_make if the current dependency
    // is not null. Then, it calls run_make on the target
    // of the current dependency, and finally, the current
    // dependency is shifted to the next one.
    while (curr_dep != NULL) {
      run_make(curr_dep->rule->target, rules, pflag);
      curr_dep = curr_dep->next_dep;
    }
    curr_dep = evaluate->dependencies;
  }

  // When pflag is 1, we evaluate each dependency in parallel
  // using the fork() command and a recursive call to run_make.
  else if (pflag == 1) { // Call fork before the recursive call.
    int pid = 0;

    // After checking if the current dependency exists, the fork
    // method is called, and this causes all of the dependencies
    // we want to check to run in parallel. The parent process
    // waits on the children processes to finish before exiting
    // with one of its childrens' exit status.
    while (curr_dep != NULL) {
      pid = fork();
      if (pid == 0) {
        run_make(curr_dep->rule->target, rules, pflag);
        exit(0);
      }
      else {
        curr_dep = curr_dep->next_dep;
      }
    }

    // Resets the value of the current dependency
    // to be what it was before the previous loop.
    curr_dep = evaluate->dependencies;

    // This keeps track of the children exit statuses
    // and returns one of them when all of the children
    // processes finish.
    while (curr_dep != NULL) {
      int exit_status = 0;
      wait(&exit_status);
      exit_status = WEXITSTATUS(exit_status);
      curr_dep = curr_dep->next_dep;

      if (exit_status != 0) {
          exit(exit_status);
      }
    }
  }

  // Check if stat fails in the case that target is not a file.
  if (stat_err != 0) {
    print_and_execute_actions(curr_act);
    return;
  }

  // The following while loop is used after checking the value
  // of the pflag because we can use the same code for both possible
  // values of the pflag. This while loop checks to see whether the
  // current dependency was modified more recently than the parent process,
  // and if it does, then we execute our print_and_execute_actions helper
  // method, which then prints and executes all of the action strings
  // we would need in order to execute our parent's action string.
  while (curr_dep != NULL) {
    struct stat buf;
    Rule *dep_rule = get_rule(rules, curr_dep->rule->target);
    char *dep_file_name = dep_rule->target;
    Dependency *dep_dependencies = dep_rule->dependencies;
    memset(&buf, 0, sizeof(buf));
    int curr_stat_err = stat(dep_file_name, &buf);

    if (curr_stat_err != 0) {
      print_and_execute_actions(curr_act);
      return;
    }
    else if (dep_dependencies == NULL) {
      long e_sec = (&eval_buf)->st_mtim.tv_sec;
      long e_nsec = (&eval_buf)->st_mtim.tv_nsec;
      long b_sec = (&buf)->st_mtim.tv_sec;
      long b_nsec = (&buf)->st_mtim.tv_nsec;

      if ((e_sec == b_sec && e_nsec < b_nsec) || e_sec < b_sec) {
        print_and_execute_actions(curr_act);
        return;
      }
    }
    curr_dep = curr_dep->next_dep;
  }
}
