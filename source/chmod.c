#include "../header/main.h"
DirectoryNode* find_node(DirectoryTree* currentDirectoryTree, char* name)  //현재 위치에서의 노드를 찾음.
{
	DirectoryNode* temp = currentDirectoryTree->current->firstChild;	// current node에 저장되어 있는 디렉토리

	while (temp != NULL)
	{
		if (strcmp(temp->name, name) == 0)		// 같은 이름의 디렉토리가 존재하는 경우
		{
			break;
		}
		temp = temp->nextSibling;
	}
	return temp;
}

void clear_permissions(Permission* change_mod, int user, int group, int others) {
    for (int i = 0; i < 3; ++i) {
        if (user) change_mod->permission[i] = 0;
        if (group) change_mod->permission[i + 3] = 0;
        if (others) change_mod->permission[i + 6] = 0;
    }
}

void update_mode(Permission* change_mod) {
    int mode = 0;

    for (int i = 0; i < 9; i += 3) {
        int octal_digit = (change_mod->permission[i] << 2) |
                          (change_mod->permission[i + 1] << 1) |
                          (change_mod->permission[i + 2]);
        mode = (mode * 10) + octal_digit; // mode를 8진수로 처리하지 않고 10진수로 처리
    }

    change_mod->mode = mode;
}


void apply_relative_mode(DirectoryNode* currentNode, Permission* change_mod, const char* permissionInfoStr) {
    int len = strlen(permissionInfoStr);
    int op = 0; // 1 for '+', -1 for '-', 0 for '='
    int user = 0, group = 0, others = 0; // To keep track of which user types to change
    for (int i = 0; i < 9; i++) {
        change_mod->permission[i] = currentNode->permission.permission[i];
    }
    for (int i = 0; i < len; i++) {
        printf("case : %c\n", permissionInfoStr[i]);
        switch (permissionInfoStr[i]) {
            case 'u': user = 1; break;
            case 'g': group = 1; break;
            case 'o': others = 1; break;
            case '+': op = 1; break;
            case '-': op = -1; break;
            case '=': op = 0; clear_permissions(change_mod, user, group, others); break;
            case 'r':
                if (user) change_mod->permission[0] = op >= 0 ? 1 : 0;
                if (group) change_mod->permission[3] = op >= 0 ? 1 : 0;
                if (others) change_mod->permission[6] = op >= 0 ? 1 : 0;
                break;
            case 'w':
                if (user) change_mod->permission[1] = op >= 0 ? 1 : 0;
                if (group) change_mod->permission[4] = op >= 0 ? 1 : 0;
                if (others) change_mod->permission[7] = op >= 0 ? 1 : 0;
                break;
            case 'x':
                if (user) change_mod->permission[2] = op >= 0 ? 1 : 0;
                if (group) change_mod->permission[5] = op >= 0 ? 1 : 0;
                if (others) change_mod->permission[8] = op >= 0 ? 1 : 0;
                break;
        }
    }
    
    // Update the mode value after applying the changes
    update_mode(change_mod);
}

void apply_absolute_mode(Permission* change_mod, const char* permissionInfoStr) {
    int mode = atoi(permissionInfoStr);
    change_mod->mode = mode;
    
    // Initialize all permissions to 0
    for (int i = 0; i < 9; i++) {
        change_mod->permission[i] = 0;
    }

    // 8진수를 2진수로 변환하여 permission 배열에 저장
    for (int i = 8; i >= 0; --i) {
        change_mod->permission[i] = mode % 2;
        mode /= 2;
    }
}

