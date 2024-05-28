#ifndef MAIN_H
#define MAIN_H
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <errno.h>
#include <stdbool.h>
#include <pthread.h>
#include <utime.h>
#include <math.h>
#include <ctype.h>
#include <libgen.h> 

#ifdef _WIN32
#include <conio.h>
#else
#include <termios.h>
#endif

#define PAGELEN 24
#define LINELEN 512
#define DEFAULT printf("%c[%dm", 0x1B, 0)
#define BOLD printf("%c[%dm", 0x1B, 1)
#define WHITE printf("\x1b[37m")
#define BLUE printf("\x1B[1;34m")
#define GREEN printf("\x1B[32m")
#define MAGENTA printf("\x1B[35m")      //자주색
#define CYAN printf("\x1B[36m")         //청록색 
#define RED printf("\x1B[31m")
#define RESET "\x1B[0m"
#define MAX_NAME 256
#define MAX_DIR 1024
#define MAX_THREAD 20
#define MAX_BUFFER 1024
#define ERROR -1
#define SUCCESS 0
// 날짜 정보 - 파일이나 폴더의 수정 시간, 접근 시간에서 사용
typedef struct date {
    int year;
    int month;
    int weekday;
    int day;
    int hour;
    int minute;
    int second;
} Date;

// 사용자와 그룹 ID
typedef struct id {
    int UID;
    int GID;
} ID;

// 파일과 폴더 권한 
typedef struct permission {
    int mode; // 접근 모드 (ex 700, 400, 777 등)
    int permission[9]; // mode를 0 혹은 1의 값으로 풀어서 저장하는 배열 
} Permission;

// 사용자 노드 - 이름, 사용자의 홈 디렉토리, id, 
typedef struct userNode {
    char name[MAX_NAME];
    char dir[MAX_DIR];
    ID id;
    Date date;       
    struct userNode *nextNode; //다음 user를 가리키는 포인터로 user 탐색에 필요
} UserNode;

// UserList로 사용자들을 모아놓음 
typedef struct userList {
    ID topId;
    UserNode *head;
    UserNode *tail;
    UserNode *current;
} UserList;

// 파일이나 디렉토리의 inode 정보들
typedef struct directoryNode {
    char name[MAX_NAME];
    char type; // -는 파일, d는 디렉토리, l은 link파일
    char viewType;
    int SIZE;
    Permission permission;
    ID id;
	Date date;
	struct directoryNode *parent;       //부모 폴더
	struct directoryNode *firstChild;   //자식 폴더 및 파일
	struct directoryNode *nextSibling;  //형제 관계의 폴더 및 파일
} DirectoryNode;

// 파일 시스템의 구조 - DirectoryTree*로 파일 시스템에 접근
typedef struct directoryTree {
	DirectoryNode* root;
	DirectoryNode* current;
} DirectoryTree;

// stackNode
typedef struct stackNode {
	char name[MAX_NAME];
	struct stackNode *nextNode;
} StackNode;

// 경로 분석에 사용할 stack 정의
typedef struct stack {
	StackNode* topNode;
	int cnt;
} Stack;

// 멀티스레딩에서 사용할 threadTree - 파일 및 폴더 생성 단계에서 사용 변경 필요
typedef struct threadTree {
    DirectoryTree *threadTree;
    char *fileName;
    char *content;    //파일의 내용 저장
    char *command;    //사용자가 입력한 명령어 저장 
    char *usrName;    //파일이나 디렉토리의 소유자
    int mode;         //접근 권한
    int option;       //옵션 
    pthread_mutex_t *mutex; // 뮤텍스 포인터 추가
} ThreadTree;

//excuteCommand.c
void executeCommand(DirectoryTree *currentDirectoryTree, char *command);

//stack.c
Stack* InitializeStack();
int IsEmpty(Stack *s);
void Push(Stack *s, char *name);
char* Pop(Stack *s);

//directory.c
DirectoryTree* loadDirectory();
int readNode(DirectoryTree *currentDirectoryTree, char *temp);
void createAndAttachNode(DirectoryTree *currentDirectoryTree, char *str, DirectoryNode *newNode, DirectoryNode *tempNode);
void SaveDirectory(DirectoryTree *currentDirectoryTree, Stack* stackDir);
void nodeWrite(DirectoryTree *currentDirectoryTree, DirectoryNode* currentNode, Stack* stackDir);

