//디렉토리 구조를 가져오는 코드
#include "../header/main.h"


DirectoryNode* IsExistDir(DirectoryTree *currentDirectoryTree, char *dirName, char type) {
    trim(dirName);  //공백 문자나 개행 문자 제거
    for (DirectoryNode *current = currentDirectoryTree->current->firstChild; current; current = current->nextSibling) {
        trim(current->name);
        if (!strcmp(current->name, dirName) && current->type == type) {
            return current;
        }
    }
    return NULL;
}


void getParentPath(DirectoryTree* directoryTree, DirectoryNode* node, char* path)
{
    if (node == directoryTree->root) {
        strcpy(path, "/");
    } else {
        getPath1(directoryTree, node, InitializeStack(), path);
        size_t len = strlen(path);
        if (len > 0 && path[len - 1] == '/') {
            path[len - 1] = '\0';
        }
    }
}




void getPath1(DirectoryTree* directoryTree, DirectoryNode* node, Stack* stack, char* path)
{
    if (node == directoryTree->root) {
        strcat(path, "/");
    } else {
        getPath1(directoryTree, node->parent, stack, path);
        strcat(path, node->name);
        strcat(path, "/");
    }
}


void getPath(DirectoryTree *dirTree, DirectoryNode *dirNode, Stack *dirStack, char *temp) {
    DirectoryNode *tmpNode = dirNode->parent;

    if (tmpNode == dirTree->root) {
        strcpy(temp, "/");
    } else {
        while (tmpNode->parent) {
            Push(dirStack, tmpNode->name);
            tmpNode = tmpNode->parent;
        }
        while (!IsEmpty(dirStack)) {
            strcat(temp, "/");
            strcat(temp, Pop(dirStack));
        }
    }
    strcat(temp, "/");
    strcat(temp, dirNode->name);
}



// 새로운 findNodeInParent 함수
DirectoryNode* findNodeInParent(DirectoryNode* parentNode, const char* nodeName) {
    if (parentNode == NULL) {
        return NULL;
    }
    DirectoryNode* child = parentNode->firstChild;
    while (child != NULL) {
        if (strcmp(child->name, nodeName) == 0) {
            return child;
        }
        child = child->nextSibling;
    }
    return NULL;
}

int moveCurrent(DirectoryTree* currentDirectoryTree, char* dirPath) {

    DirectoryNode* tempNode = NULL;
    if(strcmp(dirPath,".") == 0) {
        // do nothing, remain in the current directory
    } 
    else if(strcmp(dirPath,"..") == 0) {
        if(currentDirectoryTree->current != currentDirectoryTree->root) {
            currentDirectoryTree->current = currentDirectoryTree->current->parent;
        }
    } 
    else if(strcmp(dirPath,"~") == 0) {
        // Handle home directory
        char homePath[MAX_DIR];
        strcpy(homePath, userList->current->dir);
        movePath(currentDirectoryTree, homePath);
    } 
    else {
        tempNode = IsExistDir(currentDirectoryTree, dirPath, 'd');
        if(tempNode != NULL) {
            currentDirectoryTree->current = tempNode;
        } else {
            return ERROR;
        }
    }
    return SUCCESS;
}


int movePath(DirectoryTree* currentDirectoryTree, char* dirPath)
{
    DirectoryNode* tempNode = NULL;
    char tempPath[MAX_DIR];
    char* str = NULL;
    int val = 0;
    strncpy(tempPath, dirPath, MAX_DIR);
    tempPath[MAX_DIR - 1] = '\0'; // 보안을 위해 널 종료 문자를 추가
    tempNode = currentDirectoryTree->current;

    //입력 값이 루트로 가는 값일 때
    //if input is root
    if(strcmp(dirPath, "/") == 0){
        currentDirectoryTree->current = currentDirectoryTree->root;
    }
    else{
        //if input is absolute path
        if(dirPath[0] == '/'){
            currentDirectoryTree->current = currentDirectoryTree->root; // Set current to root for absolute path
            // 이 부분을 수정하여 절대 경로를 올바르게 처리
            str = strtok(tempPath + 1, "/"); // +1 to skip the first '/'
        } else {
            // Relative path, start tokenizing from the beginning
            str = strtok(tempPath, "/");
        }
        while(str != NULL){
            val = moveCurrent(currentDirectoryTree, str);
            //if input path doesn't exist
            if(val != 0){
                currentDirectoryTree->current = tempNode; // Restore original current node
                return -1;
            }
            str = strtok(NULL, "/");
        }
    }
    return 0;
}

