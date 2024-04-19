#ifndef LW_HEADER_H
#define LW_HEADER_H

#include <inttypes.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <stdlib.h>

typedef struct {
	char key_char[5];
	int key_int;
} ComplexKey;


typedef struct Record {
	ComplexKey key;
	char* value;
} Record;


typedef struct {
	Record* rows;
	uint64_t count;
	uint64_t max_key;
} Table;

#endif