#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <sys/types.h>
#include <unistd.h>
#include <sys/wait.h>
#include <fcntl.h>




const int MAXARGS = 1024;

const int MAXBUFFERSIZE = 4096;

struct path{
    char *PATH;
    struct path *next;
    struct path *pervious;
};

void printArgs(char *args[1024]){
    printf("++++++++++++++++++++++++++++++++++++++\n");
    for(int i = 0; args[i] != NULL; i++){
        printf("args[%d]{%s}%ld\n", i,args[i], strlen(args[i]));
    }
    printf("++++++++++++++++++++++++++++++++++++++\n");
}
void addPath(struct path *header, char *p){
    if(header->next == NULL){
        header->next = malloc(sizeof(struct path));
        header->next->next = NULL;
        header->next->pervious = header;
        header->next->PATH = strdup(p);
    }
    else{
        struct path *new = malloc(sizeof(struct path));
        new->next = header->next;
        new->pervious = header;
        new->PATH = strdup(p);
        header->next = new;
        new->next->pervious = new;
    }
}

bool removePath(struct path *header, char *p){
    if(p == NULL){
        return false;
    }
    struct path *current = header->next;
    bool found = false;
    while(current != NULL){
        struct path *pervious = current->pervious;
        struct path *next = current->next;
        if(strcmp(current->PATH, p) == 0){
            found = true;
            free(current->PATH);
            free(current);
            if(next == NULL){
                pervious->next = NULL;
            }
            else{
                pervious->next = next;
                next->pervious = pervious;
            }
        }
        current = next;
    }
    return found;
}

void clear(struct path *header){
    struct path *current = header->next;
    header->next = NULL;
    while(current != NULL){
        struct path *next = current->next;
        free(current->PATH);
        free(current);
        current = next;
    }
}

void error(){
    char error_message[30] = "An error has occurred\n";
    write(STDERR_FILENO, error_message, strlen(error_message));  
}

void freeArgs(char *args[MAXARGS]){
    for(int i = 0; args[i] != NULL; i++){
        free(args[i]);
    }
    for(int i = 0; args[i] != NULL; i++){
        args[i] = NULL;
    }
}

bool hasDelims(char *str, char *delims){
    for(int i = 0; i < strlen(str); i++){
        for(int j = 0; j < strlen(delims); j++){
            if(str[i] == delims[j]){
                return true;
            }
        }
    }
    return false;
}

bool tokenize1(char *cmd, char *args[MAXARGS], char* delim){
    char *string, *token;
    for(int i = 0; i < MAXARGS; i++){
        args[i] = NULL;
    }
    int counter = 0;
    string = strdup(cmd);
    while((token = strsep(&string, delim)) != NULL){
        if(strlen(token) >= 1){
            args[counter++] = strdup(token);
        }
    }

    // if(strcmp(args[0], "path") == 0 && (strcmp(args[1], "add")==0 || strcmp(args[1], "remove")==0) && args[3] != NULL && toArgs == true){
    //     char *buffer = malloc(MAXBUFFERSIZE);
    //     strcpy(buffer, args[2]);
    //     for(int i = 3; args[i] != NULL; i++){
    //         strcat(buffer, " ");
    //         strcat(buffer, args[i]);
    //     }
    //     for(int i = 2; args[i] != NULL; i++){
    //         free(args[i]);
    //         args[i] = NULL;
    //     }
    //     args[2] = strdup(buffer);
    //     free(buffer);
    // }

    free(string);
    //////////////////////////////////
    // printf("\n++++++++++++++++++++++++++++++ 1111111111111111\n");
    // for(int i = 0; args[i] != NULL; i++){
    //     printf("args[%d]{%s}%ld\n", i,args[i], strlen(args[i]));
    // }
    // printf("\n++++++++++++++++++++++++++++++\n");
    ///////////////////////////////////
    return true; 
}

