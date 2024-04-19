# Компилятор
CC=gcc
# Флаги компиляции
CFLAGS=-Iinclude -Wall -Wextra -pedantic

# Директории исходных файлов, объектных файлов и исполняемого файла
SRC_DIR=src
OBJ_DIR=bin
BIN_DIR=bin

# Список всех файлов с расширением .c в директории src
SRCS=$(wildcard $(SRC_DIR)/*.c)
# Заменяем путь к файлам .c на путь к файлам .o в директории bin
OBJS=$(patsubst $(SRC_DIR)/%.c,$(OBJ_DIR)/%.o,$(SRCS))
# Имя исполняемого файла
EXECUTABLE=$(BIN_DIR)/main

# Цель по умолчанию: собираем исполняемый файл
all: $(EXECUTABLE)

# Правило для сборки исполняемого файла
$(EXECUTABLE): $(OBJS)
	$(CC) $(CFLAGS) $^ -o $@

# Правило для компиляции каждого файла .c в соответствующий файл .o
$(OBJ_DIR)/%.o: $(SRC_DIR)/%.c
	$(CC) $(CFLAGS) -c $< -o $@

# Цель для очистки объектных файлов и исполняемого файла
clean:
	rm -f $(OBJS) $(EXECUTABLE)

# Цель для компиляции проекта и запуска исполняемого файла main
run: $(EXECUTABLE)
	./$(EXECUTABLE)

# Объявляем цели all, clean и run как фиктивные (phony), чтобы избежать конфликтов с файлами с такими же именами
.PHONY: all clean run
