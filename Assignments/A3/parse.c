#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "pmake.h"

/*
Helper method that takes a string t, representing the target
of a rule that needs to be made.
*/
Rule *make_rule(char* t) {
    Rule *new_rule = malloc(sizeof(Rule));
    new_rule->target = malloc(sizeof(char) * (strlen(t) + 1));
    strcpy(new_rule->target, t);
    new_rule->dependencies = NULL;
    new_rule->actions = NULL;
    new_rule->next_rule = NULL;
    return new_rule;
}

/*
Helper method that takes a Rule pointer which points to the head of
the linked list of rules, and a string which represents the name of
the rule which the dependency should be linked to.
*/
Dependency *make_dependency(Rule *head, char* d_name) {
    Dependency *new_dependency = malloc(sizeof(Dependency));
    new_dependency->next_dep = NULL;
    Rule *curr_rule = head;

    while (curr_rule != NULL && strcmp(curr_rule->target, d_name) != 0) {
        curr_rule = curr_rule->next_rule;
    }

    if (curr_rule != NULL) {
        new_dependency->rule = curr_rule;
    }
    else {
        Rule *new_rule = make_rule(d_name);
        new_dependency->rule = new_rule;
    }
    return new_dependency;
}

/*
Helper method that takes a list of strings containing all of the 
words used in the action of a rule.
*/
Action *make_action(char** a) {
    Action *new_action = malloc(sizeof(Action));
    new_action->args = a;
    new_action->next_act = NULL;
    return new_action;
}

/* Read from the open file fp, and create the linked data structure
   that represents the Makefile contained in the file.
   See the top of pmake.h for the specification of Makefile contents.
 */
Rule *parse_file(FILE *fp) {
    char line[MAXLINE];
    Rule *current = NULL;
    Rule *head = NULL;
    Rule *curr_for_d = NULL;

    // This while loop will take all of the target names at the
    // beginning of the line and use the make_rule helper method
    // to create a rule with the input target name. It will then set
    // the outside Rule pointer, current, to the newly created rule.
    // If the line starts with a tab character, then we use the
    // make_action helper method to create a new action and attach
    // it to the current rule.
    while(fgets(line, sizeof(line), fp) != NULL) {

        // The following for loop replaces all of the 
        // '\r' and '\n' characters with the null terminator.
        for(int i = 0; i < MAXLINE; ++i) {
            if (line[i] == '\r' || line[i] == '\n') {
                line[i] = '\0';
                break;
            }
            else if (line[i] == '\0') {
                break;
            }
        }
        if (is_comment_or_empty(line) != 1) {

            // Checks to see whether the first character in the
            // current line is a tab. If it is a tab, we know that
            // the current line contains an action. If the line
            // does not start with a tab, that means it starts with
            // a target.
            if (line[0] == '\t') {
                char **args = malloc(sizeof(char *));
                char *curr = strtok(line + 1, " ");
                int index = 0;

                // Continuously adds the next word before a space in the current
                // line until we reach the end of the line. This list is our array
                // of arguments for an action.
                while (curr != NULL) {
                    args = realloc(args, sizeof(char *) * (index + 1));
                    args[index] = malloc(sizeof(char) * (strlen(curr) + 1));
                    strcpy(args[index], curr);
                    index++;
                    curr = strtok(NULL, " ");
                }
                args = realloc(args, sizeof(char *) * (index + 1));
                args[index] = NULL;

                if (current->actions == NULL) {
                    current->actions = make_action(args);
                }
                else {
                    Action *curr_act = current->actions;

                    while (curr_act->next_act != NULL) {
                        curr_act = curr_act->next_act;
                    }
                    curr_act->next_act = make_action(args);
                }
            }

            // If the first character is not a tab, then we know the current
            // line starts with a target string.
            else {
                char *target = strtok(line, ":");
                target = strtok(target, " ");

                if (current == NULL) {
                    current = make_rule(target);
                    head = current;
                    curr_for_d = current;
                }
                else {
                    Rule *curr_rule = current;

                    while (curr_rule->next_rule != NULL) {
                        curr_rule = curr_rule->next_rule;
                    }
                    curr_rule->next_rule = make_rule(target);
                    current = curr_rule->next_rule;
                }
            }
        }
    }

    // We reset the location on the file so we
    // can parse through it again, this time
    // creating all of the dependencies.
    fseek(fp, 0, SEEK_SET);

    // In this while loop, we will read through each
    // line that doesn't start with a tab character
    // and take the strings representing the
    // dependency names. With these names and the 
    // head Rule pointer, we can use the helper method
    // make_dependency to create all of the dependencies
    // related to the current rule. We then create rules
    // using the make_rule helper method using the names
    // of all the dependencies, and finally link them to
    // the dependency nodes.
    while (fgets(line, sizeof(line), fp) != NULL) {
        for(int i = 0; i < MAXLINE; ++i) {
            if (line[i] == '\r' || line[i] == '\n') {
                line[i] = '\0';
                break;
            }
            else if (line[i] == '\0') {
                break;
            }
        }
        if (is_comment_or_empty(line) != 1) {
            if (line[0] != '\t') {
                strtok(line, " ");
                strtok(NULL, " ");
                char *curr_d = strtok(NULL, " ");

                while (curr_d != NULL && curr_for_d != NULL) {
                    Rule *dependency_rule = NULL;

                    if (curr_for_d->dependencies == NULL) {
                        curr_for_d->dependencies = make_dependency(head, curr_d);
                        dependency_rule = make_rule(curr_d);
                    }
                    else {
                        Dependency *cd = curr_for_d->dependencies;

                        while (cd->next_dep != NULL) {
                            cd = cd->next_dep;
                        }
                        cd->next_dep = make_dependency(head, curr_d);
                        dependency_rule = make_rule(curr_d);
                    }
                    Rule *final_rule_checker = head;

                    while (final_rule_checker != NULL && final_rule_checker->next_rule != NULL) {
                        final_rule_checker = final_rule_checker->next_rule;
                    }
                    final_rule_checker->next_rule = dependency_rule;
                    curr_d = strtok(NULL, " ");
                }
                curr_for_d = curr_for_d->next_rule;
            }
        }
    }

    return head;
}

