#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <stdbool.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/wait.h>

#include "dshlib.h"

/*
 * Implement your exec_local_cmd_loop function by building a loop that prompts the 
 * user for input.  Use the SH_PROMPT constant from dshlib.h and then
 * use fgets to accept user input.
 * 
 *      while(1){
 *        printf("%s", SH_PROMPT);
 *        if (fgets(cmd_buff, ARG_MAX, stdin) == NULL){
 *           printf("\n");
 *           break;
 *        }
 *        //remove the trailing \n from cmd_buff
 *        cmd_buff[strcspn(cmd_buff,"\n")] = '\0';
 * 
 *        //IMPLEMENT THE REST OF THE REQUIREMENTS
 *      }
 * 
 *   Also, use the constants in the dshlib.h in this code.  
 *      SH_CMD_MAX              maximum buffer size for user input
 *      EXIT_CMD                constant that terminates the dsh program
 *      SH_PROMPT               the shell prompt
 *      OK                      the command was parsed properly
 *      WARN_NO_CMDS            No commands parsed
 *      ERR_TOO_MANY_COMMANDS   too many pipes used
 *      ERR_MEMORY              dynamic memory management failure
 * 
 *   errors returned
 *      OK                     No error
 *      ERR_MEMORY             Dynamic memory management failure
 *      WARN_NO_CMDS           No commands parsed
 *      ERR_TOO_MANY_COMMANDS  too many pipes used
 *   
 *   console messages
 *      CMD_WARN_NO_CMD        print on WARN_NO_CMDS
 *      CMD_ERR_PIPE_LIMIT     print on ERR_TOO_MANY_COMMANDS
 *      CMD_ERR_EXECUTE        print on execution failure of external command
 * 
 *  Standard Library Functions You Might Want To Consider Using (assignment 1+)
 *      malloc(), free(), strlen(), fgets(), strcspn(), printf()
 * 
 *  Standard Library Functions You Might Want To Consider Using (assignment 2+)
 *      fork(), execvp(), exit(), chdir()
 */

int alloc_cmd_buff(cmd_buff_t *cmd_buff) {
    cmd_buff->_cmd_buffer = NULL;
    cmd_buff->argc = 0;
    memset(cmd_buff->argv, 0, sizeof(cmd_buff->argv));
    return OK;
}

int free_cmd_buff(cmd_buff_t *cmd_buff) {
    free(cmd_buff->_cmd_buffer);
    return OK;
}

int build_cmd_buff(char *cmd_line, cmd_buff_t *cmd_buff) {
    while (isspace(*cmd_line)) cmd_line++; 
    if (*cmd_line == '\0') return WARN_NO_CMDS;
    
    char **tokens = malloc(CMD_ARGV_MAX * sizeof(char *));
    if (!tokens) return ERR_MEMORY;
    int token_count = 0;
    char *buffer = strdup(cmd_line);
    if (!buffer) {
        free(tokens);
        return ERR_MEMORY;
    }
    
    char *start = buffer;
    int in_quotes = 0;
    char quote_char = 0;
    for (char *current = buffer; *current; current++) {
        if (*current == '"' || *current == '\'') {
            if (in_quotes && *current == quote_char) {
                *current = '\0';
                in_quotes = 0;
            } else if (!in_quotes) {
                in_quotes = 1;
                quote_char = *current;
                start = current + 1;
            }
        } else if (!in_quotes && isspace(*current)) {
            *current = '\0';
            if (start != current) tokens[token_count++] = start;
            start = current + 1;
        }
    }
    if (*start) tokens[token_count++] = start;
    
    if (!token_count) {
        free(buffer);
        free(tokens);
        return WARN_NO_CMDS;
    }
    
    cmd_buff->_cmd_buffer = buffer;
    memcpy(cmd_buff->argv, tokens, token_count * sizeof(char *));
    cmd_buff->argc = token_count;
    cmd_buff->argv[token_count] = NULL;
    free(tokens);
    return OK;
}

