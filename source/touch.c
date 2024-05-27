#include "../header/main.h"


int file_num = 0;


int touch_option(DirectoryTree *currentDirectoryTree, char *cmd) {
    DirectoryNode *tmpNode = NULL;
    tmpNode = currentDirectoryTree->current;
    ThreadTree threadTree[MAX_BUFFER];
    pthread_t threadID[MAX_THREAD];
    pthread_mutex_t makes_mutex;
    pthread_mutex_init(&makes_mutex, NULL);
    
    int thread_n = 0;
    int option = 0;
    char *time;
    char *file;
    char *cmd_op;

    // touch 뒤에 입력이 없었을 경우
    if (cmd == NULL) {
        printf("touch: missing file operand\n");
        printf("Try 'touch --help' for more information.\n");
        return -1;
    }

    // 입력된 옵션 종류
    if (cmd[0] == '-') {
        cmd_op = strtok(cmd, " ");
        if (strcmp(cmd_op, "-a") == 0) {
            option = 1;
        }
        else if (strcmp(cmd_op, "-c") == 0) {
            option = 2;
        }
        else if (strcmp(cmd_op, "-t") == 0) {
            option = 3;
        }
        else if (strcmp(cmd_op, "-ac") == 0 || strcmp(cmd_op, "-ca") == 0) {
            option = 4;
        }
        else if (strcmp(cmd_op, "-ct") == 0 || strcmp(cmd_op, "-tc") == 0) {
            option = 5;
        }
        else if (strcmp(cmd_op, "-ta") == 0 || strcmp(cmd_op, "-at") == 0) {
            option = 6;
        }
        else if (strcmp(cmd_op, "-act") == 0 || strcmp(cmd_op, "-atc") == 0 || strcmp(cmd_op, "-cat") == 0 || strcmp(cmd_op, "-cta") == 0 || strcmp(cmd_op, "-tac") == 0 || strcmp(cmd_op, "-tca") == 0) {
            option = 7;
        }
        else if (strcmp(cmd_op, "--help") == 0) {
            printf("Usage: touch [OPTION]... [FILE]...\n");
            printf("Update the access and modification times of each FILE to the current time.\n\n");
            printf("A FILE argument that does not exist is created empty, unless -c\n");
            printf("is supplied.\n\n");
            printf("A FILE argument string of - is handled specially and causes touch to\n");
            printf("change the times of the file associated with standard output.\n\n");
            printf("Mandatory arguments to long options are mandatory for short options too.\n");
            printf("  -a                     change only the access time\n");
            printf("  -c, --no-create        do not create any files\n");
            printf("  -t STAMP               use [[CC]YY]MMDDhhmm[.ss] instead of current time\n");
            printf("      --time=WORD        change the specified time:\n");
            printf("                           WORD is access, atime, or use: equivalent to -a\n");
            printf("                           WORD is modify or mtime: equivalent to -m\n");
            printf("      --help     display this help and exit\n\n");
            printf("GNU coreutils online help: <https://www.gnu.org/software/coreutils/>\n");
            printf("Full documentation at: <https://www.gnu.org/software/coreutils/touch>\n");
            printf("or available locally via: info '(coreutils) touch invocation'\n\n");

            return -1;
        }
        else { // 옵션 입력이 틀렸을 경우
            printf("touch: invalid option -- '%s'\n", cmd_op);
            printf("Try 'touch --help' for more information.\n");
            return -1;
        }

        // 변경할 시간이 없거나 시간 형식이 틀렸을 경우
        if(option == 3 || option == 5 || option == 6 || option == 7) {
            time = strtok(NULL, " ");
            if (time == NULL) {
                printf("touch: option requires an argument -- 't'\n");
                printf("Try 'touch --help' for more information.\n");
                return -1;
            }
            else if (!is_valid_touch_time(time)) {
                printf("touch: invalid date format '%s'\n", time);
                return -1;
            }
        }
        else { 
            time = NULL;
        }

        // 대상 파일이 없을 경우
        file = strtok(NULL, " ");
        if (file == NULL) {
            printf("touch: missing file operand\n");
            printf("Try 'touch --help' for more information.\n");
            return -1;
        }
    }
    else { // 옵션이 없는 경우
        // 대상 파일이 없을 경우
        file = strtok(cmd, " ");
        if (file == NULL) {
            printf("touch: missing file operand\n");
            printf("Try 'touch --help' for more information.\n");
            return -1;
        }
    }

    while(file != NULL) {
        threadTree[thread_n].threadTree = currentDirectoryTree;
        threadTree[thread_n].option = option;
        threadTree[thread_n].content = time;
        threadTree[thread_n].fileName = file;
        threadTree[thread_n].mutex = &makes_mutex;
        file = strtok(NULL, " ");
        thread_n++;
        file_num++;
    }

    // pthread 생성
    for (int i = 0; i < thread_n; i++) {
        pthread_create(&threadID[i], NULL, touch_thread, (void*)&threadTree[i]);
    }

    // 각 스레드가 종료될 때까지 대기
    for (int i = 0; i < thread_n; i++) {
        pthread_join(threadID[i], NULL);
    }

    file_num = 0;

    return 1;
}

