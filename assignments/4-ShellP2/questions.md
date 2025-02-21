1. Can you think of why we use `fork/execvp` instead of just calling `execvp` directly? What value do you think the `fork` provides?

    > **Answer**:  Calling execvp directly replaces the current process image with a new program, meaning the shell itself would be replaced and unable to continue handling user commands. By using fork, we create a child process that executes the new command while the parent remains running and can continue accepting input after the command completes.

2. What happens if the fork() system call fails? How does your implementation handle this scenario?

    > **Answer**:  If fork() fails, it typically returns -1, indicating that no new process was created.

3. How does execvp() find the command to execute? What system environment variable plays a role in this process?

    > **Answer**:  execvp() searches for the specified command in the directories listed in the PATH environment variable.

4. What is the purpose of calling wait() in the parent process after forking? What would happen if we didn’t call it?

    > **Answer**:  The wait() system call ensures that the parent process waits for the child process to complete before continuing. Without wait(), the parent could move on without knowing when the child finishes.

5. In the referenced demo code we used WEXITSTATUS(). What information does this provide, and why is it important?

    > **Answer**: WEXITSTATUS() extracts the exit status of a terminated child process. It allows the shell to determine if a command completed successfully or encountered an error.

6. Describe how your implementation of build_cmd_buff() handles quoted arguments. Why is this necessary?

    > **Answer**:  My implementation preserves spaces within quoted arguments, ensuring they are treated as a single command-line argument. For certain inputs, it should be considered one argument instead of two.

7. What changes did you make to your parsing logic compared to the previous assignment? Were there any unexpected challenges in refactoring your old code?

    > **Answer**: I restructured the parsing logic to handle a single cmd_buff instead of a command list. Handling quoted arguments and eliminating extra spaces while preserving quoted content required significant adjustments. One unexpected challenge was ensuring consistent behavior across different input formats.

8. For this quesiton, you need to do some research on Linux signals. You can use [this google search](https://www.google.com/search?q=Linux+signals+overview+site%3Aman7.org+OR+site%3Alinux.die.net+OR+site%3Atldp.org&oq=Linux+signals+overview+site%3Aman7.org+OR+site%3Alinux.die.net+OR+site%3Atldp.org&gs_lcrp=EgZjaHJvbWUyBggAEEUYOdIBBzc2MGowajeoAgCwAgA&sourceid=chrome&ie=UTF-8) to get started.

- What is the purpose of signals in a Linux system, and how do they differ from other forms of interprocess communication (IPC)?

    > **Answer**:  Signals are used to asynchronously notify a process of events like termination requests or interrupts. Unlike pipes, message queues, or shared memory, signals primarily serve as control mechanisms to influence process behavior.

- Find and describe three commonly used signals (e.g., SIGKILL, SIGTERM, SIGINT). What are their typical use cases?

    > **Answer**:  SIGKILL: Forces a process to terminate immediately; it cannot be caught or ignored. 
    
    SIGTERM: Politely asks a process to terminate, allowing it to perform cleanup before exiting.
    
    SIGINT: Sent when a user presses Ctrl+C, allowing a process to handle interruption gracefully.

- What happens when a process receives SIGSTOP? Can it be caught or ignored like SIGINT? Why or why not?

    > **Answer**:  SIGSTOP unconditionally suspends a process. Unlike SIGINT, it cannot be caught, ignored, or handled by the process. This is enforced by the kernel to ensure process suspension is always possible when required.