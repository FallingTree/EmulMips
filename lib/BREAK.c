#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <limits.h>

#include "emul.h"
#include "mem.h"
#include "is_type.h"
#include "common/notify.h"

int exec(unsigned int* jump, pm_glob param, INST inst){
	
	WARNING_MSG("Breakpoint\n");
	return 0;

}

int print(){
	printf("Fonction BREAK\n");
	return -1;
}
