1. In this assignment I suggested you use `fgets()` to get user input in the main while loop. Why is `fgets()` a good choice for this application?

    > **Answer**:  fgets() is ideal because it reads input line‐by‐line

2. You needed to use `malloc()` to allocte memory for `cmd_buff` in `dsh_cli.c`. Can you explain why you needed to do that, instead of allocating a fixed-size array?

    > **Answer**:  It would allow dynamic memory allocation for long inputs beyond SH_CMD_MAX


3. In `dshlib.c`, the function `build_cmd_list(`)` must trim leading and trailing spaces from each command before storing it. Why is this necessary? If we didn't trim spaces, what kind of issues might arise when executing commands in our shell?

    > **Answer**: Trimming whitespace ensures that extra spaces do not become part of the command or its arguments. The executable name could be prefixed by spaces and thus not match the expected filename. Trailing spaces might split arguments incorrectly

4. For this question you need to do some research on STDIN, STDOUT, and STDERR in Linux. We've learned this week that shells are "robust brokers of input and output". Google _"linux shell stdin stdout stderr explained"_ to get started.

- One topic you should have found information on is "redirection". Please provide at least 3 redirection examples that we should implement in our custom shell, and explain what challenges we might have implementing them.

    > **Answer**:  command > file: Redirect STDOUT to file.
    Challenge: Opening/creating files, replacing stdout with file descriptor.
    command 2> error.log: Redirect STDERR to error.log.
    Challenge: Separating STDERR/STDOUT streams in child processes.
    command >> file: Append STDOUT to file.
    Challenge: Handling file appends without overwriting.

- You should have also learned about "pipes". Redirection and piping both involve controlling input and output in the shell, but they serve different purposes. Explain the key differences between redirection and piping.

    > **Answer**: Redirection connects a command’s input/output to a file.Piping connects the output of one command to the input of another. Redirection interacts with files; piping links processes 

- STDERR is often used for error messages, while STDOUT is for regular output. Why is it important to keep these separate in a shell?

    > **Answer**: Avoid mixing errors with normal output and allow selective logging or debugging.

- How should our custom shell handle errors from commands that fail? Consider cases where a command outputs both STDOUT and STDERR. Should we provide a way to merge them, and if so, how?

    > **Answer**: The shell should capture the exit status of each command. When a command fails, its error messages should be displayed or logged so that the user is informed. Yeah we can allow for them to merge STDERR into STDOUT. We could use 2>&1 to redirect STDERR to STDOUT.