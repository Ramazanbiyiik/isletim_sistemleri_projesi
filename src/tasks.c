#include "scheduler.h"

void T_Worker(void *pvParameters) {
    (void)pvParameters; // Uyarıyı önlemek için

    while (1) {
        /* Scheduler "Resume" dediğinde 1 sn çalışır */
        vTaskDelay(pdMS_TO_TICKS(QUANTUM_TIME));
        
        /* Görev bitimini Scheduler yönetir, burası sonsuz döngüde bekler */
    }
}