1. Your shell forks multiple child processes when executing piped commands. How does your implementation ensure that all child processes complete before the shell continues accepting user input? What would happen if you forgot to call waitpid() on all child processes?

When the shell runs a pipeline, it creates a separate child process for each command and records their process IDs. After all the processes start, the shell calls waitpid() for each child, ensuring that it waits until every command finishes before showing a new prompt. If the shell did not wait for these children, finished processes would remain as zombies and could mix their output with new prompts, leading to resource issues and a confusing display.

2. The dup2() function is used to redirect input and output file descriptors. Explain why it is necessary to close unused pipe ends after calling dup2(). What could go wrong if you leave pipes open?

The dup2() function is used to redirect standard input or output to a pipe. After the redirection, the original pipe file descriptors are still open. It is important to close these unused descriptors to prevent running out of file handles and to ensure that the reading side of the pipe can receive an end-of-file signal. If these ends remain open, a process might hang waiting for input, and system resources could be wasted.

3. Your shell recognizes built-in commands (cd, exit, dragon). Unlike external commands, built-in commands do not require execvp(). Why is cd implemented as a built-in rather than an external command? What challenges would arise if cd were implemented as an external process?

The cd command is implemented as a built-in command because it needs to change the current directory of the shell itself. If cd were run as an external command in a child process, it would only change that process’s directory, and the parent shell’s working directory would remain unchanged. This would make the command ineffective for the user.
  
4. Currently, your shell supports a fixed number of piped commands (CMD_MAX). How would you modify your implementation to allow an arbitrary number of piped commands while still handling memory allocation efficiently? What trade-offs would you need to consider?

To allow an arbitrary number of piped commands instead of a fixed limit, the implementation can use dynamic memory allocation. Instead of relying on a fixed-size array, the shell could use a dynamically growing structure that allocates memory as needed and frees it later. While this approach makes the shell more flexible, it also adds complexity to memory management and could affect performance if not handled properly.