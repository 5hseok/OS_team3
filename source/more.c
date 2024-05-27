#include "../header/main.h"

int getch(void) // 입력 버퍼 X, 문자 표시 X 입력 함수
{  
    #ifdef _WIN32
        return _getch();    // int ch;
    #else
        struct termios old;
        struct termios new;

        tcgetattr(0, &old);
        int ch;
        new = old;
        new.c_lflag &= ~(ICANON | ECHO);
        new.c_cc[VMIN] = 1;
        new.c_cc[VTIME] = 0;

        tcsetattr(0, TCSAFLUSH, &new);
        ch = getchar();
        tcsetattr(0, TCSAFLUSH, &old);

        return ch;
    #endif
}

int more_line()
{
    int c;
    while ((c = getch()) != EOF)
    {
        if (c == 'q' || c == 'Q')
            return 0;
        if (c == ' ')
            return PAGELEN;
        if (c == '\n')
            return 1;
    }
    return 0;
}

void more(char *filename)
{
    FILE *fp;
    char line[LINELEN];
    int linenum = 0;
    int reply;

    fp = fopen(filename, "r");
    if (fp == NULL)
    {
        fprintf(stderr, "Error: Cannot open file '%s'\n", filename);
        return;
    }

    while (fgets(line, LINELEN, fp))
    {
        if (linenum == PAGELEN)
        {
            reply = more_line();
            if (reply == 0)
                break;
            else
                linenum -= reply;
        }
        if (fputs(line, stdout) == EOF)
        {
            fclose(fp);
            fprintf(stderr, "Error: Failed to write to stdout\n");
            return;
        }
        linenum++;
    }

    fclose(fp);
}