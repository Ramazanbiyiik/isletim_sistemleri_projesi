CC = gcc
CFLAGS = -Wall -Wextra -g -pthread -I./src

# FreeRTOS klasör yolunuz (Burayı kontrol edin)
FREERTOS_DIR = ./FreeRTOS

# Include Yolları
INCLUDES = -I./src \
           -I$(FREERTOS_DIR)/include \
           -I$(FREERTOS_DIR)/portable/ThirdParty/GCC/Posix \
           -I$(FREERTOS_DIR)/portable/ThirdParty/GCC/Posix/utils

# Kaynak Dosyalar
SRCS = src/main.c \
       src/scheduler.c \
       src/tasks.c \
       $(FREERTOS_DIR)/tasks.c \
       $(FREERTOS_DIR)/list.c \
       $(FREERTOS_DIR)/queue.c \
       $(FREERTOS_DIR)/timers.c \
       $(FREERTOS_DIR)/event_groups.c \
       $(FREERTOS_DIR)/stream_buffer.c \
       $(FREERTOS_DIR)/portable/MemMang/heap_3.c \
       $(FREERTOS_DIR)/portable/ThirdParty/GCC/Posix/port.c \
       $(FREERTOS_DIR)/portable/ThirdParty/GCC/Posix/utils/wait_for_event.c

TARGET = freertos_sim

$(TARGET): $(SRCS)
	$(CC) $(CFLAGS) $(INCLUDES) $(SRCS) -o $(TARGET)

clean:
	rm -f $(TARGET)