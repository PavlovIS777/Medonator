#include "./Shared/Tools.h"
#include "./Akinator/Akinator.hpp"
#include <stdio.h>
#include <string.h>
int main(void)
{
    Akinator* akinator = initAkinator();
    doAkinator(akinator);
    return 0;
}