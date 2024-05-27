#include "../header/main.h"



void chmod_print(int chmodinfo)
{
    int temp;
    int div = 100;
    int divideinfo;

    for (int i = 0; i < 3; i++) // 권한 정보 3개의 정보로 나눠짐 -> 소유자, 그룹, 다른 사용자 권한
    {
        // 한 자리수씩 나눠서 문자로 변경
        divideinfo = chmodinfo / div;
        chmodinfo = chmodinfo - divideinfo * div;
        div = div / 10;

        if (divideinfo == 0)
        {
            printf("---");
        }
        else if (divideinfo == 1)
        {
            printf("--x");
        }
        else if (divideinfo == 2)
        {
            printf("-w-");
        }
        else if (divideinfo == 3)
        {
            printf("-wx");
        }
        else if (divideinfo == 4)
        {
            printf("r--");
        }
        else if (divideinfo == 5)
        {
            printf("r-x");
        }
        else if (divideinfo == 6)
        {
            printf("rw-");
        }
        else if (divideinfo == 7)
        {
            printf("rwx");
        }
    }
    printf(" ");
}

int treePreOrder(DirectoryNode* directoryNode, int nodeNum)
{
	if (directoryNode != NULL)
	{
		nodeNum++;
		if (directoryNode->nextSibling == NULL)
		{
			nodeNum = treePreOrder(directoryNode->firstChild, nodeNum);
			nodeNum = treePreOrder(directoryNode->nextSibling, nodeNum);
		}
		else
		{
			DirectoryNode* temp = directoryNode->nextSibling;
			while (temp != NULL)
			{
				nodeNum++;
				temp = temp->nextSibling;
			}
			nodeNum = treePreOrder(directoryNode->firstChild, nodeNum);
			nodeNum = treePreOrder(directoryNode->nextSibling, nodeNum);
		}
	}
	return nodeNum;
}

int directoryLinkPrint(DirectoryNode* directoryNode)
{
	int nodeNum = 0;
	nodeNum = treePreOrder(directoryNode, 0);

	if (directoryNode->nextSibling != NULL)
	{
		DirectoryNode* temp = directoryNode->nextSibling;
		while (temp != NULL)
		{
			nodeNum--;
			if(temp->firstChild!=NULL)
			{
				DirectoryNode* newtemp = temp->nextSibling;
				while(newtemp!=NULL)
				{
					nodeNum--;
					newtemp=newtemp->firstChild;
				}
			}
			temp = temp->nextSibling;
		}
	}
	printf("%d ", nodeNum);
}


