#include "../header/main.h"

int cd(DirectoryTree* currentDirectoryTree, char* cmd)
{
    DirectoryNode* tempNode = NULL;
    char* str = NULL;
    char tmp[MAX_DIR];
    int val;

    if(cmd == NULL){
        strcpy(tmp, userList->current->dir);
        movePath(currentDirectoryTree, tmp);
    }
    else if(cmd[0] == '-'){
        if(strcmp(cmd, "--help") == 0){
            printf("cd: cd [dir]\n");
            printf("    Change the shell working directory.\n\n");
            printf("    Change the current directory to DIR. The default DIR is the value of the\n");
            printf("    HOME shell variable.\n\n");
            printf("    The variable CDPATH defines the search path for the directory containing\n");
            printf("    DIR. Alternative directory names in CDPATH are separated by a colon (:).\n");
            printf("    A null directory name is the same as the current directory. If DIR begins\n");
            printf("    with a slash (/), then CDPATH is not used.\n\n");
            printf("    If the directory is not found, and the shell option `cdable_vars' is set,\n");
            printf("    the word is assumed to be a variable name. If that variable has a value,\n");
            printf("    its value is used for DIR.\n\n");
            printf("    Options:\n");
            printf("      --help     display this help and exit\n");
            return ERROR;
        }
    
        else {
            if (cmd[1] == '\0') {
                printf("cd: Invalid option -- 'none'\n");
            } else if (cmd[1] == '-') {
                if (cmd[2] == '\0') {
                    printf("cd: Invalid option -- '-'\n");
                } else {
                    printf("cd: Invalid option -- '-'\n");
                }
            } else {
                printf("cd: Invalid option -- '%c'\n", cmd[1]);
            }
            printf("Try 'cd --help' for more information.\n");
            return ERROR;
        }
    }
    else{
        tempNode = IsExistDir(currentDirectoryTree, cmd, 'd');
        if(tempNode != NULL){
            if(HasPermission(tempNode, 'r') != 0){
                printf("-bash: cd: '%s': Permission denied.\n", cmd);
                return ERROR;
            }
        }
        tempNode = IsExistDir(currentDirectoryTree, cmd,  'f');
        if(tempNode != NULL){
            printf("-bash: cd: '%s': Not a directory.\n", cmd);
            return ERROR;
        }
        val = movePath(currentDirectoryTree, cmd);
        if(val != 0){
            printf("-bash: cd: '%s': no such file or directory\n", cmd);
            return ERROR;
        }
    }
        return 0;
}