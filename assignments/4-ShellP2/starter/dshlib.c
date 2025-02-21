#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/wait.h>
#include "dshlib.h"

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
    cmd_buff_t cmd;
    alloc_cmd_buff(&cmd);

    while (1) {
        printf("%s", SH_PROMPT);
        if (!fgets(cmd_line, SH_CMD_MAX, stdin)) {
            printf("\n");
            break;
        }
        cmd_line[strcspn(cmd_line, "\n")] = '\0';
        
        if (strlen(cmd_line) == 0) {
            printf("no commands provided\n");
            continue;
        }
        
        if (build_cmd_buff(cmd_line, &cmd) != OK) continue;
        
        if (exec_built_in_cmd(&cmd) == BI_NOT_BI) exec_cmd(&cmd);
        
        free_cmd_buff(&cmd);
        alloc_cmd_buff(&cmd);
    }

    free_cmd_buff(&cmd);
    return OK;
}
