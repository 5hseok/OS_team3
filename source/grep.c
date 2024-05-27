#include "../header/main.h"

char *strcasestr(const char *sen, const char *str) {
    for (; *sen; ++sen) {
        if (tolower((unsigned char)*sen) == tolower((unsigned char)*str)) {
            const char *S, *s;
            for (S = sen, s = str; *S && *s; ++S, ++s) {
                if (tolower((unsigned char)*S) != tolower((unsigned char)*s)) {
                    break;
                }
            }
            if (!*s) {
                return (char *) sen;
            }
        }
    }
    return NULL;
}

int grep_option(DirectoryTree *dtree, char *cmd) {
    pthread_mutex_t print_mutex;
    pthread_mutex_init(&print_mutex, NULL);
    int file_num = 0;   
    ThreadTree threadTree[MAX_BUFFER];
    pthread_t threadID[MAX_THREAD];
    int thread_n = 0;
    int option = 0;
    char *pattern;
    char *file;
    char *cmd_op;

    if (cmd == NULL) {
        printf("grep: missing operand\n");
        printf("Try 'grep --help' for more information.\n");
        return -1;
    }

    if (cmd[0] == '-') {
        cmd_op = strtok(cmd, " ");
        if (strcmp(cmd_op, "-i") == 0) {
            option = 1;
        }
        else if (strcmp(cmd_op, "-v") == 0) {
            option = 2;
        }
        else if (strcmp(cmd_op, "-n") == 0) {
            option = 3;
        }
        else if (strcmp(cmd_op, "--help") == 0) {
            printf("Usage: grep [OPTION]... PATTERN [FILE]...\n");
            printf("Search for PATTERN in each FILE.\n");
            printf("Example: grep -i 'hello world' menu.h main.c\n\n");
            printf("  -i                     ignore case distinctions\n");
            printf("  -v                     select non-matching lines\n");
            printf("  -n                     print line number with output lines\n");
            printf("      --help     display this help and exit\n\n");
            printf("GNU coreutils online help: <https://www.gnu.org/software/coreutils/>\n");
            printf("Full documentation at: <https://www.gnu.org/software/coreutils/grep>\n");
            printf("or available locally via: info '(coreutils) grep invocation'\n\n");

            return -1;
        }
        else { 
            printf("grep: invalid option -- '%s'\n", cmd_op);
            printf("Try 'grep --help' for more information.\n");
            return -1;
        }

        pattern = strtok(NULL, " ");
        if (pattern == NULL) {
            printf("grep: missing pattern\n");
            printf("Try 'grep --help' for more information.\n");
            return -1;
        }

        file = strtok(NULL, " ");
        if (file == NULL) {
            printf("grep: missing file operand\n");
            printf("Try 'grep --help' for more information.\n");
            return -1;
        }
    }
    else { 
        pattern = strtok(cmd, " ");
        if (pattern == NULL) {
            printf("grep: missing pattern\n");
            printf("Try 'grep --help' for more information.\n");
            return -1;
        }

        file = strtok(NULL, " ");
        if (file == NULL) {
            printf("grep: missing file operand\n");
            printf("Try 'grep --help' for more information.\n");
            return -1;
        }
    }

    while (file != NULL) {
        threadTree[thread_n].option = option;
        threadTree[thread_n].content = pattern;
        threadTree[thread_n].fileName = file;
        threadTree[thread_n].threadTree = dtree;
        threadTree[thread_n].mutex = &print_mutex;  // 뮤텍스 포인터를 전달
        file = strtok(NULL, " ");
        thread_n++;
        file_num++;
    }
    for (int i = 0; i < thread_n; i++) {
        threadTree[i].mode = file_num;
    }

    for (int i = 0; i < thread_n; i++) {
        pthread_create(&threadID[i], NULL, grep_thread, (void*)&threadTree[i]);
    }

    for (int i = 0; i < thread_n; i++) {
        pthread_join(threadID[i], NULL);
    }

    file_num = 0;
    pthread_mutex_destroy(&print_mutex);  // 뮤텍스 해제

    return 1;
}

void *grep_thread(void* arg) {
    ThreadTree* threadTree = (ThreadTree*)arg;
    DirectoryTree *dtree = threadTree->threadTree;
    int option = threadTree->option;
    char* pattern = threadTree->content;
    char* file = threadTree->fileName;
    char* filename;
    int file_num = threadTree->mode;
    pthread_mutex_t *print_mutex = threadTree->mutex;

    DirectoryNode *check = findNodeByPath(dtree, file);

    if (check == NULL) {
        pthread_mutex_lock(print_mutex);
        printf("grep: %s: No such file or directory\n", file);
        pthread_mutex_unlock(print_mutex);
        return NULL;
    }

    char line[MAX_BUFFER];
    int line_number = 0;
    int match_count = 0;

    if (strchr(file, '/') != NULL) {
        char* pathCopy = strdup(file); // 원래 경로를 복사해서 사용
        char* lastSlash = strrchr(pathCopy, '/');
        filename = lastSlash + 1;
    } else {
        filename = file;
    }

    FILE *fp = fopen(filename, "r");
    if (fp == NULL) {
        pthread_mutex_lock(print_mutex);
        printf("grep: %s: Cannot open file\n", file);
        pthread_mutex_unlock(print_mutex);
        return NULL;
    }

    while (fgets(line, sizeof(line), fp) != NULL) {
        line_number++;
        char *match = (option == 1) ? strcasestr(line, pattern) : strstr(line, pattern);
        int first_match = 1; // Flag to track if it's the first match in the line

        if ((option == 0 && match != NULL) ||
            (option == 1 && match != NULL) ||
            (option == 2 && match == NULL) ||
            (option == 2 && strcasestr(line, pattern) != NULL) ||
            (option == 3 && match != NULL)) {

            pthread_mutex_lock(print_mutex); // 뮤텍스 잠금
            if (match != NULL && file_num > 1) { // 파일이 여러 개인 경우
                char *start = line;
                while (match != NULL) {
                    if (first_match && option == 3) {
                        MAGENTA;
                        printf("%s", file);
                        CYAN;
                        printf(":");
                        GREEN;
                        printf("%d", line_number);
                        CYAN;
                        printf(":");
                        DEFAULT;
                        first_match = 0; // 이후에는 파일명을 출력하지 않음
                    } else if (first_match) {
                        MAGENTA;
                        printf("%s", file);
                        CYAN;
                        printf(":");
                        DEFAULT;
                        first_match = 0;
                    }
                    printf("%.*s", (int)(match - start), start);
                    RED;
                    printf("%.*s", (int)strlen(pattern), match);
                    DEFAULT;
                    start = match + strlen(pattern);
                    match = (option == 1) ? strcasestr(start, pattern) : strstr(start, pattern);
                }
                printf("%s", start);
            } else if (match != NULL) { // 파일이 하나만 들어온 경우
                char *start = line;
                while (match != NULL) {
                    if (first_match && option == 3) {
                        GREEN;
                        printf("%d", line_number);
                        CYAN;
                        printf(":");
                        DEFAULT;
                        first_match = 0;
                    }
                    printf("%.*s", (int)(match - start), start);
                    RED;
                    printf("%.*s", (int)strlen(pattern), match);
                    DEFAULT;
                    start = match + strlen(pattern);
                    match = (option == 1) ? strcasestr(start, pattern) : strstr(start, pattern);
                }
                printf("%s", start);
            } else {
                printf("%s", line);
            }
            pthread_mutex_unlock(print_mutex); // 뮤텍스 잠금 해제
        }
    }

    fclose(fp);
    return NULL;
}
