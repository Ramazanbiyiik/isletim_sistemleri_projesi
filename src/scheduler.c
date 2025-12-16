#include "scheduler.h"

/* Global Değişkenler */
static int currentTime = 0;
static TaskInfo* currentTask = NULL;

/* GÖREVLERİN KUYRUĞA GİRİŞ ZAMANI (Sıralama İçin) */
static long long taskQueueTimes[MAX_TASKS];

void initScheduler() {
    for(int i=0; i<MAX_TASKS; i++) taskQueueTimes[i] = 0;
}

/* ZAMAN AŞIMI KONTROLÜ */
void checkTimeouts() {
    for (int i = 0; i < taskCount; i++) {
        // Sadece bitmemiş, gelmiş ve şu an çalışmayanları kontrol et
        if (tasks[i].state != TASK_COMPLETED && tasks[i].state != TASK_NOT_ARRIVED && &tasks[i] != currentTask) {
            
            /* Bekleme süresi hesabı */
            // Not: Öncelik değişince arrivalTime güncellendiği için süre sıfırlanmış olur.
            int timeInSystem = currentTime - tasks[i].arrivalTime;
            
            // Eğer öncelik değişimiyle arrivalTime güncellendiyse, executedTime hesabını
            // düşmemiz gerekmez (veya negatif çıkabilir), sadece o kuyruktaki bekleme süresine bakarız.
            // Ancak basitlik adına: arrivalTime güncellendiği an timeInSystem 0 olur.
            
            int waitingTime = timeInSystem; 
            // Daha hassas hesap için çalışılan süreyi düşmek istersek:
            // int executedTime = tasks[i].burstTime - tasks[i].remainingTime;
            // waitingTime -= executedTime;
            // Ancak hocanın mantığında öncelik değişimi tam bir "reset" gibi görünüyor.

            if (waitingTime >= 20) {
                tasks[i].state = TASK_COMPLETED;
                
                printf("%s%d.0000 sn\tproses zamanasimi\t(id:%04d\toncelik:%d\tkalan sure:%d sn)%s\n", 
                       ANSI_RED, currentTime, tasks[i].id, tasks[i].priority, tasks[i].remainingTime, ANSI_RESET);
                
                if (tasks[i].handle != NULL) {
                    vTaskDelete(tasks[i].handle);
                    tasks[i].handle = NULL; 
                }
            }
        }
    }
}

/* GÖREV SEÇİM ALGORİTMASI */
TaskInfo* selectNextTask() {
    /* 1. ÖNCELİK 0 (Real-Time) */
    for (int i = 0; i < taskCount; i++) {
        if (tasks[i].state == TASK_READY && tasks[i].priority == 0) {
            return &tasks[i];
        }
    }

    /* 2. DİĞER ÖNCELİKLER (1-5 Arası) - KUYRUK ZAMANI ÖNCELİKLİ */
    for (int p = 1; p <= 5; p++) { 
        TaskInfo* selected = NULL;
        long long minQueueTime = -1;

        for (int i = 0; i < taskCount; i++) {
            if (tasks[i].state == TASK_READY && tasks[i].priority == p) {
                if (selected == NULL || taskQueueTimes[i] < minQueueTime) {
                    selected = &tasks[i];
                    minQueueTime = taskQueueTimes[i];
                }
                else if (taskQueueTimes[i] == minQueueTime) {
                    if (tasks[i].id < selected->id) selected = &tasks[i];
                }
            }
        }
        if (selected != NULL) return selected;
    }
    return NULL; 
}

void T_Scheduler(void *pvParameters) {
    (void) pvParameters;
    initScheduler();

    while (1) {
        /* 1. YENİ GELENLER */
        for (int i = 0; i < taskCount; i++) {
            if (tasks[i].arrivalTime == currentTime) {
                tasks[i].state = TASK_READY;
                taskQueueTimes[i] = currentTime; 
                
                xTaskCreate(T_Worker, "Worker", 1024, (void*)&tasks[i], tskIDLE_PRIORITY + 1, &tasks[i].handle);
                vTaskSuspend(tasks[i].handle);
            }
        }

        /* 2. MEVCUT GÖREV */
        if (currentTask != NULL) {
            if (currentTask->state == TASK_COMPLETED) {
                currentTask = NULL;
            } else {
                currentTask->remainingTime--;

                if (currentTask->remainingTime <= 0) {
                    /* GÖREV BİTTİ */
                    currentTask->state = TASK_COMPLETED;
                    printf("%s%d.0000 sn\tproses sonlandi\t\t(id:%04d\toncelik:%d\tkalan sure:0 sn)%s\n", 
                           currentTask->color, currentTime, currentTask->id, currentTask->priority, ANSI_RESET);
                    
                    if (currentTask->handle != NULL) {
                        vTaskDelete(currentTask->handle);
                        currentTask->handle = NULL;
                    }
                    currentTask = NULL;
                } else {
                    /* QUANTUM DOLDU -> ASKIYA AL */
                    vTaskSuspend(currentTask->handle); 
                    
                    /* Priority 0 DEĞİLSE ceza uygula */
                    if (currentTask->priority > 0) {
                        if (currentTask->priority < 5) {
                            currentTask->priority++;
                        }
                        
                        /* KRİTİK NOKTA: Öncelik değiştiği veya askıya alındığı için
                           1. Kuyruğun sonuna at (Sıralama için)
                           2. Geliş zamanını güncelle (Zaman Aşımı Sıfırlama için) */
                        taskQueueTimes[currentTask->id] = currentTime; 
                        currentTask->arrivalTime = currentTime; // ZAMAN AŞIMI RESET!

                        printf("%s%d.0000 sn\tproses askida\t\t(id:%04d\toncelik:%d\tkalan sure:%d sn)%s\n", 
                               currentTask->color, currentTime, currentTask->id, currentTask->priority, currentTask->remainingTime, ANSI_RESET);
                    }
                }
            }
        }

        /* 3. SEÇİM YAP */
        TaskInfo *nextTask = selectNextTask();

        if (nextTask != NULL) {
            // Mesaj Mantığı
            if (nextTask->burstTime == nextTask->remainingTime) {
                 printf("%s%d.0000 sn\tproses basladi\t\t(id:%04d\toncelik:%d\tkalan sure:%d sn)%s\n", 
                       nextTask->color, currentTime, nextTask->id, nextTask->priority, nextTask->remainingTime, ANSI_RESET);
            } else {
                 printf("%s%d.0000 sn\tproses yurutuluyor\t(id:%04d\toncelik:%d\tkalan sure:%d sn)%s\n", 
                       nextTask->color, currentTime, nextTask->id, nextTask->priority, nextTask->remainingTime, ANSI_RESET);
            }

            if (nextTask->handle != NULL) {
                vTaskResume(nextTask->handle);
            }
            currentTask = nextTask;
            
        } else {
            currentTask = NULL;
        }

        /* 4. ZAMAN AŞIMI KONTROLÜ (Seçilen Hariç) */
        checkTimeouts();

        /* 5. ZAMANI İLERLET */
        currentTime++;
        
        int allFinished = 1;
        for(int i=0; i<taskCount; i++) {
            if(tasks[i].state != TASK_COMPLETED) {
                allFinished = 0;
                break;
            }
        }
        
        if(allFinished && taskCount > 0) {
            printf("Tum gorevler tamamlandi.\n");
            exit(0);
        }

        vTaskDelay(pdMS_TO_TICKS(QUANTUM_TIME));
    }
}