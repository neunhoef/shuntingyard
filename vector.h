#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

typedef struct vector {
  size_t objSize;
  size_t allocSize;
  size_t usedSize;
  void* buf;
} Vector;

int vector_init(Vector* v, size_t objSize, size_t initialSize);
void vector_destroy(Vector* v);
size_t vector_size(Vector* v);
size_t vector_alloc_size(Vector* v);
int vector_reserve(Vector* v, size_t size);
int vector_shrink(Vector* v);
int vector_push_back(Vector* v, void* data);
int vector_pop_back(Vector* v, void* p);
void* vector_front(Vector* v);
void* vector_back(Vector* v);
void* vector_at(Vector* v, size_t pos);
int vector_assign(Vector* v, size_t pos, void* value);

