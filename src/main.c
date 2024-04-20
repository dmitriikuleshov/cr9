#include "../headers/table.h"


void trim_newline(char *str) {
    char* pos;
    if ((pos = strchr(str, '\n')) != NULL)
        *pos = '\0';
}


long get_file_size(FILE *file) {
    long size;
    fseek(file, 0L, SEEK_END); // Переходим в конец файла
    size = ftell(file); // Получаем текущую позицию, что равно размеру файла
    fseek(file, 0L, SEEK_SET); // Возвращаемся в начало файла
    return size;
}

int is_file_empty(FILE *file) {
    int c;
    while ((c = fgetc(file)) != EOF) {
        if (c != ' ' && c != '\n' && c != '\t') { // Проверяем символы, не являющиеся пробелами
            fseek(file, 0L, SEEK_SET); // Возвращаемся в начало файла
            return 0; // Файл не пустой
        }
    }
    fseek(file, 0L, SEEK_SET); // Возвращаемся в начало файла
    return 1; // Файл пустой или состоит только из пробелов
}


bool load_table_from_file(const char* filename, Table* table) {
    FILE *file = fopen(filename, "r");
    if (!file) {
        printf("Failed to open file: %s\n", filename);
        return false;
    }

    if (is_file_empty(file)) {
        printf("The file is empty\n");
        fclose(file);
        exit(EXIT_FAILURE);
    }

    char line[256];
    uint64_t count = 0;
    uint64_t size = 12; // Максимально возможное количество элементов

    Record* records = malloc(size * sizeof(Record));
    if (!records) {
        fclose(file);
        printf("Memory allocation failed\n");
        return false;
    }

    while (fgets(line, sizeof(line), file) && count < size) {
        trim_newline(line);
        Record record;
        char key_char[6]; // временный буфер для строкового ключа + '\0'

        // Считываем данные: целочисленный ключ, строковый ключ, значение
        if (sscanf(line, "%d %5s %[^\n]s", &record.key.key_int, key_char, line) == 3) {
            strncpy(record.key.key_char, key_char, sizeof(record.key.key_char));
            record.value = strdup(line);
            records[count++] = record;
        } else {
            printf("Error reading line: %s\n", line);
            fclose(file);
            free(records);
            exit(EXIT_FAILURE); // завершаем программу в случае ошибки чтения
        }
    }

    fclose(file);

    table->rows = records;
    table->count = count;
    return true;
}


void free_table(Table *table) {
    for (uint64_t i = 0; i < table->count; i++) {
        free(table->rows[i].value);
    }
    free(table->rows);
}

// похоже на build-max-heap, но учитывает, что части массива могут быть отсортированы
void heapify(Record arr[], uint64_t n, uint64_t i) {
    uint64_t largest = i;
    // Вычисляем индексы левого и правого потомка для узла i
    uint64_t l = 2 * i + 1;
    uint64_t r = 2 * i + 2;

    // Проверяем, существует ли левый потомок, и больше ли его ключ, чем ключ largest
    if (l < n && (arr[l].key.key_int > arr[largest].key.key_int || (arr[l].key.key_int == arr[largest].key.key_int && strcmp(arr[l].key.key_char, arr[largest].key.key_char) > 0))) {
        largest = l;
    }

    // Проверяем, существует ли правый потомок, и больше ли его ключ, чем ключ largest
    if (r < n && (arr[r].key.key_int > arr[largest].key.key_int || (arr[r].key.key_int == arr[largest].key.key_int && strcmp(arr[r].key.key_char, arr[largest].key.key_char) > 0))) {
        largest = r;
    }

    // Если largest не равен i, значит свойство кучи нарушено, и нужно произвести обмен
    if (largest != i) {
        // Меняем местами узлы arr[i] и arr[largest]
        Record temp = arr[i];
        arr[i] = arr[largest];
        arr[largest] = temp;

        // Рекурсивно вызываем heapify для поддерева с корнем в узле largest
        heapify(arr, n, largest);
    }
}

// nlogn
void heapSort(Record arr[], uint64_t n) {
    // Строим начальную кучу, начиная с последнего уровня и двигаясь вверх
    for (int i = n / 2 - 1; i >= 0; i--) {
        heapify(arr, n, i);
    }

    // После построения начальной кучи последовательно извлекаем элементы и перестраиваем кучу
    for (uint64_t i = n - 1; i > 0; i--) {
        Record temp = arr[0];
        arr[0] = arr[i];
        arr[i] = temp;
        // Перестраиваем кучу для уменьшенного размера
        heapify(arr, i, 0);
    }
}


int binarySearch(Record arr[], uint64_t l, uint64_t r, ComplexKey key) {
    while (l <= r) {
        uint64_t mid = l + (r - l) / 2;

        if (arr[mid].key.key_int == key.key_int && strcmp(arr[mid].key.key_char, key.key_char) == 0) {
            return mid;
        }

        if (arr[mid].key.key_int < key.key_int || (arr[mid].key.key_int == key.key_int && strcmp(arr[mid].key.key_char, key.key_char) < 0)) {
            l = mid + 1;
        } else {
            r = mid - 1;
        }
    }

    return -1;
}


int main() {
    char filename[100];
    int user_number;
    char user_string[6]; // 5 символов + 1 символ для завершающего нуля

    printf("Enter file name: ");
    scanf("%s", filename);

    printf("Enter a number: ");
    scanf("%d", &user_number);

    printf("Enter a string of up to five characters: ");
    scanf("%s", user_string);


    Table table;
    if (load_table_from_file(filename, &table)) {
        printf("Table loaded successfully.\n");

        // Сортировка таблицы
        heapSort(table.rows, table.count);

        // Печать отсортированной таблицы
        printf("Sorted table:\n");
        for (uint64_t i = 0; i < table.count; i++) {
            printf("%d %s %s\n", table.rows[i].key.key_int, table.rows[i].key.key_char, table.rows[i].value);
        }

        // Поиск элемента в таблице
        ComplexKey searchKey;
        searchKey.key_int = user_number;
        strncpy(searchKey.key_char, user_string, sizeof(searchKey.key_char));

        int index = binarySearch(table.rows, 0, table.count - 1, searchKey);
        if (index != -1) {
            printf("Element found at index %d: %d %s %s\n", index, table.rows[index].key.key_int, table.rows[index].key.key_char, table.rows[index].value);
        } else {
            printf("Element not found.\n");
        }
    }

    free_table(&table);
    return 0;
}