void *touch_thread(void* arg) {

    ThreadTree* threadTree = (ThreadTree*)arg;
    DirectoryTree *currentDirectoryTree = threadTree->threadTree;
    DirectoryNode *currentNode = currentDirectoryTree->current;
    DirectoryNode *check = NULL;
    DirectoryNode *parent;
    pthread_mutex_t *makes_mutex = threadTree->mutex;


    int option = threadTree->option;
    char* t = threadTree->content;
    char* file = threadTree->fileName;

    // 파일의 수정 시간 받아 옴
    struct stat file_stat;
    // 시간 구조체
    time_t file_time;
    // 현재 시간
    time_t current_time;
    // 시간 변경
    struct utimbuf new_times;

    // 파일 존재 유무 체크
    parent = findNodeToTouch(currentDirectoryTree, file); // 부모 노드
    
    if (parent==NULL){
        parent = findNodeByPath(currentDirectoryTree, file);
    }

    DirectoryNode* current = parent->firstChild;
    while (current != NULL) {
        if (strcmp(current->name, file) == 0) {
            check = current;
            break;
        }
        current = current->nextSibling;
    }

    if (check == NULL) {               // 파일 또는 디렉토리가 없을 경우
        char* pathCopy = strdup(file); // 원래 경로를 복사해서 사용
        char* lastSlash = strrchr(pathCopy, '/');
        if (lastSlash != NULL) {
            // 마지막 '/' 위치에서 문자열을 자른다
            *lastSlash = '\0';
        }

        // 생성하려는 파일 위치가 적합한지 확인
        if (parent == NULL) {
            printf("touch: cannot touch '%s': No such file or directory", file);
            return NULL;
        }

        if (option == 2 || option == 4 || option == 5 || option == 7) {
            return NULL;
        }

        if (HasPermission(parent, 'w') != 0) {
            printf("touch: '%s' Can not create file: Permission denied.\n", file);
            return NULL;
        }

        // 파일 이름
        char *f = malloc(strlen(file) + 1);;
        if (strchr(file, '/') != NULL) {
            char* lastSlash = strrchr(file, '/');
            strcpy(f, lastSlash+1);
        }
        else {
            strcpy(f, file);
        }
        // 파일 생성
        FILE* fp = fopen(file, "w");
        int fd = 0;
        if (fp == NULL) fd = -1;
        if (fd == -1) {
            printf("File cannot create");
            return NULL;
        }

        if (option == 3) { // -t
            // 시간 문자열을 구성하여 시간 구조체로 변환
            file_time = timeSetting(t);
            // 파일의 모든 시간을 현재 시간으로 변경
            utime(f, NULL);
            // 파일의 접근 시간과 수정 시간 변경
            new_times.actime = file_time;
            new_times.modtime = file_time;
            utime(f, &new_times);
        }

        if (option == 6) { // -ta, -at
            // 시간 문자열을 구성하여 시간 구조체로 변환
            file_time = timeSetting(t);
            stat(f, &file_stat);
            // 파일의 모든 시간을 현재 시간으로 변경
            utime(f, NULL);
            // 파일의 접근 시간과 수정 시간 변경
            new_times.actime = file_time;
            new_times.modtime = file_stat.st_mtime;
            utime(f, &new_times);
        }

        // 파일 정보 얻기
        struct stat fileStat;
        if (fstat(fd, &fileStat) == -1) {
            perror("fstat");
            close(fd);
            return NULL;
        }

        // 생성한 파일 DirectoryNode
        DirectoryNode *filenode = (DirectoryNode*)malloc(sizeof(DirectoryNode));
        // 파일 이름
        strcpy(filenode->name, f);
        // 파일 타입
        filenode->type = '-';
        if(filenode->name[0] != '.')
            filenode->viewType = 's';
        else
            filenode->viewType = ' ';
        // 파일 사이즈
        filenode->SIZE = fileStat.st_size;
        // 파일 권한
        filenode->permission.mode = 644;
        // 파일 아이디
        filenode->id.UID = userList->current->id.UID;
        filenode->id.GID = userList->current->id.GID;
        // 파일 수정 시간
        struct tm *modTime = localtime(&fileStat.st_mtime);
        if (modTime == NULL) {
            perror("localtime");
        }
        filenode->date.year = modTime->tm_year + 1900;
        filenode->date.month = modTime->tm_mon + 1;
        filenode->date.weekday = modTime->tm_wday;
        filenode->date.day = modTime->tm_mday;
        filenode->date.hour = modTime->tm_hour;
        filenode->date.minute = modTime->tm_min;
        filenode->date.second = modTime->tm_sec;
        // 부모 폴더
        filenode->parent = parent;
        // 자식 폴더 및 파일
        filenode->firstChild = NULL;
        // 형제 관계의 폴더 및 파일
        filenode->nextSibling = NULL;

        // 부모 폴더의 자식 폴더 및 파일 이어주기
        if (parent->firstChild == NULL) {
            parent->firstChild = filenode;
        } 
        else {
            DirectoryNode* current = parent->firstChild;
            while (current->nextSibling != NULL) {
                current = current->nextSibling;
            }
            current->nextSibling = filenode;
        }
        pthread_mutex_lock(makes_mutex);
        SaveDirectory(currentDirectoryTree, InitializeStack());
        pthread_mutex_unlock(makes_mutex);
    }
    else if (check != NULL && current->id.UID == userList->current->id.UID) { // 파일 또는 디렉토리의 메타데이터 수정이 가능한 경우 => 루트 계정도 가능
        switch (option)
        {
        case 0: // 옵션 x
        case 2: // -c
            // 파일의 모든 시간을 현재 시간으로 변경
            utime(file, NULL);
            break;
        case 1: // -a
        case 4: // -ac, -ca
            stat(file, &file_stat);
            // 현재 시간 가져오기
            current_time= time(NULL);
            struct tm *now = localtime(&current_time);
            // 파일의 모든 시간을 현재 시간으로 변경
            utime(file, NULL);
            // 파일의 접근 시간과 수정 시간 변경
            new_times.actime = mktime(now);
            new_times.modtime = file_stat.st_mtime;
            utime(file, &new_times);
            break;
        case 3: // -t
        case 5: // -ct, -tc
            // 시간 문자열을 구성하여 시간 구조체로 변환
            file_time = timeSetting(t);
            // 파일의 모든 시간을 현재 시간으로 변경
            utime(file, NULL);
            // 파일의 접근 시간과 수정 시간 변경
            new_times.actime = file_time;
            new_times.modtime = file_time;
            utime(file, &new_times);
            break;
        case 6: // -ta, -at
        case 7: // -act, -atc, -cat, -cta, -tac, -tca
            // 시간 문자열을 구성하여 시간 구조체로 변환
            file_time = timeSetting(t);
            stat(file, &file_stat);
            // 파일의 모든 시간을 현재 시간으로 변경
            utime(file, NULL);
            // 파일의 접근 시간과 수정 시간 변경
            new_times.actime = file_time;
            new_times.modtime = file_stat.st_mtime;
            utime(file, &new_times);
            break;
        default:
            printf("Invalid option\n");
            break;
        }

        int fd = open(file, O_RDONLY);
        if (fd == -1) {
            printf("Error: No such file or directory\n");
            return NULL;
        }

        // 파일 정보 얻기
        struct stat fileStat;
        if (fstat(fd, &fileStat) == -1) {
            perror("fstat");
            close(fd);
            return NULL;
        }

        // 파일 수정 시간 변경
        struct tm *modTime = localtime(&fileStat.st_mtime);
        if (modTime == NULL) {
            perror("localtime");
        }
        
        current->date.year = modTime->tm_year + 1900;
        current->date.month = modTime->tm_mon + 1;
        current->date.weekday = modTime->tm_wday;
        current->date.day = modTime->tm_mday;
        current->date.hour = modTime->tm_hour;
        current->date.minute = modTime->tm_min;
        current->date.second = modTime->tm_sec;

        // // Directory.txt 파일에 디렉토리 정보 기록
        char parentPath[MAX_DIR] = "";
        FILE *directoryFile = fopen("system/Directory.txt", "r+");
        if (directoryFile == NULL) {
            printf("Error: Cannot open Directory.txt for writing\n");
            return NULL;
        }
        close(fd);
        char line[1000];
        while (fgets(line, sizeof(line), directoryFile)) {
            if (strstr(line, current->name) && strstr(line, parent->name) != NULL) {
                // 해당 라인을 수정
                fseek(directoryFile, -strlen(line)-1, SEEK_CUR);
                getParentPath(currentDirectoryTree, current->parent, parentPath);
                fprintf(directoryFile, "%s %c %d %d %d %d %d %d %d %d %s     \n", current->name, current->type, current->permission.mode,
                        current->SIZE, current->id.UID, current->id.GID, current->date.month, current->date.day,
                        current->date.hour, current->date.minute, parentPath);
                fflush(directoryFile); // 파일 버퍼를 비워줍니다.
                break;
            }
        }

        pthread_mutex_lock(makes_mutex);
        SaveDirectory(currentDirectoryTree, InitializeStack());
        pthread_mutex_unlock(makes_mutex);
        fclose(directoryFile);
    }
    else { // 파일 또는 디렉토리의 쓰기 권한이 없을 경우
        printf("touch: cannot touch '%s': Permission denied\n", file);
        return NULL;
    }

    char temp[MAX_NAME] = "";
    getPath(currentDirectoryTree, parent, InitializeStack(), temp);
    // extractDirectoryPath(temp);
    linuxFileSystem = loadDirectory();
    movePath(linuxFileSystem, temp);
}


