#include "../headers/table.h"


void trim_newline(char *str) {
    char* pos;
    if ((pos = strchr(str, '\n')) != NULL)
        *pos = '\0';
}


bool load_table_from_file(const char* filename, Table* table) {
    FILE *file = fopen(filename, "r");
    if (!file) {
        printf("Failed to open file: %s\n", filename);
        return false;
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


void heapify(Record arr[], uint64_t n, uint64_t i) {
    uint64_t largest = i;
    uint64_t l = 2 * i + 1;
    uint64_t r = 2 * i + 2;

    if (l < n && (arr[l].key.key_int > arr[largest].key.key_int || (arr[l].key.key_int == arr[largest].key.key_int && strcmp(arr[l].key.key_char, arr[largest].key.key_char) > 0))) {
        largest = l;
    }

    if (r < n && (arr[r].key.key_int > arr[largest].key.key_int || (arr[r].key.key_int == arr[largest].key.key_int && strcmp(arr[r].key.key_char, arr[largest].key.key_char) > 0))) {
        largest = r;
    }

    if (largest != i) {
        Record temp = arr[i];
        arr[i] = arr[largest];
        arr[largest] = temp;

        heapify(arr, n, largest);
    }
}


void heapSort(Record arr[], uint64_t n) {
    for (uint64_t i = n / 2 - 1; i >= 0; i--) {
        heapify(arr, n, i);
    }

    for (uint64_t i = n - 1; i > 0; i--) {
        Record temp = arr[0];
        arr[0] = arr[i];
        arr[i] = temp;

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
    Table table;
    if (load_table_from_file("data.txt", &table)) {
        printf("Table loaded successfully.\n");

        for (uint64_t i = 0; i < table.count; i++) {
            printf("%d %s %s\n", table.rows[i].key.key_int, table.rows[i].key.key_char, table.rows[i].value);
        }
    }


    free_table(&table);
    return 0;
}