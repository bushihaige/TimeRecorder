#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <ctype.h>

#define MAX_TASKS 100
#define MAX_NAME_LENGTH 100
#define FILENAME "work_log.csv"
#define TIME_FORMAT "%Y-%m-%d %H:%M:%S"

typedef enum {
    LOW,
    MEDIUM,
    HIGH
} FocusLevel;

typedef enum {
    DEEPWORK,
    SHALLOWWORK,
    HAPPYWORK,
    DISTRACTION
} Eventtype;

typedef struct {
    time_t start_time;
    time_t end_time;
    Eventtype event;
    char task_name[MAX_NAME_LENGTH];
    FocusLevel focus;
} TimeNode;

TimeNode tasks[MAX_TASKS];
int task_count = 0;

const char* focus_level_to_str(FocusLevel level) {
    switch (level) {
    case LOW: return "LOW";
    case MEDIUM: return "MEDIUM";
    case HIGH: return "HIGH";
    default: return "UNKNOWN";
    }
}

const char* event_type_to_str(Eventtype type) {
    switch (type) {
    case DEEPWORK: return "DEEPWORK";
    case SHALLOWWORK: return "SHALLOWWORK";
    case HAPPYWORK: return "HAPPYWORK";
    case DISTRACTION: return "DISTRACTION";
    default: return "UNKNOWN";
    }
}

void format_time(char* buffer, size_t size, time_t time) {
    struct tm* timeinfo = localtime(&time);
    strftime(buffer, size, TIME_FORMAT, timeinfo);
}

void record_new_task() {
    if (task_count >= MAX_TASKS) {
        printf("Maximum tasks reached!\n");
        return;
    }

    if (task_count == 0) {
        tasks[task_count].start_time = time(NULL);
        char timeBuf[20];
        format_time(timeBuf, sizeof(timeBuf), tasks[task_count].start_time);
        printf("Work started at: %s\n", timeBuf);
    }
    else {
        tasks[task_count].start_time = tasks[task_count - 1].end_time;
    }

    tasks[task_count].end_time = time(NULL);

    // 事件类型输入
    int event_type_input;
    do {
        printf("Enter event type: \n");
        printf("[1] Deepwork\n");
        printf("[2] shallowwork\n");
        printf("[3] happywork\n");
        printf("[4] Distraction\n");
        printf("Your choice: ");
        event_type_input = getchar() - '0';
        while (getchar() != '\n');
    } while (event_type_input != 1 && event_type_input != 2 && event_type_input != 3 && event_type_input != 4);

    
    char focus_input ;
    switch (event_type_input) {
    case 1:
        // 任务名输入（对TASK要求）
        printf("Enter event description: ");
        fgets(tasks[task_count].task_name, MAX_NAME_LENGTH, stdin);
        tasks[task_count].task_name[strcspn(tasks[task_count].task_name, "\n")] = '\0';
        tasks[task_count].event = DEEPWORK;
        tasks[task_count].focus = HIGH;
        // 专注等级输入
        
        do {
            printf("Enter focus level (L-LOW, M-MEDIUM, H-HIGH): ");
            focus_input = toupper(getchar());
            while (getchar() != '\n');
        } while (focus_input != 'L' && focus_input != 'M' && focus_input != 'H');

        switch (focus_input) {
        case 'L': tasks[task_count].focus = LOW; break;
        case 'M': tasks[task_count].focus = MEDIUM; break;
        case 'H': tasks[task_count].focus = HIGH; break;
        }
        break;
    case 2:
        // 任务名输入（对TASK要求）
        printf("Enter event description: ");
        fgets(tasks[task_count].task_name, MAX_NAME_LENGTH, stdin);
        tasks[task_count].task_name[strcspn(tasks[task_count].task_name, "\n")] = '\0';
        tasks[task_count].event = SHALLOWWORK;
        
        do {
            printf("Enter focus level (L-LOW, M-MEDIUM): ");
            focus_input = toupper(getchar());
            while (getchar() != '\n');
        } while (focus_input != 'L' && focus_input != 'M');

        switch (focus_input) {
        case 'L': tasks[task_count].focus = LOW; break;
        case 'M': tasks[task_count].focus = MEDIUM; break;
        }
        break;
    case 3:
        printf("Enter event description: ");
        fgets(tasks[task_count].task_name, MAX_NAME_LENGTH, stdin);
        tasks[task_count].task_name[strcspn(tasks[task_count].task_name, "\n")] = '\0';
        tasks[task_count].event = HAPPYWORK;
        tasks[task_count].focus = LOW;  // 开心工作默认LOW专注度
        break;
    case 4:
        tasks[task_count].event = DISTRACTION;
        tasks[task_count].focus = LOW;  // 分心事件默认LOW专注度
        break;
    }

    task_count++;
    printf("Event recorded!\n\n");
}

void save_to_csv() {
    FILE* file = fopen(FILENAME, "w");
    if (!file) {
        printf("Failed to create CSV file!\n");
        return;
    }

    // 写入CSV表头
    fprintf(file, "TaskID,StartTime,EndTime,Duration(min),EventType,TaskName,FocusLevel\n");

    for (int i = 0; i < task_count; i++) {
        double duration = difftime(tasks[i].end_time, tasks[i].start_time) / 60.0;
        char startTime[20], endTime[20];

        format_time(startTime, sizeof(startTime), tasks[i].start_time);
        format_time(endTime, sizeof(endTime), tasks[i].end_time);

        fprintf(file, "%d,%s,%s,%.1f,%s,%s,%s\n",
            i + 1,
            startTime,
            endTime,
            duration,
            event_type_to_str(tasks[i].event),
            tasks[i].task_name,
            focus_level_to_str(tasks[i].focus));
    }

    fclose(file);
    printf("Work log saved to %s\n", FILENAME);
}

void print_menu() {
    printf("\nWork Time Tracker\n");
    printf("1. Record new event\n");
    printf("2. Finish work and save log\n");
    printf("Enter your choice: ");
}

int main() {
    printf("Work Time Tracker started\n");
    char timeBuf[20];
    format_time(timeBuf, sizeof(timeBuf), time(NULL));
    printf("Current time: %s\n", timeBuf);

    int choice;
    do {
        print_menu();
        if (scanf("%d", &choice) != 1) {
            while (getchar() != '\n');
            printf("Invalid input! Please enter a number.\n");
            continue;
        }
        while (getchar() != '\n');

        switch (choice) {
        case 1:
            record_new_task();
            break;
        case 2:
            if (task_count > 0) {
                tasks[task_count - 1].end_time = time(NULL);
                save_to_csv();
            }
            else {
                printf("No tasks recorded!\n");
            }
            break;
        default:
            printf("Invalid choice!\n");
        }
    } while (choice != 2);

    return 0;
}