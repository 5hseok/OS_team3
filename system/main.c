#include "../header/main.h"


int main(){
    char command[100];  //명령어를 저장할 배열
    //현재 파일 시스템 구조를 가져오고
    //사용자, 그룹 정보를 가져오고
    //Stack을 초기화 시킨다.

    //이후, login as로 로그인할 유저 이름을 입력받아 로그인한다. 
    //성공 메시지 혹은 도움 혹은 실패 메시지를 출력한 뒤, 
    //현재 유저의 정보를 유저 파일에 저장한다.
    //이후 반복문을 돌리면서 명령어를 입력받고 실행시킨다.

    linuxFileSystem = loadDirectory();
    userList = loadUserList();
    dirStack = InitializeStack();

    login(userList, linuxFileSystem);
    Start();
    SaveUserList(userList);

    while(1){
        printPrompt(linuxFileSystem, dirStack);
        fgets(command, sizeof(command), stdin);
        command[strlen(command)-1] = '\0';
        executeCommand(linuxFileSystem, command);
    }
    return 0;
}