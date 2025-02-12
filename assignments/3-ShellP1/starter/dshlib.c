#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>

#include "dshlib.h"

/*
 *  build_cmd_list
 *    cmd_line:     the command line from the user
 *    clist *:      pointer to clist structure to be populated
 *
 *  This function builds the command_list_t structure passed by the caller
 *  It does this by first splitting the cmd_line into commands by spltting
 *  the string based on any pipe characters '|'.  It then traverses each
 *  command.  For each command (a substring of cmd_line), it then parses
 *  that command by taking the first token as the executable name, and
 *  then the remaining tokens as the arguments.
 *
 *  NOTE your implementation should be able to handle properly removing
 *  leading and trailing spaces!
 *
 *  errors returned:
 *
 *    OK:                      No Error
 *    ERR_TOO_MANY_COMMANDS:   There is a limit of CMD_MAX (see dshlib.h)
 *                             commands.
 *    ERR_CMD_OR_ARGS_TOO_BIG: One of the commands provided by the user
 *                             was larger than allowed, either the
 *                             executable name, or the arg string.
 *
 *  Standard Library Functions You Might Want To Consider Using
 *      memset(), strcmp(), strcpy(), strtok(), strlen(), strchr()
 */

static char* trim_whitespace(char *str)
{
    char *end;
    while (isspace((unsigned char)*str))
        str++;
    if (*str == 0) 
        return str;
    end = str + strlen(str) - 1;
    while (end > str && isspace((unsigned char)*end))
        end--;
    end[1] = '\0';
    return str;
}
 int build_cmd_list(char *cmd_line, command_list_t *clist)
{
    clist->num = 0;

    char *line = trim_whitespace(cmd_line);
    if (strlen(line) == 0)
    {
        return WARN_NO_CMDS;
    }

    int cmd_count = 0;
    char *saveptr;
    char *token = strtok_r(line, PIPE_STRING, &saveptr);
    while (token != NULL)
    {
        char *command_str = trim_whitespace(token);
        if (strlen(command_str) == 0)
        {
            token = strtok_r(NULL, PIPE_STRING, &saveptr);
            continue;
        }

        if (cmd_count >= CMD_MAX)
        {
            return ERR_TOO_MANY_COMMANDS;
        }

        char *cmd_copy = strdup(command_str);
        if (!cmd_copy)
        {
            perror("strdup");
            exit(EXIT_FAILURE);
        }

        char *arg_saveptr;
        char *exe = strtok_r(cmd_copy, " ", &arg_saveptr);
        if (exe == NULL)
        {
            free(cmd_copy);
            token = strtok_r(NULL, PIPE_STRING, &saveptr);
            continue;
        }
        if (strlen(exe) >= EXE_MAX)
        {
            free(cmd_copy);
            return ERR_CMD_OR_ARGS_TOO_BIG;
        }
        strcpy(clist->commands[cmd_count].exe, exe);

        clist->commands[cmd_count].args[0] = '\0';
        char *arg;
        int first = 1;
        while ((arg = strtok_r(NULL, " ", &arg_saveptr)) != NULL)
        {
            if (!first)
            {
                if (strlen(clist->commands[cmd_count].args) + 1 >= ARG_MAX)
                {
                    free(cmd_copy);
                    return ERR_CMD_OR_ARGS_TOO_BIG;
                }
                strcat(clist->commands[cmd_count].args, " ");
            }
            if (strlen(clist->commands[cmd_count].args) + strlen(arg) >= ARG_MAX)
            {
                free(cmd_copy);
                return ERR_CMD_OR_ARGS_TOO_BIG;
            }
            strcat(clist->commands[cmd_count].args, arg);
            first = 0;
        }
        free(cmd_copy);
        cmd_count++;
        token = strtok_r(NULL, PIPE_STRING, &saveptr);
    }

    if (cmd_count == 0)
    {
        return WARN_NO_CMDS;
    }
    clist->num = cmd_count;
    return OK;
}
