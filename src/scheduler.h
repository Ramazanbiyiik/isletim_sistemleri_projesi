#ifndef SCHEDULER_H
#define SCHEDULER_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>

#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"

/* Sabitler */
#define MAX_TASKS 50
#define QUANTUM_TIME 1000 // 1 saniye

/* Renkler */
#define ANSI_RESET   "\x1b[0m"
#define ANSI_RED     "\x1b[31m"
#define ANSI_GREEN   "\x1b[32m"
#define ANSI_YELLOW  "\x1b[33m"
#define ANSI_BLUE    "\x1b[34m"
#define ANSI_MAGENTA "\x1b[35m"
#define ANSI_CYAN    "\x1b[36m"

/* Durumlar */
typedef enum {
    TASK_NOT_ARRIVED,
    TASK_READY,
    TASK_RUNNING,
    TASK_SUSPENDED,
    TASK_COMPLETED
} TaskState;

/* Görev Bilgisi */
typedef struct {
    int id;
    int arrivalTime;
    int priority;       // 0: RT, 1-3: User
    int burstTime;
    int remainingTime;
    TaskState state;
    TaskHandle_t handle;
    char color[10];
} TaskInfo;

/* Global Değişkenler */
extern TaskInfo tasks[MAX_TASKS];
extern int taskCount;

/* Fonksiyonlar */
void T_Scheduler(void *pvParameters);
void T_Worker(void *pvParameters);
void readFile(const char* filename);

#endif