void ls(DirectoryTree* currentDirectoryTree, char* option) {
    DirectoryNode* currentNode = currentDirectoryTree->current;
    DirectoryNode* temp;
    DirectoryNode* directory_list[MAX_DIR];
    int directory_num = 0;

    if (currentNode->firstChild == NULL) {
        return;
    } else {
        temp = currentNode->firstChild->nextSibling;
        if (temp == NULL) {
            directory_list[directory_num++] = currentNode->firstChild;
        } else {
            directory_list[directory_num++] = currentNode->firstChild;
            while (temp != NULL) {
                directory_list[directory_num++] = temp;
                temp = temp->nextSibling;
            }
        }
    }

    if (option == NULL) {
        int num = 0;
        while (num < directory_num) {
            if (directory_list[num]->viewType == 's') {
                if (directory_list[num]->type == 'd')  BLUE;
                printf("%s ", directory_list[num]->name);
            }
            DEFAULT;
            num++;
        }
        printf("\n");
    }
    else if (!strcmp(option, "--help")) {
        printf("Usage: ls [OPTION]... [FILE]...\n");
        printf("List information about the FILEs (the current directory by default).\n");
        printf("Sort entries alphabetically if none of -cftuvSUX nor --sort is specified.\n");
        printf("Mandatory arguments to long options are mandatory for short options too.\n");
        printf("  -a, --all                  do not ignore entries starting with .\n");
        printf("  -l                         use a long listing format\n");
        printf("      --help     display this help and exit\n");
    }
    else if (!strcmp(option, "-l")) {
        int num = 0;
        int maxLinks = 0, maxUser = 0, maxGroup = 0, maxSize = 0, maxDay = 0;
        printf("total %d\n", directory_num);

        while (num < directory_num) {
            if (directory_list[num]->viewType == 's') {
                int linkCount = snprintf(NULL, 0, "%d", countChildren(directory_list[num]));
                if (linkCount > maxLinks) maxLinks = linkCount;

                int userLen = strlen(getUserName(directory_list[num]->id.UID));
                if (userLen > maxUser) maxUser = userLen;

                int groupLen = strlen(getGroupName(directory_list[num]->id.GID));
                if (groupLen > maxGroup) maxGroup = groupLen;

                int sizeLen = snprintf(NULL, 0, "%d", directory_list[num]->SIZE);
                if (sizeLen > maxSize) maxSize = sizeLen;

                int dayLen = snprintf(NULL, 0, "%d", directory_list[num]->date.day);
                if (dayLen > maxDay) maxDay = dayLen;
            }
            num++;
        }

        num = 0;
        while (num < directory_num) {
            if (directory_list[num]->type == 'd') {
                printf("d");
                chmod_print(directory_list[num]->permission.mode);
                printf("%*d ", maxLinks, 2 + countChildren(directory_list[num]));
                printf("%-*s ", maxUser, getUserName(directory_list[num]->id.UID));
                printf("%-*s ", maxGroup, getGroupName(directory_list[num]->id.GID));
                printf("%*d ", maxSize, directory_list[num]->SIZE);
                getMonth(directory_list[num]->date.month);
                printf("%*d %02d:%02d ", maxDay, directory_list[num]->date.day, directory_list[num]->date.hour, directory_list[num]->date.minute);
                BLUE;
                printf("%s\n", directory_list[num]->name);
                DEFAULT;
            }
            else if (directory_list[num]->type == '-') {
                printf("-");
                chmod_print(directory_list[num]->permission.mode);
                printf("%*d ", maxLinks, 1);
                printf("%-*s ", maxUser, getUserName(directory_list[num]->id.UID));
                printf("%-*s ", maxGroup, getGroupName(directory_list[num]->id.GID));
                printf("%*d ", maxSize, directory_list[num]->SIZE);
                getMonth(directory_list[num]->date.month);
                printf("%*d %02d:%02d ", maxDay, directory_list[num]->date.day, directory_list[num]->date.hour, directory_list[num]->date.minute);
                printf("%s\n", directory_list[num]->name);
            }
            num++;
        }
        printf("\n");
    }
    else if (!strcmp(option, "-a")) {
        int num = 0;
        while (num < directory_num) {
            if(directory_list[num]->type =='d') BLUE;
            printf("%s ", directory_list[num]->name);
            DEFAULT;
            num++;
        }
        printf("\n");
    }
    else if (!strcmp(option, "-al") || !strcmp(option, "-la")) {
        int num = 0;
        int maxLinks = 0, maxUser = 0, maxGroup = 0, maxSize = 0, maxDay = 0;
        printf("total %d\n", directory_num);

        while (num < directory_num) {
            int linkCount = snprintf(NULL, 0, "%d", countChildren(directory_list[num]));
            if (linkCount > maxLinks) maxLinks = linkCount;

            int userLen = strlen(getUserName(directory_list[num]->id.UID));
            if (userLen > maxUser) maxUser = userLen;

            int groupLen = strlen(getGroupName(directory_list[num]->id.GID));
            if (groupLen > maxGroup) maxGroup = groupLen;

            int sizeLen = snprintf(NULL, 0, "%d", directory_list[num]->SIZE);
            if (sizeLen > maxSize) maxSize = sizeLen;

            int dayLen = snprintf(NULL, 0, "%d", directory_list[num]->date.day);
                if (dayLen > maxDay) maxDay = dayLen;

            num++;
        }

        num = 0;
        while (num < directory_num) {
            if (directory_list[num]->type == 'd') {
                printf("d");
                chmod_print(directory_list[num]->permission.mode);
                printf("%*d ", maxLinks, 2 + countChildren(directory_list[num]));
                printf("%-*s ", maxUser, getUserName(directory_list[num]->id.UID));
                printf("%-*s ", maxGroup, getGroupName(directory_list[num]->id.GID));
                printf("%*d ", maxSize, directory_list[num]->SIZE);
                getMonth(directory_list[num]->date.month);
                printf("%*d %02d:%02d ", maxDay, directory_list[num]->date.day, directory_list[num]->date.hour, directory_list[num]->date.minute);
                BLUE;
                printf("%s\n", directory_list[num]->name);
                DEFAULT;
            }
            else if (directory_list[num]->type == '-') {
                printf("-");
                chmod_print(directory_list[num]->permission.mode);
                printf("%*d ", maxLinks, 1);
                printf("%-*s ", maxUser, getUserName(directory_list[num]->id.UID));
                printf("%-*s ", maxGroup, getGroupName(directory_list[num]->id.GID));
                printf("%*d ", maxSize, directory_list[num]->SIZE);
                getMonth(directory_list[num]->date.month);
                printf("%*d %02d:%02d ", maxDay, directory_list[num]->date.day, directory_list[num]->date.hour, directory_list[num]->date.minute);
                printf("%s\n", directory_list[num]->name);
            }
            num++;
        }
        printf("\n");
    }
    else {
        printf("ls: invalid option -- %s\n", option);
        printf("Try 'ls --help' for more information.\n");
    }
}