#ifndef _IGASHANDLER_H_
#define _IGASHANDLER_H_
#include <stdbool.h>

void preInit();

void init();

void measure();

bool isValueCritical();

void setBurnerActive(bool isActive);

void setCriticalValue(int criticalValue);

#endif
