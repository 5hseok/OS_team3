#include "../header/main.h"

DirectoryNode* copyNodeRecursive(DirectoryNode* source, DirectoryNode* parent) {
    if (!source) {
        return NULL;
    }

    DirectoryNode* newNode = (DirectoryNode*)malloc(sizeof(DirectoryNode));
    if (!newNode) {
        return NULL;
    }

    // 현재 노드 복사
    strcpy(newNode->name, source->name);
    newNode->type = source->type;
    newNode->viewType = source->viewType;
    newNode->SIZE = source->SIZE;
    newNode->permission = source->permission;
    newNode->id = source->id;
    newNode->date = source->date;
    newNode->parent = parent;
    newNode->firstChild = NULL;
    newNode->nextSibling = NULL;

    // 자식 노드 복사
    if (source->firstChild) {
        newNode->firstChild = copyNodeRecursive(source->firstChild, newNode);
    }

    // 형제 노드 복사
    if (source->nextSibling) {
        newNode->nextSibling = copyNodeRecursive(source->nextSibling, parent);
    }

    return newNode;
}

DirectoryNode* copyNode(DirectoryNode* source) {
    if (!source) {
        return NULL;
    }

    DirectoryNode* newNode = (DirectoryNode*)malloc(sizeof(DirectoryNode));
    if (!newNode) {
        return NULL;
    }

    // 현재 노드 복사
    strcpy(newNode->name, source->name);
    newNode->type = source->type;
    newNode->viewType = source->viewType;
    newNode->SIZE = source->SIZE;
    newNode->permission = source->permission;
    newNode->id = source->id;
    newNode->date = source->date;
    newNode->parent = NULL;
    newNode->firstChild = NULL;
    newNode->nextSibling = NULL;

    // 하위 노드들 복사
    if (source->firstChild) {
        newNode->firstChild = copyNodeRecursive(source->firstChild, newNode);
    }

    return newNode;
}

int cp(DirectoryTree* currentDirectoryTree, char* cmd) {
    char* sourceName = strtok(cmd, " ");
    char* destPath = strtok(NULL, " ");
    if (!sourceName) {
        printf("cp: missing file operand\n");
        printf("Try 'cp --help' for more information.\n");
        return ERROR;
    }
    if (!strcmp(sourceName, "--help"))
    {
        printf("Usage: cp [OPTION] ... SOURCE DIRECTORY\n");
        printf("Copy SOURCE to DEST.\n");
        printf("    --help    display this help and exit\n");
        return SUCCESS;
    }
    if (!destPath) {
        printf("cp: missing destination file operand after '%s'\n", sourceName);
        return ERROR;
    }

    DirectoryNode* sourceNode = findNodeInTree(currentDirectoryTree, sourceName);
    if (!sourceNode) {
        printf("cp: cannot stat '%s': No such file or directory\n", sourceName);
        return ERROR;
    }

    DirectoryNode* newParentNode = NULL;
    char* newFileName = NULL;

    char* lastSlash = strrchr(destPath, '/');
    if (lastSlash) {
        *lastSlash = '\0';
        newFileName = lastSlash + 1;
        newParentNode = findNodeByPath(currentDirectoryTree, destPath);
        *lastSlash = '/';  // Restore the original string
        if (!strcmp(newFileName,"")){
            newFileName = sourceNode->name;
        }
    } else {
        DirectoryNode* tempNode = findNodeInTree(currentDirectoryTree, destPath);
        if (tempNode != NULL && tempNode->type == 'd') {
            newParentNode = tempNode;
            newFileName = sourceNode->name;
        } else {
            if (!strcmp(destPath, ".")) {
                newParentNode = currentDirectoryTree->current;
                newFileName = sourceNode->name;
            } else if (!strcmp(destPath, "..")) {
                newParentNode = currentDirectoryTree->current->parent;
                newFileName = sourceNode->name;
            } else {
                newParentNode = currentDirectoryTree->current;
                newFileName = destPath;
            }
        }
    }

    if (!newParentNode) {
        printf("cp: cannot copy to '%s': No such file or directory\n", destPath);
        return ERROR;
    }

    if (HasPermission(newParentNode, 'w') != 0) {
        printf("cp: cannot copy to '%s': Permission denied\n", destPath);
        return ERROR;
    }

    DirectoryNode* existingNode = findNodeInParent(newParentNode, newFileName);
    if (existingNode) {
        if (existingNode->parent->firstChild == existingNode) {
            existingNode->parent->firstChild = existingNode->nextSibling;
        } else {
            DirectoryNode* temp = existingNode->parent->firstChild;
            while (temp->nextSibling != existingNode) {
                temp = temp->nextSibling;
            }
            temp->nextSibling = existingNode->nextSibling;
        }
        free(existingNode);
    }

    DirectoryNode* copiedNode = copyNode(sourceNode);
    if (!copiedNode) {
        printf("cp: memory allocation failed\n");
        return ERROR;
    }
    strcpy(copiedNode->name, newFileName);
    copiedNode->parent = newParentNode;

    copiedNode->nextSibling = newParentNode->firstChild;
    newParentNode->firstChild = copiedNode;
    
    SaveDirectory(currentDirectoryTree, dirStack);
    return SUCCESS;
}
