#include "vector.h"

#include <stdlib.h>

int main(int argc, char* argv[]) {
  int n = atoi(argv[1]);

  Vector v;
  vector_init(&v, sizeof(int), 1);
  for (int i = 0; i < n; ++i) {
    vector_push_back(&v, &i);
  }
  printf("Size of vector: %ld\n", vector_size(&v));
  printf("AllocSize of vector: %ld\n", vector_alloc_size(&v));
  vector_shrink(&v);
  printf("Size of vector: %ld\n", vector_size(&v));
  printf("AllocSize of vector: %ld\n", vector_alloc_size(&v));
  for (int i = n; i > 0; --i) {
    int j;
    vector_pop_back(&v, &j);
    printf("Number at position %d: %d\n", i-1, j);
  }
  vector_destroy(&v);
  return 0;
}

