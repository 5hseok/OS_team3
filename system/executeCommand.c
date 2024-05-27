#include "../header/main.h"

void executeCommand(DirectoryTree *currentDirectoryTree, char *command) {
    char *var;
    char *var1;
    char *var2;
    char *cmdRest;

    if (!strcmp(command, "") || command[0] == ' ') return;

    var = strtok(command, " ");

    if (!strcmp(var, "mkdir")) {
        cmdRest = command + (var - command) + strlen(var) + 1;  // 명령어 나머지 부분을 가져오기
        Mkdir(currentDirectoryTree, cmdRest);
    } else if (!strcmp(var, "touch")) {
        // touch 코드 구현
        cmdRest = command + (var - command) + strlen(var) + 1;  // 명령어 나머지 부분을 가져오기
        touch_option(currentDirectoryTree, cmdRest);
    } else if (!strcmp(var, "mv")) {
        cmdRest = command + (var - command) + strlen(var) + 1;  // 명령어 나머지 부분을 가져오기
        // mv 코드 구현
        mv(currentDirectoryTree, cmdRest);
    } else if (!strcmp(var, "cp")) {
        cmdRest = command + (var - command) + strlen(var) + 1;
        cp(currentDirectoryTree, cmdRest);
    } else if (!strcmp(var, "cd")) {
        var = strtok(NULL, " ");
        cd(currentDirectoryTree, var);
    } else if (!strcmp(var, "pwd")) {
        var = strtok(NULL, " ");
        pwd(currentDirectoryTree, dirStack, var);
    } else if (!strcmp(var, "ls")) {
        var = strtok(NULL, " ");
        ls(currentDirectoryTree, var);
    } else if (!strcmp(var, "cat")) {
        cmdRest = command + (var - command) + strlen(var) + 1;
        cat(currentDirectoryTree, cmdRest);
    } else if (!strcmp(var, "chmod")) {
        var = strtok(NULL, " ");
        if (var == NULL) {
            printf("chmod: Invalid option\n");
            printf("Try 'chmod --help' for more information.\n");
            return;
        }
        var1 = strtok(NULL, " ");
        if (var1 == NULL){
            printf("chmod: missing operand after \'%s\'\n", var);
            printf("Try \'chmod --help\'for more informatiln.\n");
            return;
        }
        ch_mod(currentDirectoryTree, var, var1);
    } else if (!strcmp(var, "grep")) {
        // grep 코드 구현
        cmdRest = command + (var - command) + strlen(var) + 1;  // 명령어 나머지 부분을 가져오기
        grep_option(currentDirectoryTree, cmdRest);
    } else if (!strcmp(var, "more")) {
        var = strtok(NULL, " ");
        more(var);
    } else if (!strcmp(command, "exit")) {
        printf("logout\n");
        exit(0);
    
    } else printf("Command '%s' not found\n", var);
    return;
}