int parse_permission_info(DirectoryNode* currentNode, char* permissionInfo, Permission* change_mod) {
    if (strchr(permissionInfo, '+') != NULL || strchr(permissionInfo, '-') != NULL || strchr(permissionInfo, '=') != NULL) {
        apply_relative_mode(currentNode, change_mod, permissionInfo);
        currentNode->permission = *change_mod;

        return SUCCESS;
    } else if (permissionInfo[0] - '0' < 8 && permissionInfo[1] - '0' < 8 && permissionInfo[2] - '0' < 8 && strlen(permissionInfo) == 3) {
        apply_absolute_mode(change_mod, permissionInfo);
        currentNode->permission = *change_mod;

        return SUCCESS;
    }
    else {
        printf("chmod: Invalid Mode: \'%s\'\n", permissionInfo);
        printf("Try \'chmod --help\' for more information.\n");
        return ERROR;
    }
}

void ch_mod(DirectoryTree* currentDirectoryTree, char* permissionInfo, char* nodeName) {
    Permission* change_mod = (Permission*)malloc(sizeof(Permission));
    if (!strcmp(permissionInfo, "--help")){
        printf("Usage: chmod [OPTION]... OCTAL-MODE FILE...\n");
        printf("Change the mode of each FILE to MODE.\n\n");
        printf("  Options:\n");
        printf("  -R, --recursive        change files and directories recursively\n");
        printf("      --help     Display this help and exit\n");
        return;
    }
    DirectoryNode* temp = find_node(currentDirectoryTree, nodeName);
    if (temp == NULL) {
        printf("chmod: No such file or directory\n");
        return;
    }
    if (userList->current->id.UID == temp->id.UID ||
        userList->current->id.GID == temp->id.GID ||
        userList->current->name == "root") {
        
        if (parse_permission_info(temp, permissionInfo, change_mod) == 0) {
            // Directory.txt에 현재 노드의 상태 기록
            // Directory.txt 파일에서 일치하는 행을 찾아 수정된 권한 정보로 업데이트

            // 현재 노드의 경로를 가져오기
            char tempPath[MAX_DIR] = "";
            Stack* dirStack = InitializeStack();
            getPath(currentDirectoryTree, temp, dirStack, tempPath);
            free(dirStack); // Stack 메모리 해제

            // 임시 파일 생성
            FILE *tempFile = fopen("system/Directory_temp.txt", "w");
            FILE *directoryFile = fopen("system/Directory.txt", "r");
            if (directoryFile == NULL) {
                printf("Error: Cannot open Directory.txt for reading\n");
                return;
            }
            if (tempFile == NULL) {
                printf("Error: Cannot open Directory_temp.txt for writing\n");
                return;
            }

            // 기존 파일 내용 복사 및 동일한 노드 삭제
            char fileLine[256];
            char fileName[MAX_NAME], filePath[MAX_DIR], fileType;
            int fileMode, fileSize, fileUID, fileGID, fileMonth, fileDay, fileHour, fileMinute;

            extractDirectoryPath(tempPath);


            while (fgets(fileLine, sizeof(fileLine), directoryFile)) {
                sscanf(fileLine, "%s %c %d %d %d %d %d %d %d %d %s", fileName, &fileType, &fileMode, &fileSize, &fileUID, &fileGID, &fileMonth, &fileDay, &fileHour, &fileMinute, filePath);
                if (!(strcmp(fileName, temp->name) == 0 && fileType == temp->type && strcmp(filePath, tempPath) == 0)) {
                    fputs(fileLine, tempFile);
                }
            }
            fclose(directoryFile);

            // 새로운 권한 정보로 노드 추가
            fprintf(tempFile, "%s %c %d %d %d %d %d %d %d %d %s\n", temp->name, temp->type, temp->permission.mode,
                    temp->SIZE, temp->id.UID, temp->id.GID, temp->date.month, temp->date.day,
                    temp->date.hour, temp->date.minute, tempPath);

            fclose(tempFile);
            remove("system/Directory.txt");
            rename("system/Directory_temp.txt", "system/Directory.txt");
            linuxFileSystem = loadDirectory();
            movePath(linuxFileSystem, tempPath);
        } else {
            printf("chmod: Invalid Mode: \'%s\'\n", permissionInfo);
            printf("Try \'chmod --help\' for more information.\n");
        }

        free(change_mod);
        }
}