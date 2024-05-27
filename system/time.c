#include "../header/main.h"

time_t timeSetting(const char* time_str) {
    time_t file_time;

    char* dot_ptr = strchr(time_str, '.'); // 소수점 위치 찾기

    // 소수점 이전의 문자열의 길이 계산
    size_t length = (dot_ptr != NULL) ? (size_t)(dot_ptr - time_str) : strlen(time_str);

    // 새로운 버퍼 할당 (널 문자를 포함하여 길이 + 1 만큼 할당)
    char* result = (char*)malloc(length + 1);

    // 소수점 이전의 문자열을 새로운 버퍼에 복사
    strncpy(result, time_str, length);

    // 복사된 문자열의 끝에 널 문자 추가
    result[length] = '\0';

    // 시간 정보를 저장할 구조체
    struct tm time_struct = {0};

    // 문자열을 tm 구조체로 변환
    if (sscanf(result, "%4d%2d%2d%2d%2d", &time_struct.tm_year, &time_struct.tm_mon, &time_struct.tm_mday, &time_struct.tm_hour, &time_struct.tm_min) != 5) {
        printf("sscanf failed\n");
        free(result);
        return (time_t)-1; // 실패 시 -1 반환
    }

    // tm_year는 1900년 기준, tm_mon은 0-11 범위
    time_struct.tm_year -= 1900;
    time_struct.tm_mon -= 1;

    // time_t 구조체 생성
    file_time = mktime(&time_struct);

    int seconds = 0;
    // 소수점 이하의 초를 가져와서 처리
    if (dot_ptr != NULL) {
        seconds = atoi(dot_ptr + 1); // 소수점 이하의 초를 int 형태로 변환
        file_time += (time_t)seconds; // 초를 추가하여 time_t 값을 완성
    }

    free(result);

    return file_time;
}


bool is_valid_date(int year, int month, int day) {
    // 월이 유효한지 확인 (1~12월)
    if (month < 1 || month > 12) {
        return false;
    }

    // 윤년 계산
    bool is_leap_year = (year % 4 == 0 && year % 100 != 0) || (year % 400 == 0);

    // 각 달의 일 수
    int days_in_month[] = { 31, is_leap_year ? 29 : 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31 };

    // 일 이 유효한지 확인
    if (day < 1 || day > days_in_month[month - 1]) {
        return false;
    }

    return true;
}

bool is_valid_time(int hour, int minute, int second) {
    // 시간이 유효한지 확인
    if (hour < 0 || hour > 23) {
        return false;
    }

    // 분이 유효한지 확인
    if (minute < 0 || minute > 59) {
        return false;
    }

    // 초가 유효한지 확인
    if (second < 0 || second > 59) {
        return false;
    }

    return true;
}

bool is_valid_touch_time(const char *timestr) {
    // 길이가 12 또는 15인지 확인 (초가 있는 경우)
    int len = strlen(timestr);
    if (len != 12 && len != 15) {
        return false;
    }

    // 숫자인지 확인
    for (int i = 0; i < len; i++) {
        if (i == 12 && timestr[i] == '.') {
            continue; // 소수점은 건너뜀
        }
        if (!isdigit(timestr[i])) {
            return false;
        }
    }

    // 시간 정보 추출
    char year_str[5] = {0};
    char month_str[3] = {0};
    char day_str[3] = {0};
    char hour_str[3] = {0};
    char minute_str[3] = {0};
    char second_str[3] = {0};

    strncpy(year_str, timestr, 4);
    strncpy(month_str, timestr + 4, 2);
    strncpy(day_str, timestr + 6, 2);
    strncpy(hour_str, timestr + 8, 2);
    strncpy(minute_str, timestr + 10, 2);
    if (len == 15) {
        strncpy(second_str, timestr + 13, 2);
    } else {
        strcpy(second_str, "0"); // 초가 없으면 0초로 설정
    }

    int year = atoi(year_str);
    int month = atoi(month_str);
    int day = atoi(day_str);
    int hour = atoi(hour_str);
    int minute = atoi(minute_str);
    int second = atoi(second_str);

    // 날짜와 시간이 유효한지 확인
    if (!is_valid_date(year, month, day)) {
        return false;
    }
    if (!is_valid_time(hour, minute, second)) {
        return false;
    }

    return true;
}


void getMonth(int type) {
    switch(type) {
        case 1:
            printf("Jan ");
            break;
        case 2:
            printf("Feb ");
            break;
        case 3:
            printf("Mar ");
            break;
        case 4:
            printf("Apr ");
            break;
        case 5:
            printf("May ");
            break;
        case 6:
            printf("Jun ");
            break;
        case 7:
            printf("Jul ");
            break;
        case 8:
            printf("Aug ");
            break;
        case 9:
            printf("Sep ");
            break;
        case 10:
            printf("Oct ");
            break;
        case 11:
            printf("Nov ");
            break;
        case 12:
            printf("Dec ");
            break;
        default:
            break;
    }
}

void getWeekday(int type) {
    switch(type) {
        case 0:
            printf("Sun ");
            break;
        case 1:
            printf("Mon ");
            break;
        case 2:
            printf("Tue ");
            break;
        case 3:
            printf("Wed ");
            break;
        case 4:
            printf("Thu ");
            break;
        case 5:
            printf("Fri ");
            break;
        case 6:
            printf("Sat ");
            break;
        default:
            break;
    }
}

void getToday(Date *date) {
    time(&ltime);
    today = localtime(&ltime);

    date->weekday = today->tm_wday;
    date->month = today->tm_mon + 1;
    date->day = today->tm_mday;
    date->hour = today->tm_hour;
    date->minute = today->tm_min;
    date->second = today->tm_sec;
    date->year = today->tm_year + 1900;
}