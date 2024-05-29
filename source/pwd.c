#include "../header/main.h"

// 현재 경로를 출력해야 하므로 DirectoryTree로 파일 시스템에 접근할 수 있어야 하고
// 현재 파일 경로까지 탐색해야 하므로 root부터 현재 위치까지 Stack에 넣은 후 다시 Pop한다.
// Stack을 사용하는 이유는 현재 경로를 root부터 출력하기 위함이다.
void inputStack(DirectoryTree *currentDirectory, DirectoryNode *currentNode, Stack *dirStack) {
    // 루트 디렉토리가 아닌 경우
    while (currentNode != NULL && currentNode->parent != currentDirectory->root) {
        // strdup 함수를 사용하여 currentNode->name의 복사본을 만들고 스택에 저장
        char *nameCopy = strdup(currentNode->name);
        if (nameCopy == NULL) {
            // 메모리 할당 실패 처리
            fprintf(stderr, "Memory allocation failed\n");
            exit(1);
        }
        Push(dirStack, nameCopy);
        currentNode = currentNode->parent;
    }
    if (currentNode != NULL) {
        char *nameCopy = strdup(currentNode->name);
        if (nameCopy == NULL) {
            // 메모리 할당 실패 처리
            fprintf(stderr, "Memory allocation failed\n");
            exit(1);
        }
        Push(dirStack, nameCopy);
    }
}

void popStack(Stack *dirStack){
    while (!IsEmpty(dirStack)) {
            char *name = Pop(dirStack);
            printf("/%s", name);
            free(name); // Pop으로 얻은 문자열의 메모리를 해제
    }
    printf("\n");
}

void printPath(DirectoryTree *currentDirectory, Stack *dirStack) {
    DirectoryNode *currentNode = currentDirectory->current;
    if (currentNode == currentDirectory->root) {
        // 루트 디렉토리인 경우
        printf("/\n");
    }
    else {
        inputStack(currentDirectory, currentNode, dirStack);
        popStack(dirStack);
    }
}

int pwd(DirectoryTree *currentDirectory, Stack *dirStack, char *option) {
    int dashCount=0;
    if (currentDirectory == NULL) {
        printf("The current directory information could not be found.\n");
        return ERROR;
    }
    if (option == NULL) {
        printPath(currentDirectory, dirStack);
    }
    else if (strcmp(option, "-") == 0 || strcmp(option, "--") == 0){
        printPath(currentDirectory, dirStack);
    }
    else if (strcmp(option, "--help") == 0){
        printf("pwd: pwd [-LP]\n");
        printf("  Print the name of the current working directory.\n\n");
        
        printf("  Options:\n");
        printf("    -L\t print the value of $PWD if it names the current working\n");
        printf("  \t directory\n");
        printf("    -P\t print the physical direcrtory, without any symbolic links\n\n");
        
        printf("  By default, 'pwd' behaves as if '-L' were specified.\n\n");
        
        printf("  Exit status:\n");
        printf("  Returns 0 unless an invalid option is given or the current directory\n");
        printf("  cannot be read.\n");
    }
    else if (strchr(option, '-') == NULL){
        printPath(currentDirectory, dirStack);
    }
    else{
        printf("-bash: pwd: %.2s: invalid option\n", option);
        printf("pwd: usage: pwd [-LP]\n");
        return ERROR;
    }
    return SUCCESS;
}