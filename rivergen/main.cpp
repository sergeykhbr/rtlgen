// main.cpp : Defines the entry point for the application.
//

#include <iostream>
#include "prj_river.h"

int main()
{
    RiverProject *prj = new RiverProject("_generated");
    prj->generate(SYSC_ALL);
    return 0;
}
