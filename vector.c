#include "vector.h"

int vector_init(Vector* v, size_t objSize, size_t initialSize) {
  v->objSize = objSize;
  v->allocSize = initialSize;
  v->usedSize = 0;
  v->buf = (void*) malloc(objSize * initialSize);
  if (v->buf == NULL) {
    return 1;
  }
  return 0;
}

void vector_destroy(Vector* v) {
  if (v->buf != NULL) {
    free(v->buf);
  }
  v->buf = NULL;
  v->allocSize = 0;
  v->usedSize = 0;
}

size_t vector_size(Vector* v) {
  return v->usedSize;
}

size_t vector_alloc_size(Vector* v) {
  return v->allocSize;
}

int vector_reserve(Vector* v, size_t size) {
  if (v->allocSize < size) {
    void* newBuf = realloc(v->buf, size * v->objSize);
    if (newBuf == NULL) {
      return 1;
    }
    v->buf = newBuf;
    v->allocSize = size;
  }
  return 0;
}

int vector_shrink(Vector* v) {
  if (v->usedSize < v->allocSize) {
    void* newBuf = realloc(v->buf, v->usedSize * v->objSize);
    if (newBuf == NULL) {
      return 1;
    }
    v->buf = newBuf;
    v->allocSize = v->usedSize;
  }
  return 0;
}

int vector_push_back(Vector* v, void* data) {
  if (v->usedSize >= v->allocSize) {
    // Vector full, must double size:
    size_t newSize = v->usedSize * 2;
    void* newBuf = realloc(v->buf, newSize * v->objSize);
    if (newBuf == NULL) {
      return 1;
    }
    v->buf = newBuf;
    v->allocSize = newSize;
  }
  char* base = (char*) v->buf;
  memcpy(base + (v->objSize * v->usedSize), data, v->objSize);
  v->usedSize += 1;
}

int vector_pop_back(Vector* v, void* place) {
  if (v->usedSize == 0) {
    return 1;
  }
  char* base = (char*) v->buf;
  v->usedSize -= 1;
  memcpy(place, base + (v->objSize * v->usedSize), v->objSize);
  return 0;
}

void* vector_front(Vector* v) {
  if (v->usedSize == 0) {
    return NULL;
  } else {
    return v->buf;
  }
}

void* vector_back(Vector* v) {
  if (v->usedSize == 0) {
    return NULL;
  } else {
    return (void*) ((char*) v->buf + v->objSize * (v->usedSize - 1));
  }
}

void* vector_at(Vector* v, size_t pos) {
  if (pos >= v->usedSize) {
    return NULL;
  } else {
    return (void*) ((char*) v->buf + v->objSize * pos);
  }
}

int vector_assign(Vector* v, size_t pos, void* value) {
  if (pos >= v->usedSize) {
    return 1;
  }
  void* obj = (void*) ((char*) v->buf + pos * v->objSize);
  memcpy(value, obj, v->objSize);
}

