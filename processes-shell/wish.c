#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <assert.h>

#define DEBUG false //debug mode
#define ERROR_MSG "An error has occurred\n"
#define PROMPT_MSG "wish> "
#define DELIM " \t\n"
#define COMMAND_DELIM "&"
#define REDIRECT_OPERATOR ">"
#define STREQUAL(a, b) (strcmp(a,b)== 0)

char* path[100] = {NULL,}; // finiteness assumed ( < 100 )
int path_cnt = 1;
bool interactive = true; // print PROMPT_MSG or not


int do_built_in(char* command); // return : -1 error, 0 no built_in, 1 yes built_in
int execute(char* command); // return :-1 error, else child pid
char* strsep_repitition(char** command); 
char** parse_args(char* args_str); // dynamically allocate
char* path_join(char*str1, char* str2);



int main(int argc, char* argv[]) {
    path[0] = strdup("/bin"); // dynamic allocating with default path dir
    // invalid arguments 
    if (argc > 2) { 
        fprintf(stderr, ERROR_MSG);
        exit(1);
    }

    // input as a batch file
    // redirect stdin to the batch input file
    if (argc == 2) { 
        if (freopen(argv[1], "r", stdin) == NULL) {
            fprintf(stderr, ERROR_MSG);
            exit(1);
        }
        interactive = false;
    }

    /*
    Interactive shell
    */
    char* line = NULL;
    size_t len = 0;
    ssize_t read;

    while (1) {
        if (interactive) {printf(PROMPT_MSG);} // prompt msg
        if ( (read = getline(&line, &len, stdin)) == -1) break; // read line
        if ( line[read-1] == '\n') {line[read-1] = '\0';} // delete new line character


        int pids[100]; // less than 100 parallel command assumed
        memset(pids, -1, sizeof(pids));
        int pid_cnt = 0;

        char* command; // one independent command (could be seperated by &)
        while ( (command = strsep(&line, COMMAND_DELIM)) != NULL) {
            while (*command == ' ') command++; // left trim
            if (command[0] == '\0') continue; // no command

            //////////////// built-in /////////////////
            char* command_dup = strdup(command);
            int ret = do_built_in(command_dup);
            free(command_dup);
            if (ret == -1) {
                fprintf(stderr, ERROR_MSG);
                continue;
            } else if (ret == 1) { // yes, it was a built-in command
                continue;
            }

            /////////////// execute file ///////////////
             ret = execute(command);
             if (ret == -1) { // something goes wrong within it
                fprintf(stderr, ERROR_MSG);
             }
             else { // child pid returned
                pids[pid_cnt++] = ret;
             }

             if (DEBUG) {printf("\n");}
        }
        free(line);
        line = NULL; len =0;

        // waiting for child processes to be terminated
        for (int i=0; i<pid_cnt; ++i) {
            int status;
            int terminated_pid = waitpid(pids[i],&status, 0);
            assert(terminated_pid == pids[i]);
        }
    }
    return 0;
}

int do_built_in(char* command) {
/*
return 
     0 - not a built-in command
    -1 - error
     1 - successfully handled
*/ 
    char* cmd = strsep_repitition(&command);
    char* arg;

    // exit
    if (STREQUAL(cmd, "exit")) {
        // no remaining args should be left
        if ( (arg = strsep_repitition(&command)) != NULL) {
            return -1;
        } else {
            exit(0); 
        }
    }
    // cd
    else if (STREQUAL(cmd, "cd")) {
        arg = strsep_repitition(&command); // path

        // no remaining args
        if (arg == NULL || strsep_repitition(&command) != NULL) { 
            return -1;
        } else { 
            if (chdir(arg) == -1) {
                return -1;
            }
        }
        return 1;
    }
    // path
    else if (STREQUAL(cmd, "path")) {
        for (int i=0; i<path_cnt; ++i) {
            free(path[i]);
        }

        path_cnt = 0;
        while ((arg = strsep_repitition(&command)) != NULL) {
            path[path_cnt++] = strdup(arg);
        }
        return 1;
    }
    return 0; // was not built-in command
}


