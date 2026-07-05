#ifndef PARSER_H
#define PARSER_H

#include "../../include/void.h"

/* Parse input line into command and arguments */
int parse_command(const char *input, char **argv, int max_args);

/* Free parsed arguments */
void free_parsed_args(char **argv, int argc);

/* Expand environment variables in string */
char *expand_env_vars(const char *str);

#endif /* PARSER_H */

