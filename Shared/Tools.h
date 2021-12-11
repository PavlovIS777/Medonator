#ifndef TOOLS_H
#define TOOLS_H

#include "stdio.h"
#include "stdlib.h"
#include "assert.h"

void* safeCalloc(size_t count, size_t size);

size_t makeHash(const char* str, size_t len);

void skipDelims(char** string);

#endif //!TOOLS_H