// 자식 노드의 수를 계산하는 함수
int countChildren(DirectoryNode* directoryNode) {
    int count = 0;
    DirectoryNode* temp = directoryNode->firstChild;
    while (temp != NULL) {
        count++;
        temp = temp->nextSibling;
    }
    return count;
}

void extractDirectoryPath(char *path) {
    char *lastSlash = strrchr(path, '/');
    if (lastSlash != NULL) {
        *lastSlash = '\0'; // 부모 경로로 만듭니다.
    }
}


DirectoryNode* findNodeByPath(DirectoryTree* tree, const char* path) {
    if (tree == NULL || tree->root == NULL || path == NULL) {
        return NULL;
    }


    char* tempPath = strdup(path);
    char* lastToken = NULL;
    DirectoryNode* tempNode = tree->current;
    tempPath = strtok(tempPath, "/");
    if (tempPath != NULL){
        tempNode = findNode(tempNode, tempPath);
    }
    while(tempPath != NULL) {
        lastToken = tempPath;
        tempPath = strtok(NULL, " ");
        if(tempPath == NULL) break;

        tempNode = findNode(tempNode, lastToken);
    } 
    
    if (tempNode != NULL) return tempNode;
        // 절대 경로가 '/'로 시작하는지 확인합니다.
    if (path[0] != '/') {
        return NULL;
    }
    // 루트부터 시작합니다.
    DirectoryNode* currentNode = tree->root;
    const char* token = strtok((char*)path + 1, "/"); // '/' 이후의 경로를 토큰화합니다.

    // 각 경로 토큰에 대해 디렉토리를 탐색합니다.
    while (token != NULL) {
        DirectoryNode* child = currentNode->firstChild;
        while (child != NULL) {
            if (strcmp(child->name, token) == 0) {
                currentNode = child;
                break;
            }
            child = child->nextSibling;
        }
        // 해당 토큰에 해당하는 디렉토리를 찾지 못한 경우 NULL을 반환합니다.
        if (child == NULL) {
            return NULL;
        }
        token = strtok(NULL, "/");
    }
    return currentNode;
}

// DirectoryTree 전체에서 특정 이름을 가진 Node를 찾는 함수입니다.
DirectoryNode* findNodeInTree(DirectoryTree* tree, const char* nodeName) {
    if (tree == NULL || tree->root == NULL) {
        return NULL;
    }
    if (nodeName[0] == '/') return findNodeByPath(tree, nodeName);

    return findNode(tree->current, nodeName);
}


// 특정 이름을 가진 Node를 찾는 함수입니다.
DirectoryNode* findNode(DirectoryNode* currentNode, const char* nodeName) {
    if (currentNode == NULL) {
        return NULL;
    }
    // 현재 Node가 찾고자 하는 Node인지 확인합니다.
    if (strcmp(currentNode->name, nodeName) == 0 || strcmp(nodeName, ".") == 0) {
        return currentNode;
    }
    if (strcmp(nodeName, "..") == 0)
        return currentNode->parent;
    // 자식 노드들에서 찾습니다.
    DirectoryNode* foundNode = findNode(currentNode->firstChild, nodeName);
    if (foundNode != NULL) {
        return foundNode;
    }
    // 형제 노드들에서 찾습니다.
    return findNode(currentNode->nextSibling, nodeName);
}


// 문자열 끝에 공백 문자나 새줄 문자가 있을 경우 제거하는 함수
void trim(char *str) {
    char *end;

    // 문자열 끝에 있는 공백 문자 제거
    end = str + strlen(str) - 1;
    while (end > str && (*end == '\n' || *end == ' ' || *end == '\t')) {
        end--;
    }

    // 새로운 끝을 null 문자로 설정
    *(end + 1) = '\0';
}