int execute(char* command) {
/*
params
    command - one command (not parrarel multiple commands, chained with "&")
            - example ("ls -l dir1/dir2 > output.txt")
return
    -1 - error
     

*/
    char* args_str = strsep(&command, REDIRECT_OPERATOR); // example above ("ls -l dir1/dir2 ")
    char** args = parse_args(args_str); // ["ls", "-l", "dir1/dir2", NULL]
    char** envp = {NULL};

    char* output_path = strsep(&command, REDIRECT_OPERATOR); // example above (" output.txt")
    while (output_path!=NULL && *output_path == ' ') output_path++; // left trim ("output.txt")

    // at least one args needed (except for last NULL) 
    if (args[0] == NULL) {
        if (DEBUG) {
            printf("no command is specified\n");
        }
        return -1;
    }

    // output_path validity check
    if (output_path!= NULL) {

        // multiple redirection operator (example : ls >> output.txt  // ls > o1.txt > o2.txt ..)
        if (strsep(&command, REDIRECT_OPERATOR) != NULL) {
            if (DEBUG) {
                printf("multiple redirection operator\n");
            }
            return -1;
        }
        
        // no output_path (example : "ls >", "ls >  ")
        if (output_path[0] == '\0') {
            if (DEBUG) {
                printf("no output_path specified\n");
            }
            return -1;
        }

        // multiple args are remaining (example : "ls > o1.txt o2.txt")
        char* temp = strtok(output_path, DELIM);
        assert(temp == output_path);
        temp = strtok(NULL, DELIM);

        if (temp != NULL) { // something reamining after output path
            if (DEBUG) {
                printf("multilple redirection paths are given\n");
            }
            return -1; 
        }
    } // output_path validity check end

    // find executable file
    char* exe_path = NULL;  
    for (int i=0; i<path_cnt; ++i) {
        char* exe_path_cand = path_join(path[i], args[0]);
        fflush(stdout);
        if (access(exe_path_cand, X_OK) == 0) {
            // can execute it
            exe_path = exe_path_cand;
            break;
        }
        free(exe_path_cand);
    }
    if (exe_path == NULL) { // didn't find it
        return -1;
    }

    // execute!
    pid_t pid = fork();
    if (pid == -1) { // fork failed
        if (DEBUG)  {
            printf("fork failed\n");
        }
        return -1;
    }

    if (pid > 0) { // parent
        return pid; //return child pid
    } else { // child
        // redirection if needed
        if (output_path != NULL) {
            if (DEBUG) {
                printf("redirected to %s\n", output_path);
            }
            if (freopen(output_path, "w", stdout) == NULL) {
                
                fprintf(stderr, ERROR_MSG);
                exit(1);
            }
        }
        if (execve(exe_path, args, envp) == -1) {
            // execution failed
            fprintf(stderr, ERROR_MSG);
            exit(1);
        }

        // should not reach here
        assert(false);
        exit(1);
    }
}

char* strsep_repitition(char** command) {
    char* ret = NULL;
    while ( (ret= strsep(command, DELIM)) != NULL ) {
        if (ret[0] != '\0') return ret;
    }
    return ret;
}


char** parse_args(char* args_str) {
    char* token;
    char** argv = NULL;

    int cnt = 0;
    while ( (token = strsep_repitition(&args_str)) != NULL) {
        argv = realloc(argv, (cnt+1) * sizeof(char*));
        argv[cnt++] = strdup(token);
        if (DEBUG) {
            printf("arg %d : %s\n", cnt, token);
        }
    }

    // add NULL at the end
    argv = realloc(argv, (cnt+1) * sizeof(char*));
    argv[cnt++] = NULL;
    return argv;
}

char* path_join(char* str1, char* str2) {
    char *result = (char *)malloc(strlen(str1) + strlen(str2) + 2);

    // Check if memory allocation was successful
    if (result == NULL) {
        return NULL;
    }

    strcpy(result, str1);
    result[strlen(str1)] = '/';
    result[strlen(str1)+1] = '\0';
    strcat(result, str2);
    return result;
}