bool tokenize2(char *cmd, char *args[MAXARGS], char* delim){
    char *string, *token;
    for(int i = 0; i < MAXARGS; i++){
        args[i] = NULL;
    }
    int counter = 0;
    string = strdup(cmd);
    while((token = strsep(&string, delim)) != NULL){
        if(strlen(token) >= 1){
            if(hasDelims(token, ">&")){
                char buffer[4096]; //magic, when char buffer[MAXBUFFERSIZE] or char buffer[1024 + strlen(token)], it will strcat a SOH or STX
                buffer[0] = 0;
                for(int i = 0; i <= strlen(token); i++){
                    char c = token[i];
                    if(token[i] == '>'){
                        if(strlen(buffer) > 0){
                            args[counter++] = strdup(buffer);
                        }
                        args[counter++] = strdup(">");
                        buffer[0] = 0;
                    }
                    else if(token[i] == '&'){
                        if(strlen(buffer) > 0){
                            args[counter++] = strdup(buffer);
                        }
                        args[counter++] = strdup("&");
                        buffer[0] = 0;
                    }
                    else{
                        strcat(buffer, &c);
                    }
                }
                if(strlen(buffer)>0){
                    args[counter++] = strdup(buffer);
                }
                //printf("'%s' ", token);
            }
            else{
                args[counter++] = strdup(token);  
            }
        }
    }

    // if(strcmp(args[0], "path") == 0 && (strcmp(args[1], "add")==0 || strcmp(args[1], "remove")==0) && args[3] != NULL && toArgs == true){
    //     char *buffer = malloc(MAXBUFFERSIZE);
    //     strcpy(buffer, args[2]);
    //     for(int i = 3; args[i] != NULL; i++){
    //         strcat(buffer, " ");
    //         strcat(buffer, args[i]);
    //     }
    //     for(int i = 2; args[i] != NULL; i++){
    //         free(args[i]);
    //         args[i] = NULL;
    //     }
    //     args[2] = strdup(buffer);
    //     free(buffer);
    // }

    free(string);
    //////////////////////////////////
    // printf("\n++++++++++++++++++++++++++++++ 22222222222222\n");
    // for(int i = 0; args[i] != NULL; i++){
    //     printf("args[%d]{%s}%ld\n", i,args[i], strlen(args[i]));
    //     // if(strlen(args[i])==2){
    //     //     printf("i:%d, %d, %d,\n", i, args[i][0], args[i][1]);
    //     // }
    // }
    // printf("\n++++++++++++++++++++++++++++++\n");
    ///////////////////////////////////
    return true; 
}

bool checkBuiltIn(char *args[MAXARGS], struct path *Header){;
    if(args[0] == NULL){
        return true;
    }
    if(strcmp(args[0], "exit") == 0){
        if(args[1] == NULL){
            freeArgs(args);
            clear(Header);
            exit(0);
        }
        else{
            error();
        }
        return true;
    }

    if(strcmp(args[0], "cd") == 0){
        if(args[1] != NULL && args[2] == NULL){
            int state = chdir(args[1]);
            if(state == -1){
                //printf("Invalid Input (cannot chdir(%s))\n", args[1]);
                error();
            }
        }
	    else{
	        //printf("Invalid Input (cd should take only one argument)\n");
            error();
	    }
        return true;
    }
    if(strcmp(args[0], "path") == 0){
        if(args[1] == NULL){
            error();
            return true;
        }
        else if(strcmp(args[1], "add") == 0 && args[3] == NULL){
            addPath(Header, args[2]);
        }
        else if(strcmp(args[1], "remove") == 0 && args[3] == NULL){
            if(removePath(Header, args[2]) == false){
                //printf("Invalid Input (cannot found %s)\n", args[2]);
                error();
            }
        }
        else if(strcmp(args[1], "clear") == 0){
            clear(Header);
        }
/////////////////////////////////////////////////////////////////////////
        else if(strcmp(args[1], "print") == 0){
            struct path *temp = Header;
            while(temp != NULL){
                printf("%s, ", temp->PATH);
                temp = temp->next;
            }
            printf("\n");
        }
////////////////////////////////////////////////////////////////////////
        else {
            //printf("Invalid Input (path should follow by add, remove, clear, or print)\n");
            error();
        }
        return true;
    }
    return false;
}

void runProgram(char *args[MAXARGS], struct path *Header){
    // for(int i = 0; args[i] != NULL; i++){
    //     printf("'%s', ",args[i]);
    // }
    if(args[0] != NULL){
    //printf("\nrunProgram\n");
        struct path *current = Header->next;
        bool found = false;
        while(current != NULL){
            //printf("\n%s\n", current->PATH);
            char *path = malloc(strlen(args[0])+strlen(current->PATH)+3);
            memset(path, '\0', strlen(args[0])+strlen(current->PATH)+3);
            strcat(path, current->PATH);
            char c = '/';
            strcat(path, &c);
            strcat(path, args[0]);
            //printf("\npath:%s,\n", path);
            if(access(path, X_OK) == 0){
                //printf("YES\n");
                int child_status;
                if(fork() == 0){
    //////////////////////////////////////////////////////////////////////
                    // int n = 0;
                    // for(int i = 1; args[i] != NULL; i++){
                    //     if(strcmp(args[i], ">") == 0){
                    //         n++;
                    //     }
                    //     if(n > 1){
                    //         exit(1);
                    //     }
                    // }
                    for(int i = 1; args[i] != NULL; i++){
                        if(strcmp(args[i], ">") == 0){
                            if(args[i+1] != NULL && strcmp(args[i+1], ">") != 0 && args[i+2] == NULL){
                                int fd = open(args[i+1], O_RDWR | O_CREAT, S_IRUSR | S_IWUSR);
                                if(fd == -1){
                                    exit(1);
                                }
                                dup2(fd, 1);
                                dup2(fd, 2);
                                close(fd);
                                while(args[i] != NULL && i < MAXARGS){
                                    free(args[i]);
                                    args[i] = NULL;
                                }
                            }
                            else{
                                exit(1);
                            }
                            break;
                        }
                    }

    /////////////////////////////////////////////////////////////////
                    if(execv(path, args) == -1){
                        //printf("Invalid Input (cannot execv)\n");
                        //error();
                    }
                    exit(1);
                }
                else{
                    wait(&child_status);
                    if(child_status == 1){
                        error();
                    }
                    found = true;
                    break;
                }
            }
            free(path);
            current = current->next;
        }
        if(found == false){
            error();
        }
    }
    else{
        error();
    }
}

