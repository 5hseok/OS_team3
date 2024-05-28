#include "../header/main.h"

int mv(DirectoryTree* currentDirectoryTree, char* cmd) {
    char* sourceName = strtok(cmd, " ");
    char* destPath = strtok(NULL, " ");
    if (!sourceName) {
        printf("mv: missing file operand\n");
        printf("Try 'mv --help' for more information.\n");
        return ERROR;
    }

    if (!strcmp(sourceName, "--help"))
    {
        printf("Usage: mv [OPTION]... SOURCE DIRECTORY\n");
        printf("Rename SOURCE to DEST, or move SOURCE to DIRECTORY.\n");
        printf("    --help    display this help and exit\n");
        return SUCCESS;
    }

    if (!destPath) {
        printf("mv: missing destination file operand after '%s'\n", sourceName);
        return ERROR;
    }

    DirectoryNode* sourceNode = findNodeInTree(currentDirectoryTree, sourceName);
    if (!sourceNode) {
        printf("mv: cannot stat '%s': No such file or directory\n", sourceName);
        return ERROR;
    }

    // Check write permissions for the source's parent directory
    if (HasPermission(sourceNode->parent, 'w') != 0) {
        printf("mv: cannot move '%s': Permission denied\n", sourceName);
        return ERROR;
    }

    DirectoryNode* newParentNode = NULL;
    char* newFileName = NULL;

    char* lastSlash = strrchr(destPath, '/');
    if (lastSlash) {
        // Separate directory path and new file name
        *lastSlash = '\0';
        newFileName = lastSlash + 1;
        newParentNode = findNodeByPath(currentDirectoryTree, destPath);
        *lastSlash = '/';  // Restore the original string
    } else {
        // No slash means the destination is just a new name in the same directory
        DirectoryNode* tempNode = IsExistDir(currentDirectoryTree, destPath, 'd');
        if (tempNode != NULL){
            newParentNode = tempNode;
            newFileName = sourceNode->name;
        }
        else{
            if(!strcmp(destPath, ".")){
                newParentNode = currentDirectoryTree->current;
                newFileName = sourceNode->name;
            }
            else if(!strcmp(destPath, "..")){
                newParentNode = currentDirectoryTree->current->parent;
                newFileName = sourceNode->name;
            }
            else{
                newParentNode = sourceNode->parent;
                newFileName = destPath;
            }
        }
    }

    if (!newParentNode) {
        printf("mv: cannot move to '%s': No such file or directory\n", destPath);
        return ERROR;
    }

    // If the new parent directory is not writable
    if (HasPermission(newParentNode, 'w') != 0) {
        printf("mv: cannot move to '%s': Permission denied\n", destPath);
        return ERROR;
    }

    // Remove sourceNode from its current location
    if (sourceNode->parent->firstChild == sourceNode) {
        sourceNode->parent->firstChild = sourceNode->nextSibling;
    } else {
        DirectoryNode* temp = sourceNode->parent->firstChild;
        while (temp->nextSibling != sourceNode) {
            temp = temp->nextSibling;
        }
        temp->nextSibling = sourceNode->nextSibling;
    }

    // Add sourceNode to the new parent directory
    sourceNode->parent = newParentNode;
    sourceNode->nextSibling = newParentNode->firstChild;
    newParentNode->firstChild = sourceNode;
    strncpy(sourceNode->name, newFileName, MAX_NAME);

    SaveDirectory(currentDirectoryTree, dirStack);
    return SUCCESS;
}