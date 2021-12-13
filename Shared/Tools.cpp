#include "Tools.h"

void* safeCalloc(size_t count, size_t size)
{
    void* temp = calloc(count, size);
    if (temp != nullptr)
        return temp;
    else
    {
        printf("Can't allocte memory.");
        assert(temp);
        return nullptr;
    }
}

size_t makeHash(const char* str, size_t len) 
{
    size_t hash = 0;
    for (size_t it = 0; it < len; str++, it++) 
    {
        hash += (unsigned char)(*str);
        hash += (hash << 20);
        hash ^= (hash >> 12);
    }

    hash += (hash << 6);
    hash ^= (hash >> 22);
    hash += (hash << 29);

    return hash;
}

void skipDelims(char** string)
{
    while(**string == '\n' || **string == ' ')
        ++(*string);
}

int doubleCompare(double lhv, double rhv)
{
    return lhv - rhv > COMPARE_DOUBLE_EPS? 1 : 0;
}