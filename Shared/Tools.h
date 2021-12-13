#ifndef TOOLS_H
#define TOOLS_H

#include "stdio.h"
#include "stdlib.h"
#include "assert.h"

#define COMPARE_DOUBLE_EPS 1e-20

void* safeCalloc(size_t count, size_t size);

size_t makeHash(const char* str, size_t len);

void skipDelims(char** string);

int doubleCompare(double lhv, double rhv);

#endif //!TOOLS_H