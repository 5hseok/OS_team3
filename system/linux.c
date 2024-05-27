#include "../header/main.h"


//로그인, 로그아웃, 프롬프트 메세지 등을 출력하는 코드들 
void login(UserList *userList, DirectoryTree *currentDirectoryTree) {
    UserNode *tempUser = NULL;
    char userName[MAX_NAME];
    char temp[MAX_DIR];
    tempUser = userList->head;
    printf("Users: ");
    
    while (tempUser) {
        printf("%s ", tempUser->name);
        tempUser = tempUser->nextNode;
    }

    printf("\n");

    while (1) {
        printf("Login as: ");
        fgets(userName, sizeof(userName), stdin);
        userName[strlen(userName) - 1] = '\0';
        if (!strcmp(userName, "exit")) {
            exit(0);
        }
        tempUser = userExistence(userList, userName);
        if (tempUser) {
            userList->current = tempUser;
            currentUser = tempUser;
            break;
        }
        printf("'%s' User not found\n", userName);
    }

    // If the user is not root, set the user's home directory as current
    if (strcmp(userName, "root") != 0) {
        strcpy(temp, userList->current->dir);
        movePath(currentDirectoryTree, temp);
    }
}


void printPrompt(DirectoryTree *currentDirectoryTree, Stack *dirStack) {
    DirectoryNode *tempNode = NULL;
    char temp[MAX_DIR] = "";
    char homeDir[MAX_DIR];
    char *printCursor;

    if (userList->current == userList->head) {
        printCursor = "#"; // root로 로그인 했을 때
        DEFAULT;
        printf("%s@OS_01_team3", userList->current->name);
        printf(":");
    } else {
        printCursor = "$"; // 그 외 user로 로그인 했을 때
        BOLD; GREEN;
        printf("%s@OS_01_team3", userList->current->name);
        DEFAULT;
        printf(":");
    }

    tempNode = currentDirectoryTree->current;
    strncpy(homeDir, userList->current->dir, MAX_DIR); // 홈 디렉토리 경로 복사

    // 현재 디렉토리 경로 구성
    if (tempNode == currentDirectoryTree->root) {
        strcpy(temp, "/");
    } else {
        while (tempNode->parent) {
            Push(dirStack, tempNode->name);
            tempNode = tempNode->parent;
        }
        while (!IsEmpty(dirStack)) {
            strcat(temp, "/");
            strcat(temp, Pop(dirStack));
        }
    }

    // 홈 디렉토리 경로와 비교하여 경로를 '~'로 대체
    trim(temp);
    trim(homeDir);
    BOLD;
    if (strcmp(homeDir, "/") != 0) {            //root 유저가 아니라면 
        BLUE;
        if (strncmp(temp, homeDir, strlen(homeDir)) == 0) {
            printf("~%s", temp + strlen(homeDir));
        } else {
            printf("%s", temp);
        }
    } else {                                       //root 유저라면
        printf("%s", temp);
    }

    DEFAULT;
    printf("%c ", *printCursor);
}



void Start(){
    Date tmpDate;
    getToday(&tmpDate);
    printf("Welcome to OS_01_team3 LINUX OS!!!\n\n");
    printf("System information as of ");
    getWeekday(tmpDate.weekday);
    getMonth(tmpDate.month);
    printf("%d %02d:%02d:%02d UTC %d\n\n", tmpDate.day, tmpDate.hour, tmpDate.minute, tmpDate.second, tmpDate.year);
    printf("You can use the command displayed below.\n");
    printf("\t- ls\n\t- cat\n\t- cd\n\t- mkdir\n\t- touch\n\t- mv\n\t- chmod\n\t- more\n\t- cp\n\t- grep\n\t- pwd\n\n");
    printf("The 'exit' command allows you to exit the program.\n\n");
    printf("Last login: ");
    getWeekday(userList->current->date.weekday);
    getMonth(userList->current->date.month);
    printf("%d %02d:%02d:%02d %d\n", userList->current->date.day, userList->current->date.hour, userList->current->date.minute, userList->current->date.second, userList->current->date.year);
}