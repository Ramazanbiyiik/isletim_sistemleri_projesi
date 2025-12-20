#include "scheduler.h"

TaskInfo tasks[MAX_TASKS];
int taskCount = 0;

void assignColor(int taskIndex) {
    const char* colors[] = {ANSI_RED, ANSI_GREEN, ANSI_YELLOW, ANSI_BLUE, ANSI_MAGENTA, ANSI_CYAN};
    strcpy(tasks[taskIndex].color, colors[taskIndex % 6]);
}

void readFile(const char* filename) {
    FILE *file = fopen(filename, "r");
    if (!file) { printf("Dosya bulunamadi: %s\n", filename); exit(1); }

    int arrival, priority, burst;
    int id_counter = 0;

    while (fscanf(file, "%d, %d, %d", &arrival, &priority, &burst) != EOF) {
        if (taskCount >= MAX_TASKS) break;
        tasks[taskCount].id = id_counter++;
        tasks[taskCount].arrivalTime = arrival;
        tasks[taskCount].priority = priority;
        tasks[taskCount].burstTime = burst;
        tasks[taskCount].remainingTime = burst;
        tasks[taskCount].state = TASK_NOT_ARRIVED;
        assignColor(taskCount);
        taskCount++;
    }
    fclose(file);
    printf("Okunan gorev sayisi: %d\n", taskCount);
}

int main(int argc, char *argv[]) {
    if (argc != 2) { printf("Kullanim: ./freertos_sim giris.txt\n"); return 1; }
    
    readFile(argv[1]);

    xTaskCreate(T_Scheduler, "Scheduler", 4096, NULL, tskIDLE_PRIORITY + 2, NULL);
    vTaskStartScheduler();
    return 0;
}
