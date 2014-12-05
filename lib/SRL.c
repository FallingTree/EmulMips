#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <limits.h>

#include "emul.h"
#include "mem.h"
#include "is_type.h"
#include "trouver.h"
#include "common/notify.h"


int exec(unsigned int* jump, pm_glob param, INST inst){

	reg *registre = param.p_registre;

	registre[inst.rd].content = registre[inst.rt].content >> inst.sa ;

	return 0;
}

int print(){
	printf("Fonction SRL\n");
	return 0;
}
