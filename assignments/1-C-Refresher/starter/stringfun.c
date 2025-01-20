#include <stdio.h>
#include <string.h>
#include <stdlib.h>


#define BUFFER_SZ 50

//prototypes
void usage(char *);
void print_buff(char *, int);
int  setup_buff(char *, char *, int);

//prototypes for functions to handle required functionality
int  count_words(char *, int, int);
//add additional prototypes here


int setup_buff(char *buff, char *user_str, int len){
    //TODO: #4:  Implement the setup buff as per the directions
    if (!buff || !user_str) {
            return -2; // Error: NULL pointer passed
        }
        int count = 0;
        while (*user_str && count < len) {
            if (*user_str == ' ' || *user_str == '\t') {
                if (count == 0 || *(buff - 1) == ' ') {
                    user_str++;
                    continue;
                }
                *buff++ = ' ';
                count++;
            } else {
                *buff++ = *user_str;
                count++;
            }
            user_str++;
        }
        if (*(buff - 1) == ' ') {
            buff--;
            count--;
        }
        while (count < len) {
            *buff++ = '.';
            count++;
        }
        if (*user_str) {
            exit(3);
        }
        return count; 
    }

void print_buff(char *buff, int len){
    printf("Buffer:  ");
    for (int i=0; i<len; i++){
        putchar(*(buff+i));
    }
    putchar('\n');
}

void usage(char *exename){
    printf("usage: %s [-h|c|r|w|x] \"string\" [other args]\n", exename);

}

int count_words(char *buff, int len, int str_len){
    int word_count = 0;
    int in_word = 0;
    for (int i = 0;  i < len && i < str_len; i++) {
        if (buff[i] != ' ' && buff[i] != '\0') {
            if (!in_word) {
                in_word = 1;
                word_count++;
            }
        } else {
            in_word = 0;
        }
    }
    return word_count;
}
//ADD OTHER HELPER FUNCTIONS HERE FOR OTHER REQUIRED PROGRAM OPTIONS

int main(int argc, char *argv[]){

    char *buff;             //placehoder for the internal buffer
    char *input_string;     //holds the string provided by the user on cmd line
    char opt;               //used to capture user option from cmd line
    int  rc;                //used for return codes
    int  user_str_len;      //length of user supplied string

    //TODO:  #1. WHY IS THIS SAFE, aka what if arv[1] does not exist?
    //      PLACE A COMMENT BLOCK HERE EXPLAINING
    //The purpose is to check if there are enough arguments. If there is not then it will tell the user and exit
    if ((argc < 2) || (*argv[1] != '-')){
        usage(argv[0]);
        exit(1);
    }

    opt = (char)*(argv[1]+1);   //get the option flag

    //handle the help flag and then exit normally
    if (opt == 'h'){
        usage(argv[0]);
        exit(0);
    }

    //WE NOW WILL HANDLE THE REQUIRED OPERATIONS

    //TODO:  #2 Document the purpose of the if statement below
    //      PLACE A COMMENT BLOCK HERE EXPLAINING
    //The purpose is to check if there are enough arguments. If there is not then it will tell the user and exit
    if (argc < 3){
        usage(argv[0]);
        exit(1);
    }

    input_string = argv[2]; //capture the user input string

    //TODO:  #3 Allocate space for the buffer using malloc and
    //          handle error if malloc fails by exiting with a 
    //          return code of 99
    // CODE GOES HERE FOR #3
    buff = (char*) malloc(BUFFER_SZ * sizeof(char));
    if (buff == NULL) {
        exit(99);
    }

    user_str_len = setup_buff(buff, input_string, BUFFER_SZ);     //see todos
    if (user_str_len < 0){
        printf("Error setting up buffer, error = %d", user_str_len);
        exit(2);
    }

    switch (opt){
        case 'c':
            rc = count_words(buff, BUFFER_SZ, user_str_len);  //you need to implement
            if (rc < 0){
                printf("Error counting words, rc = %d", rc);
                exit(2);
            }
            printf("Word Count: %d\n", rc);
            break;

        //TODO:  #5 Implement the other cases for 'r' and 'w' by extending
        //       the case statement options
        case 'r':
            int reverse_len = 0;
            while (reverse_len < BUFFER_SZ && buff[reverse_len] != '.') {
                reverse_len++;
            }
            char *start = buff;
            char *end = buff + reverse_len - 1;
            while (start < end) {
                char temp = *start;
                *start++ = *end;
                *end-- = temp;
            }
            break;
        case 'w':
            printf("Word Print\n");
            printf("----------\n");
            int word_start = 0;
            int word_index = 1;
            for (int i = 0; i <= user_str_len; i++) {
                if (*(buff + i) == ' ' || *(buff + i) == '.' ||  *(buff + i) == '\0'|| i == user_str_len) {
                    if (i > word_start) {
                        int word_len = i - word_start;
                        printf("%d. ", word_index++);
                        for (int j = word_start; j < i; j++) {
                            putchar(*(buff + j));
                        }
                        printf(" (%d)\n", word_len);
                    }
                    word_start = i + 1;  
                }
            }
            rc = count_words(buff, BUFFER_SZ, user_str_len);  //you need to implement
            if (rc < 0){
                printf("Error counting words, rc = %d", rc);
                exit(2);
            }
            printf("\nWord Count: %d\n", rc);
            break;
        case 'x':
            if (argc != 5) {
                printf("Not Implemented!\n");
            exit(3);
            }
            break;

        default:
            usage(argv[0]);
            exit(1);
    }

    //TODO:  #6 Dont forget to free your buffer before exiting
    print_buff(buff,BUFFER_SZ);
    free(buff);
    exit(0);
}

//TODO:  #7  Notice all of the helper functions provided in the 
//          starter take both the buffer as well as the length.  Why
//          do you think providing both the pointer and the length
//          is a good practice, after all we know from main() that 
//          the buff variable will have exactly 50 bytes?
//  
//          PLACE YOUR ANSWER HERE
// This is so that we can change the number of bytes without needing the change all the code. Since it is not hardcoded
// we are able to increase or decrease the number of bytes easily. 