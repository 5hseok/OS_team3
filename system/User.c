#include "../header/main.h"


UserList *loadUserList() {
    UserList *userList = (UserList *)malloc(sizeof(UserList));
    char tmp[MAX_NAME];

    User = fopen("system/User.txt", "r");
    while (fgets(tmp, MAX_NAME, User)) {
        readUser(userList, tmp);
    }
    fclose(User);
    userList->current = NULL;
    return userList;
}

UserNode *userExistence(UserList *userList, char *name){
    for (UserNode *current = userList->head; current; current = current->nextNode) {
        if (!strcmp(current->name, name)) {
            return current;
        }
    }
    return NULL;
}

int readUser(UserList *userList, char *tmp) {
    UserNode *userNode = (UserNode *)malloc(sizeof(UserNode));
    char *str;

    userNode->nextNode = NULL;
    str = strtok(tmp, " ");
    strncpy(userNode->name, str, MAX_NAME);
    str = strtok(NULL, " ");
    userNode->id.UID = atoi(str);
    str = strtok(NULL, " ");
    userNode->id.GID = atoi(str);
    str = strtok(NULL, " ");
    userNode->date.year = atoi(str);
    str = strtok(NULL, " ");
    userNode->date.month = atoi(str);
    str = strtok(NULL, " ");
    userNode->date.weekday = atoi(str);
    str = strtok(NULL, " ");
    userNode->date.day = atoi(str);
    str = strtok(NULL, " ");
    userNode->date.hour = atoi(str);
    str = strtok(NULL, " ");
    userNode->date.minute = atoi(str);
    str = strtok(NULL, " ");
    userNode->date.second = atoi(str);
    str = strtok(NULL, " ");
    str[strlen(str) -1] = '\0';
    strncpy(userNode->dir, str, MAX_DIR);

    if (!strcmp(userNode->name, "root")) {
        userList->head = userNode;
        userList->tail = userNode;
    } else {
        userList->tail->nextNode = userNode;
        userList->tail = userNode;
    }
    return SUCCESS;
}
void userWrite(FILE *tempFile, UserList *userList, UserNode *userNode) {
    // 현재 시간을 가져옵니다.
    time(&ltime);
    today = localtime(&ltime);
    if (userList->current == userNode){
        // 현재 로그인한 사용자의 날짜 정보를 업데이트합니다.
        userNode->date.year = today->tm_year + 1900;
        userNode->date.month = today->tm_mon + 1;
        userNode->date.weekday = today->tm_wday;
        userNode->date.day = today->tm_mday;
        userNode->date.hour = today->tm_hour;
        userNode->date.minute = today->tm_min;
        userNode->date.second = today->tm_sec;

        // 임시 파일에 업데이트된 사용자 정보를 저장합니다.
        fprintf(tempFile, "%s %d %d %d %d %d %d %d %d %d %s\n", 
                userNode->name, userNode->id.UID, userNode->id.GID, 
                userNode->date.year, userNode->date.month, userNode->date.weekday, 
                userNode->date.day, userNode->date.hour, userNode->date.minute, 
                userNode->date.second, userNode->dir);
    } else {
        // 임시 파일에 기존 사용자 정보를 저장합니다.
        fprintf(tempFile, "%s %d %d %d %d %d %d %d %d %d %s\n", 
                userNode->name, userNode->id.UID, userNode->id.GID, 
                userNode->date.year, userNode->date.month, userNode->date.weekday, 
                userNode->date.day, userNode->date.hour, userNode->date.minute, 
                userNode->date.second, userNode->dir);
    }
}

void SaveUserList(UserList *userList) {
    FILE *tempFile = fopen("system/User_temp.txt", "w");
    if (tempFile == NULL) {
        printf("Error: Could not open temporary file for writing.\n");
        return;
    }

    // 기존 User.txt 파일을 읽기 위해 연다.
    FILE *originalFile = fopen("system/User.txt", "r");
    if (originalFile == NULL) {
        printf("Error: Could not open User.txt for reading.\n");
        fclose(tempFile);
        return;
    }

    char line[256];
    while (fgets(line, sizeof(line), originalFile) != NULL) {
        // 각 줄의 첫 번째 토큰(유저 이름)을 가져와서 현재 리스트에서 해당 유저를 찾는다.
        char *userName = strtok(line, " ");
        UserNode *userNode = userExistence(userList, userName);
        if (userNode) {
            userWrite(tempFile, userList, userNode);
        } else {
            // 현재 리스트에 없는 유저의 경우 그대로 임시 파일에 복사한다.
            fputs(line, tempFile);
        }
    }

    fclose(originalFile);
    fclose(tempFile);

    // 기존 User.txt 파일을 삭제하고 임시 파일을 User.txt로 변경한다.
    remove("system/User.txt");
    rename("system/User_temp.txt", "system/User.txt");
}


// system/User.txt 파일에서 유저 이름을 가져오는 함수
const char* getUserName(int uid) {
    UserNode *current = userList->head;
    while (current != NULL) {
        if (current->id.UID == uid) {
            return current->name;
        }
        current = current->nextNode;
    }
    return "unknown";
}

// system/User.txt 파일에서 그룹 이름을 가져오는 함수
const char* getGroupName(int gid) {
    UserNode *current = userList->head;
    while (current != NULL) {
        if (current->id.GID == gid) {
            return current->name;  // Assuming that the group name is the same as the user name
        }
        current = current->nextNode;
    }
    return "unknown";
}