DirectoryNode* findNodeToTouch(DirectoryTree* tree, const char* path) {
    if (tree == NULL || tree->root == NULL || path == NULL) {
        return NULL;
    }

    // 현재 디렉토리에서 생성하는 경우 현재 노드를 반환
    if (strchr(path, '/') == NULL) {
        return tree->current;
    }

    // 시작 노드
    DirectoryNode* currentNode;

    // 상대 경로인 경우 currentNode는 그대로 사용

    // path를 '/'로 토큰화하여 각 디렉토리 수준으로 이동
    char* pathCopy = strdup(path); // 원래 경로를 복사해서 사용
    char* lastSlash = strrchr(pathCopy, '/');
    if (lastSlash != NULL) {
        // 마지막 '/' 위치에서 문자열을 자른다
        *lastSlash = '\0';
    }

    char* token = strtok(pathCopy, "/");
    if (token[0] == '~') { // 절대 경로
        currentNode = tree->root;
        while (token != NULL && currentNode != NULL) {
            DirectoryNode* child = currentNode->firstChild;
            currentNode = NULL; // 다음 단계의 디렉토리를 초기화

            // 다음 토큰으로 이동
            token = strtok(NULL, "/");
            // 자식 노드들 중에서 이름이 일치하는 노드를 찾음
            while (child != NULL) {
                if (strcmp(child->name, token) == 0) {
                    currentNode = child;
                    break;
                }
                child = child->nextSibling;
            }
        }

        if (token == NULL) {
            free(pathCopy); // 복사한 경로 메모리 해제
            return currentNode; // 최종적으로 발견된 노드를 반환 (없으면 NULL)
        }
        else { // 잘못된 경로일 경우
            free(pathCopy);
            return NULL;
        }
    }
    else if (strcmp(token, "..") == 0) { // 상대 경로
        currentNode = tree->current;
        while (token != NULL && currentNode != NULL) {
            if(strcmp(token, "..") == 0) {
                currentNode = tree->current->parent;
                // 다음 토큰으로 이동
                token = strtok(NULL, "/");
            }
            else {
                DirectoryNode* child = currentNode->firstChild;
                currentNode = NULL; // 다음 단계의 디렉토리를 초기화
                // 자식 노드들 중에서 이름이 일치하는 노드를 찾음
                while (child != NULL) {
                    if (strcmp(child->name, token) == 0) {
                        currentNode = child;
                        break;
                    }
                    child = child->nextSibling;
                }
            }
            // 다음 토큰으로 이동
            token = strtok(NULL, "/");

            if (token == NULL) {
                free(pathCopy); // 복사한 경로 메모리 해제
                return currentNode; // 최종적으로 발견된 노드를 반환 (없으면 NULL)
            }
            else { // 잘못된 경로일 경우
                free(pathCopy);
                return NULL;
            }
        }

        if (token == NULL) {
            free(pathCopy); // 복사한 경로 메모리 해제
            return currentNode; // 최종적으로 발견된 노드를 반환 (없으면 NULL)
        }
        else { // 잘못된 경로일 경우
            free(pathCopy);
            return NULL;
        }

    }
    else { // 현재 디렉토리 안에서 생성
        currentNode = tree->current;
        while (token != NULL && currentNode != NULL) {
            DirectoryNode* child = currentNode->firstChild;
            currentNode = NULL; // 다음 단계의 디렉토리를 초기화

            // 다음 토큰으로 이동
            token = strtok(NULL, "/");
            // 자식 노드들 중에서 이름이 일치하는 노드를 찾음
            while (child != NULL) {
                if (strcmp(child->name, token) == 0) {
                    currentNode = child;
                    break;
                }
                child = child->nextSibling;
            }
        }

        if (token == NULL) {
            free(pathCopy); // 복사한 경로 메모리 해제
            return currentNode; // 최종적으로 발견된 노드를 반환 (없으면 NULL)
        }
        else { // 잘못된 경로일 경우
            free(pathCopy);
            return NULL;
        }
    }
}
