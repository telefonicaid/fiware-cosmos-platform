#include <stdio.h>

#include "au.h"


char au_clearScreen[] = { 0x1b, '[', '2', 'J', 0x1b, '[', '0', ';' ,'0','H', 0 };