int redir(char *args[MAXARGS]){ 
    int sign = 0;
    for(int i = 0; args[i] != NULL && sign < 2; i++){
        if(strcmp(args[i], ">")==0){
            sign++;
        }
    }  
    if(sign == 0){
        return 0;
    }
    if(sign == 1){
        int position;
        for(int i = 0; args[i] != NULL && sign < 2; i++){
            if(strcmp(args[i], ">")==0){
                position = i;
                break;
            }
        }
        if(args[position+1] != NULL && strcmp(args[position+1], ">") != 0 && args[position+2] == NULL){
            int fd = open(args[position+1], O_RDWR | O_CREAT, S_IRUSR | S_IWUSR);
            if(fd > -1){
                dup2(fd, 1);
                dup2(fd, 2);
                close(fd);
                for(;args[position] != NULL && position < MAXARGS; position++){
                    free(args[position]);
                    args[position] = NULL;
                }
                return 1;
            }
        }
    }
    return -1; 
}

bool runProgram2(char *args[MAXARGS], struct path *Header){
    struct path *current = Header->next;
    bool found = false;
    while(current != NULL){
        char *path = malloc(strlen(args[0])+strlen(current->PATH)+3);
        memset(path, '\0', strlen(args[0])+strlen(current->PATH)+3);
        strcat(path, current->PATH);
        strcat(path, "/");
        strcat(path, args[0]);
        if(access(path, X_OK) == 0){
            found = true;
            int childStatus;
            if(fork() == 0){
                if(redir(args) == -1){
                    exit(1);
                }
                execv(path, args);
                exit(1);
            }
            else{
                wait(&childStatus);
                if(childStatus == -1){
                    return false;
                }
                break;
            }
        }
        free(path);
        current = current->next;
    }
    if(found){
        return true;
    }
    return false;
}


char* findPath(char *arg, struct path *Header){
    struct path *current = Header->next;
    while(current != NULL){
        char *path = malloc(MAXARGS);
        path[0] = 0;
        strcat(path, current->PATH);
        strcat(path, "/");
        strcat(path, arg);
        if(access(path, X_OK) == 0){
            return path;
        }
        free(path);
        current = current->next; 
    }
    return NULL;
}

void run(char *args[MAXARGS], char *path){
    if(fork() == 0){
        if(redir(args)==-1){
            exit(1);
        }
        execv(path, args);
        exit(1);
    }
}

bool runProgram2v2(char *args[MAXARGS], struct path *Header){
    char *path = findPath(args[0], Header);
    if(path != NULL){
        run(args, path);
        free(path);
    }
    else{
        return false;
    }
    int childState;
    wait(&childState);
    if(childState == 1){
        return false;
    }
    return true;
}

bool runProgram5(char *cmd[MAXARGS], struct path *Header){
    char *args[MAXARGS];
    for(int i = 0; i < MAXARGS; i++){
        args[i] = NULL;
    }
    for(int i = 0; cmd[i] != NULL; i++){
        if(cmd[i+1] == NULL){
            cmd[i+1] = strdup("&");
            break;
        }
    }
    bool alltrue = true;
    int count = 0;
    for(int i = 0; cmd[i] != NULL; i++){
        if(strcmp(cmd[i], "&") != 0){
            args[count++] = strdup(cmd[i]);
        }
        else{
            if(count > 0){
                char *path = findPath(args[0], Header);
                if(path != NULL){
                    run(args, path);
                    free(path);
                }
                else{
                    free(path);
                    return false;
                }
            }
            freeArgs(args);
            count = 0;
        }
    }
    int childState;
    while(wait(&childState) != -1){
        if(childState == 1){
            return false;
        }
    }
    return alltrue;
}