/******************************************************************************
 * These helper functions are provided for you. Do not modify them.
 *****************************************************************************/
/* Print the list of actions */
void print_actions(Action *act) {
    while(act != NULL) {
        if(act->args == NULL) {
            fprintf(stderr, "ERROR: action with NULL args\n");
            act = act->next_act;
            continue;
        }
        printf("\t");

        int i = 0;
        while(act->args[i] != NULL) {
            printf("%s ", act->args[i]) ;
            i++;
        }
        printf("\n");
        act = act->next_act;
    }
}

/* Print the list of rules to stdout in makefile format. If the output
   of print_rules is saved to a file, it should be possible to use it to
   run make correctly.
 */
void print_rules(Rule *rules){
    Rule *cur = rules;

    while (cur != NULL) {
        if (cur->dependencies || cur->actions) {
            // Print target
            printf("%s : ", cur->target);

            // Print dependencies
            Dependency *dep = cur->dependencies;
            while (dep != NULL){
                if(dep->rule->target == NULL) {
                    fprintf(stderr, "ERROR: dependency with NULL rule\n");
                }
                printf("%s ", dep->rule->target);
                dep = dep->next_dep;
            }
            printf("\n");

            // Print actions
            print_actions(cur->actions);
        }
        cur = cur->next_rule;
    }
}


/* Return 1 if the line is a comment line, as defined on the assignment handout.
   Return 0 otherwise.
 */
int is_comment_or_empty(const char *line) {
    for (int i = 0; i < strlen(line); i++){
        if (line[i] == '#') {
            return 1;
        }
        if (line[i] != '\t' && line[i] != ' ') {
            return 0;
        }
    }
    return 1;
}

/* Convert an array of args to a single space-separated string in buffer.
   Returns buffer.  Note that memory for args and buffer should be allocted
   by the caller.
 */
char *args_to_string(char **args, char *buffer, int size) {
    buffer[0] = '\0';
    int i = 0;
    while (args[i] != NULL) {
        strncat(buffer, args[i], size - strlen(buffer));
        strncat(buffer, " ", size - strlen(buffer));
        i++;
    }
    return buffer;
}
