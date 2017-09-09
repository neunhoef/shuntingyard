#include <stdatomic.h>
#include <stdio.h>
#include <stddef.h>

int main(int argc, char* argv[]) {
  atomic_int_fast32_t a;
  atomic_store(&a, 12);

  printf("Read of atomic value: %ld %ld\n", sizeof(atomic_int_fast32_t),
         atomic_load(&a));
}
