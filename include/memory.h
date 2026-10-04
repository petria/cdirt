#ifndef CDIRT_MEMORY_H
#define CDIRT_MEMORY_H
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdint.h>
#include <stdarg.h>
#include <limits.h>

/* Allocation failure must not enter the signal recovery/autosave path. */
static inline void memory_failure(void) {
  fputs("C-Dirt: allocation or size failure\n", stderr);
  exit(EXIT_FAILURE);
}
static inline size_t memory_add(size_t a, size_t b) {
  if (b > SIZE_MAX - a) memory_failure();
  return a + b;
}
static inline void *memory_alloc(size_t count, size_t size) {
  void *p;
  if (size && count > SIZE_MAX / size) memory_failure();
  if (!count || !size) return NULL;
  p = calloc(count, size);
  if (!p) memory_failure();
  return p;
}
static inline void *memory_copy(const void *s, size_t len) {
  void *p = memory_alloc(len, 1);
  if (len) memcpy(p, s, len);
  return p;
}
static inline char *memory_string(const char *s) {
  return memory_copy(s, memory_add(strlen(s), 1));
}
static inline char *text_vformat(const char *fmt, va_list args) {
  va_list copy;
  int n;
  char *s;
  va_copy(copy, args);
  n = vsnprintf(NULL, 0, fmt, copy);
  va_end(copy);
  if (n < 0) memory_failure();
  s = memory_alloc(memory_add((size_t)n, 1), 1);
  va_copy(copy, args);
  if (vsnprintf(s, (size_t)n + 1, fmt, copy) != n) memory_failure();
  va_end(copy);
  return s;
}
static inline char *text_format(const char *fmt, ...) {
  va_list args;
  char *s;
  va_start(args, fmt);
  s = text_vformat(fmt, args);
  va_end(args);
  return s;
}
/* Builders own data. text_take transfers it; borrowed scratch is documented
 * at the few public legacy interfaces that still return it. */
typedef struct { char *data; size_t len, cap; } Text;
static inline void text_reserve(Text *t, size_t extra) {
  size_t need = memory_add(memory_add(t->len, extra), 1), cap;
  char *p;
  if (need <= t->cap) return;
  cap = t->cap ? t->cap : 64;
  while (cap < need) {
    if (cap > SIZE_MAX / 2) { cap = need; break; }
    cap *= 2;
  }
  p = realloc(t->data, cap);
  if (!p) memory_failure();
  t->data = p; t->cap = cap;
  t->data[t->len] = 0;
}
static inline void text_bytes(Text *t, const char *s, size_t n) {
  text_reserve(t, n);
  if (n) memcpy(t->data + t->len, s, n);
  t->len += n; t->data[t->len] = 0;
}
static inline void text_append(Text *t, const char *s) {
  if (s) text_bytes(t, s, strlen(s));
}
static inline void text_char(Text *t, char c) { text_bytes(t, &c, 1); }
static inline char *text_take(Text *t) {
  char *s;
  text_reserve(t, 0); s = t->data;
  t->data = NULL; t->len = t->cap = 0;
  return s;
}
#endif