int execute_pipeline(command_list_t *clist) {
    if (!clist || clist->num == 0) {
        fprintf(stderr, CMD_WARN_NO_CMD);
        return WARN_NO_CMDS;
    }
    int num_cmds = clist->num;
    int *pipefds = NULL;    
    if (num_cmds > 1) {
        pipefds = malloc(2 * (num_cmds - 1) * sizeof(int));
        if (!pipefds)
            return ERR_MEMORY;
        for (int i = 0; i < num_cmds - 1; i++) {
            if (pipe(pipefds + i * 2) == -1) {
                perror("pipe");
                free(pipefds);
                return ERR_EXEC_CMD;
            }
        }
    }
    pid_t pids[num_cmds];
    for (int i = 0; i < num_cmds; i++) {
        pid_t pid = fork();
        if (pid == 0) {
            if (i > 0) {
                if (dup2(pipefds[(i - 1) * 2], STDIN_FILENO) == -1) {
                    perror("dup2");
                    exit(ERR_EXEC_CMD);
                }
            }
            if (i < num_cmds - 1) {
                if (dup2(pipefds[i * 2 + 1], STDOUT_FILENO) == -1) {
                    perror("dup2");
                    exit(ERR_EXEC_CMD);
                }
            }
            if (pipefds != NULL) {
                for (int j = 0; j < 2 * (num_cmds - 1); j++) {
                    close(pipefds[j]);
                }
            }
            execvp(clist->commands[i].argv[0], clist->commands[i].argv);
            fprintf(stderr, "error: fail to execute external commands\n");
            exit(ERR_EXEC_CMD);
        } else if (pid > 0) {
            pids[i] = pid;
        } else {
            perror("fork");
            if (pipefds != NULL)
                free(pipefds);
            return ERR_EXEC_CMD;
        }
    }
    if (pipefds != NULL) {
        for (int i = 0; i < 2 * (num_cmds - 1); i++) {
            close(pipefds[i]);
        }
        free(pipefds);
    }
    int status;
    for (int i = 0; i < num_cmds; i++) {
        waitpid(pids[i], &status, 0);
    }
    return OK;
}

Built_In_Cmds match_command(const char *input) {
    if (strcmp(input, EXIT_CMD) == 0) return BI_CMD_EXIT;
    if (strcmp(input, "cd") == 0) return BI_CMD_CD;
    if (strcmp(input, "dragon") == 0) return BI_CMD_DRAGON;
    return BI_NOT_BI;
}

Built_In_Cmds exec_built_in_cmd(cmd_buff_t *cmd) {
    switch (match_command(cmd->argv[0])) {
        case BI_CMD_EXIT:
            exit(249);
        case BI_CMD_CD:
            if (cmd->argc > 1) chdir(cmd->argv[1]);
            return BI_EXECUTED;
        case BI_CMD_DRAGON:
            printf("DRAGON: extra credit not implemented\n");
            return BI_EXECUTED;
        default:
            return BI_NOT_BI;
    }
}

int exec_cmd(cmd_buff_t *cmd) {
    pid_t pid = fork();
    if (pid == 0) {
        execvp(cmd->argv[0], cmd->argv);
        fprintf(stderr, "error: fail to execute external commands\n");
        exit(ERR_EXEC_CMD);
    } else if (pid > 0) {
        int status;
        waitpid(pid, &status, 0);
        return WEXITSTATUS(status);
    }
    return ERR_EXEC_CMD;
}

int exec_local_cmd_loop() {
    char cmd_line[SH_CMD_MAX];
    while (1) {
        printf("%s", SH_PROMPT);
        if (!fgets(cmd_line, SH_CMD_MAX, stdin)) {
            printf("\n");
            break;
        }
        cmd_line[strcspn(cmd_line, "\n")] = '\0';
        
        if (strlen(cmd_line) == 0) {
            printf("warning: no commands provided\n");
            continue;
        }
        
        if (strchr(cmd_line, '|')) {
            command_list_t clist;
            clist.num = 0;
            char *token, *saveptr;
            token = strtok_r(cmd_line, "|", &saveptr);
            while (token != NULL) {
                while (isspace(*token)) token++;
                if (*token == '\0') {
                    token = strtok_r(NULL, "|", &saveptr);
                    continue;
                }
                
                cmd_buff_t temp;
                alloc_cmd_buff(&temp);
                if (build_cmd_buff(token, &temp) != OK) {
                    free_cmd_buff(&temp);
                    token = strtok_r(NULL, "|", &saveptr);
                    continue;
                }
                if (clist.num >= 8) {
                    fprintf(stderr, "error: piping limited to 8 commands\n");
                    free_cmd_buff(&temp);
                    break;
                }
                clist.commands[clist.num++] = temp;
                token = strtok_r(NULL, "|", &saveptr);
            }
            if (clist.num == 0) {
                fprintf(stderr, CMD_WARN_NO_CMD);
                continue;
            }
            execute_pipeline(&clist);
            for (int i = 0; i < clist.num; i++) {
                free_cmd_buff(&clist.commands[i]);
            }
        } else {
            cmd_buff_t cmd;
            alloc_cmd_buff(&cmd);
            if (build_cmd_buff(cmd_line, &cmd) != OK) {
                free_cmd_buff(&cmd);
                continue;
            }
            if (exec_built_in_cmd(&cmd) == BI_NOT_BI) {
                exec_cmd(&cmd);
            }
            free_cmd_buff(&cmd);
        }
    }
    return OK;
}