//pwd.c
void inputStack(DirectoryTree *currentDirectory, DirectoryNode *currentNode, Stack *dirStack);
void popStack();
void printPath(DirectoryTree *currentDirectory, Stack *dirStack);
int pwd(DirectoryTree *currentDirectory, Stack *dirStack, char *option);

//cd.c
int cd(DirectoryTree *currentDirectoryTree, char *cmd);

//ls.c
void chmod_print(int chmodinfo);
int treePreOrder(DirectoryNode* directoryNode, int nodeNum);
int directoryLinkPrint(DirectoryNode* directoryNode);
void ls(DirectoryTree* currentDirectoryTree, char* option);

//chmod.c
DirectoryNode* find_node(DirectoryTree* currentDirectoryTree, char* name);
void clear_permissions(Permission* change_mod, int user, int group, int others);
void update_mode(Permission* change_mod);
void apply_absolute_mode(Permission* p, const char* modeStr);
void apply_relative_mode(DirectoryNode* currentNode, Permission* p, const char* modeStr);
void ch_mod(DirectoryTree* currentDirectoryTree, char* permissionInfo, char* nodeName);
int parse_permission_info(DirectoryNode* currentNode, char* permissionInfo, Permission* change_mod);

//utilize.c
DirectoryNode* IsExistDir(DirectoryTree *currentDirectoryTree, char *dirName, char type);
void getParentPath(DirectoryTree* currentDirectoryTree, DirectoryNode* temp, char* path);
DirectoryNode* findNodeByPath(DirectoryTree* tree, const char* path);
void extractDirectoryPath(char *path);
void getPath1(DirectoryTree* directoryTree, DirectoryNode* node, Stack* stack, char* path);
int movePath(DirectoryTree *currentDirectoryTree, char *dirPath);
int moveCurrent(DirectoryTree *currentDirectoryTree, char *dirPath);
void getPath(DirectoryTree *dirTree, DirectoryNode *dirNode, Stack *dirStack, char *temp);
DirectoryNode* findNode(DirectoryNode* currentNode, const char* nodeName);
DirectoryNode* findNodeInTree(DirectoryTree* tree, const char* nodeName);
DirectoryNode* findNodeInParent(DirectoryNode* parentNode, const char* nodeName);
int countChildren(DirectoryNode* directoryNode);
void trim(char *str);

//mkdir.c
int MakeDir(DirectoryTree* currentDirectoryTree, char* dirName, char type, int mode);
int Mkdir(DirectoryTree* currentDirectoryTree, char* cmd);
char* getDir(char* dirPath);
void* mkdirThread(void* arg);

int modeToPermission(DirectoryNode* dirNode);
int HasPermission(DirectoryNode* dirNode, char o);

//cat.c
void cat(DirectoryTree* currentDirectoryTree,char* var);
void* cat_thread(void* arg);

//User.c
UserList *loadUserList();
UserNode *userExistence(UserList *userList, char *name);
int readUser(UserList *userList, char *tmp);
void SaveUserList(UserList *userList);
void userWrite(FILE *tempFile, UserList *userList, UserNode *userNode);
const char* getGroupName(int gid);
const char* getUserName(int uid);

//linux.c
void login(UserList *UsrList, DirectoryTree *dirTree);
void printPrompt(DirectoryTree *dirTree, Stack *dirStack);
void Start();

//cp.c
int cp(DirectoryTree *currentDirectoryTree, char *cmd);
DirectoryNode* copyNode(DirectoryNode* source);

//more.c
int getch(void); // 입력 버퍼 X, 문자 표시 X 입력 함수
int more_line();
void more(char *filename);

//touch.c
DirectoryNode* findNodeToTouch(DirectoryTree* tree, const char* path);
int touch_option(DirectoryTree *dtree, char *cmd);
void *touch_thread(void* arg); 

//grep.c
void *grep_thread(void* arg);
int grep_option(DirectoryTree *dTree,char *cmd);
char *strcasestr(const char *sen, const char *str);

//mv.c
int mv(DirectoryTree* currentDirectoryTree, char* cmd);

//time
bool is_valid_date(int year, int month, int day);
bool is_valid_time(int hour, int minute, int second);
bool is_valid_touch_time(const char *timestr);
void getToday(Date *date);
void getWeekday(int type);
void getMonth(int type);
time_t timeSetting(const char* time_str);
extern time_t *ltime;
extern struct tm *today;

extern DirectoryTree* linuxFileSystem;
extern Stack* dirStack;
extern UserList* userList;
extern FILE* Directory;
extern FILE* User;
extern UserNode* currentUser;// 8진수를 2진수로 변환하는 함수
#endif