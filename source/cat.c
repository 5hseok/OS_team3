#include "../header/main.h"

void cat(DirectoryTree *currentDirectoryTree, char *cmd) {
    int file_num = 0;
    ThreadTree threadTree[MAX_THREAD];
    pthread_t threadID[MAX_THREAD];
    int thread_n = 0;
    int option = 0;
    char *file = NULL;
    char *cmd_op = NULL;
    
    if (cmd[0] == '-') {
        cmd_op = strtok(cmd, " ");
        if (strcmp(cmd_op, "-n") == 0) {
            option = 1;
        }
        else if (strcmp(cmd_op, "--help") == 0) {
            printf("Manual: cat [<Option>]... [<File>]...\n");
            printf(" FILES are combined and sent to standard output.\n\n");
            printf("  Options:\n");
            printf("    -n, --number         \t number all output lines\n");
            printf("    >,                  \t redirect output to a file\n");
            printf("    --help              \t Display this help and exit\n");
            return;
        }
        else {
            printf("cat: invalid option -- '%s'\n", cmd_op);
            printf("Try 'cat --help' for more information.\n");
            return;
        }
        
        file = strtok(NULL, " ");
        if (file == NULL) {
            printf("cat: missing file operand\n");
            printf("Try 'cat --help' for more information.\n");
            return;
        }
    }
    else if (cmd[0] == '>') {
            option = 2;
            file = cmd + 2;
            file = strtok(file, " ");
        }
    else 
    {
        if (cmd == NULL) {
            printf("cat: missing file operand\n");
            printf("Try 'cat --help' for more information.\n");
            return;
        }

        file = strtok(cmd, " ");
        if (file == NULL) {
            printf("cat: missing file operand\n");
            printf("Try 'cat --help' for more information.\n");
            return;
        }
    }

    while (file != NULL && thread_n < MAX_THREAD) {
        threadTree[thread_n].option = option;
        threadTree[thread_n].fileName = strdup(file);  // Make a copy of the file name
        threadTree[thread_n].threadTree = currentDirectoryTree;
        file = strtok(NULL, " ");
        thread_n++;
        file_num++;
    }

    for (int i = 0; i < thread_n; i++) {
        pthread_create(&threadID[i], NULL, cat_thread, (void*)&threadTree[i]);
    }

    for (int i = 0; i < thread_n; i++) {
        pthread_join(threadID[i], NULL);
    }

    file_num = 0;
}

void* cat_thread(void* arg) {
    ThreadTree* threadTree = (ThreadTree*)arg;

    DirectoryTree *dtree = threadTree->threadTree;
    int option = threadTree->option;
    char* file1 = threadTree->fileName;
    FILE *file;
    char line[256];
    int lineNumber = 1;
    DirectoryNode *newNode = NULL;

    if (option == 2) {
        FILE *redirectFile = fopen(file1, "w");
        if (redirectFile == NULL) {
            printf("Error: Cannot open file '%s' for writing\n", file1);
            return NULL;
        }

        printf("Enter text (Press Ctrl+D to save and exit):\n");
        while (fgets(line, sizeof(line), stdin)) {
            fputs(line, redirectFile);
        }

        fclose(redirectFile);
        clearerr(stdin);

        // Allocate memory for the new node
        newNode = (DirectoryNode*) malloc(sizeof(DirectoryNode));
        if (newNode == NULL) {
            printf("Error: Memory allocation failed\n");
            return NULL;
        }
        strcpy(newNode->name, file1);
        newNode->type = '-';
        if (newNode -> name[0] !='.')
            newNode->viewType = 's';
        else 
            newNode->viewType = ' ';
        newNode->SIZE = 0; // 초기 크기는 0으로 설정
        newNode->id.UID = userList->current->id.UID;
        newNode->id.GID = userList->current->id.GID;
        time_t now = time(NULL);
        struct tm *t = localtime(&now);
        newNode->date.year = t->tm_year + 1900;
        newNode->date.month = t->tm_mon + 1;
        newNode->date.day = t->tm_mday;
        newNode->date.hour = t->tm_hour;
        newNode->date.minute = t->tm_min;
        newNode->date.second = t->tm_sec;
        newNode->permission.mode = 0664; // 파일 권한 설정
        modeToPermission(newNode);
        newNode->parent = dtree->current;
        newNode->nextSibling = dtree->current->firstChild;

        if(dtree->current->firstChild == NULL){
            dtree->current->firstChild = newNode;
        }
        else{
            DirectoryNode *tempNode = dtree->current->firstChild;
            while (tempNode->nextSibling) {
                tempNode = tempNode->nextSibling;
            }
            tempNode->nextSibling = newNode;
        }

        char tempPath[MAX_DIR] = "";
        Stack* dirStack = InitializeStack();
        getPath(dtree, newNode, dirStack, tempPath);

        extractDirectoryPath(tempPath);

        FILE *tempFile = fopen("system/Directory_temp.txt", "w");
        FILE *directoryFile = fopen("system/Directory.txt", "r");
        if (directoryFile == NULL) {
            printf("Error: Cannot open Directory.txt for reading\n");
            return NULL;
        }
        if (tempFile == NULL) {
            printf("Error: Cannot open Directory_temp.txt for writing\n");
            return NULL;
        }

        // 기존 파일 내용 복사 및 동일한 노드 삭제
        char fileLine[256];
        char fileName[MAX_NAME], filePath[MAX_DIR], fileType;
        int fileMode, fileSize, fileUID, fileGID, fileMonth, fileDay, fileHour, fileMinute;

        while (fgets(fileLine, sizeof(fileLine), directoryFile)) {
            sscanf(fileLine, "%s %c %d %d %d %d %d %d %d %d %s\n", fileName, &fileType, &fileMode, &fileSize, &fileUID, &fileGID, &fileMonth, &fileDay, &fileHour, &fileMinute, filePath);
            if (!(strcmp(fileName, newNode->name) == 0 && fileType == newNode->type && strcmp(filePath, tempPath) == 0)) {
                fputs(fileLine, tempFile);
            }
        }
        fclose(directoryFile);

        fprintf(tempFile, "%s %c %o %d %d %d %d %d %d %d %s\n", newNode->name, newNode->type, newNode->permission.mode,
                newNode->SIZE, newNode->id.UID, newNode->id.GID, newNode->date.month, newNode->date.day,
                newNode->date.hour, newNode->date.minute, tempPath);

        fclose(tempFile);
        remove("system/Directory.txt");
        rename("system/Directory_temp.txt", "system/Directory.txt");
        linuxFileSystem = loadDirectory();
        movePath(linuxFileSystem, tempPath);
    } 
    else {
        file = fopen(file1, "r");
        if (file == NULL) {
            printf("Error: Cannot open file '%s'\n", file1);
            return NULL;
        }

        if (option == 1) {
            while (fgets(line, sizeof(line), file)) {
                printf("    %d  %s", lineNumber, line);
                lineNumber++;
            }
        } else {
            while (fgets(line, sizeof(line), file)) {
                printf("%s", line);
            }
        }

        fclose(file);
    }

    return NULL;
}