bool runProgram5v2(char *cmd[MAXARGS], struct path *Header){
    char *args[MAXARGS];
    for(int i = 0; i < MAXARGS; i++){
        args[i] = NULL;
    }
    for(int i = 0; cmd[i] != NULL; i++){
        if(cmd[i+1] == NULL){
            cmd[i+1] = strdup("&");
            break;
        }
    }
    bool alltrue = true;
    int count = 0;
    for(int i = 0; cmd[i] != NULL; i++){
        if(strcmp(cmd[i], "&") != 0 ){
            args[count++] = strdup(cmd[i]);
        }
        else{
            //printArgs(args);
            if(count > 0){
                char *path = findPath(args[0], Header);
                if(path != NULL){
                    run(args, path);
                    free(path);
                }
                else{
                    return false;
                }
            }
            freeArgs(args);
            count = 0;
        }
    }
    int childState;
    while(wait(&childState) != -1){
        if(childState == 1){
            return false;
        }
    }
    return alltrue;
}

bool runProgram4(char *args[MAXARGS], struct path *Header){
    struct path *current = Header->next;
    bool found = false;
    while(current != NULL){
        char *path = malloc(strlen(args[0])+strlen(current->PATH)+3);
        memset(path, '\0', strlen(args[0])+strlen(current->PATH)+3);
        strcat(path, current->PATH);
        char c = '/';
        strcat(path, &c);
        strcat(path, args[0]);
        if(access(path, X_OK) == 0){
            found = true;
            if(fork() == 0){
                if(redir(args) == -1){
                    exit(1);
                }
                execv(path, args);
                exit(1);
            }
            break;
        }
        free(path);
        current = current->next;
    }
    if(found){
        return true;
    }
    return false;
}

bool runProgram3(char *cmd[MAXARGS], struct path *Header){
    char *args[MAXARGS];
    for(int i = 0; i < MAXARGS; i++){
        args[i] = NULL;
    }
    int position = 0;
    for(position = 0; cmd[position] != NULL; position++);
    cmd[position] = strdup("&");
    int count = 0;
    for(int i = 0; cmd[i] != NULL; i++){
        if(strcmp(cmd[i], "&") != 0){
            args[count++] = strdup(cmd[i]);
        }
        else{
            if(runProgram4(args, Header) == false){
                return false;
            }
            freeArgs(args);
            count = 0;
        }
    }
    int childState;
    while(wait(&childState)>0){
        if(childState == 1){
            return false;
        }
    }
    return true;
}

int main(int argc, char *argv[]){
    struct path pathHeader = {.PATH = NULL, .next = NULL, .pervious = NULL};
    addPath(&pathHeader, "/bin");
    if(argc == 1){
        printf("smash> ");
        fflush(stdout);
        char *line = NULL;
        size_t len = 0;
        while(getline(&line, &len, stdin) != -1){
            for(int i = 0; i < strlen(line); i++){
                if(line[i] == '\r' || line[i] == '\n'){
                    line[i] = '\0';
                }
            }
            char *cmd[MAXARGS];
            tokenize1(line, cmd, ";");
            for(int i =0; cmd[i] != NULL; i++){
                char *args[MAXARGS];
                tokenize2(cmd[i], args, " \t"); 
                if(!checkBuiltIn(args, &pathHeader)){
                    if(!runProgram5v2(args, &pathHeader)){
                        error();
                    }
                }                
                freeArgs(args);
            }
            freeArgs(cmd);
            printf("smash> ");
            fflush(stdout);
        }
        free(line);
    }
    else if(argc == 2){
        FILE *fp = fopen(argv[1], "r");
        if(fp == NULL){
            error();
            clear(&pathHeader);
            exit(1);
        }
        char *line = NULL;
        size_t len = 0;
        while(getline(&line, &len, fp) != -1){
            for(int i = 0; i < strlen(line); i++){
                if(line[i] == '\r' || line[i] == '\n'){
                    line[i] = '\0';
                }
            }
            char *cmd[MAXARGS];
            tokenize1(line, cmd, ";");
            for(int i = 0; cmd[i] != NULL; i++){
                char *args[MAXARGS];
                tokenize2(cmd[i], args, " \t");
                if(!checkBuiltIn(args, &pathHeader)){
                    if(!runProgram5v2(args, &pathHeader)){
                        error();
                    }
                }
                freeArgs(args);
            }
            freeArgs(cmd);
        }
        free(line);
        fclose(fp);
    }
    else{
        error();
        clear(&pathHeader);
        exit(1);
    }
    clear(&pathHeader);
    return 0;
}
