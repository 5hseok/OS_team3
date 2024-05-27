#include "../header/main.h"


int HasPermission(DirectoryNode* dirNode, char o)
{
    if(userList->current->id.UID == 0)
        return 0;

    if(userList->current->id.UID == dirNode->id.UID){
        if(o == 'r'){
            if(dirNode->permission.permission[0] == 0)
                return -1;
            else
                return 0;
        }
        if(o == 'w'){
            if(dirNode->permission.permission[1] == 0)
                return -1;
            else
                return 0;
        }
        if(o == 'x'){
            if(dirNode->permission.permission[2] == 0)
                return -1;
            else
                return 0;
        }
    }
    else if(userList->current->id.GID == dirNode->id.GID){
        if(o == 'r'){
            if(dirNode->permission.permission[3] == 0)
                return -1;
            else
                return 0;
        }
        if(o == 'w'){
            if(dirNode->permission.permission[4] == 0)
                return -1;
            else
                return 0;
        }
        if(o == 'x'){
            if(dirNode->permission.permission[5] == 0)
                return -1;
            else
                return 0;
        }
    }
    else{
        if(o == 'r'){
            if(dirNode->permission.permission[6] == 0)
                return -1;
            else
                return 0;
        }
        if(o == 'w'){
            if(dirNode->permission.permission[7] == 0)
                return -1;
            else
                return 0;
        }
        if(o == 'x'){
            if(dirNode->permission.permission[8] == 0)
                return -1;
            else
                return 0;
        }
    }
    return -1;
}

int modeToPermission(DirectoryNode *dirNode) {
    char buf[4];
    int tmp;
    int j;

    for (int i = 0; i < 9; i++) {
        dirNode->permission.permission[i] = 0;
    }
    sprintf(buf, "%d", dirNode->permission.mode);

    for (int i = 0; i < 3; i++) {
        tmp = buf[i] - '0';
        j = 2;
        while (tmp) {
        dirNode->permission.permission[3 * i + j] = tmp % 2;
        tmp /= 2;
        j--;
        }
    }
    return SUCCESS;
}

