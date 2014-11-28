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

	reg *registre = param.p_registre;


	int32_t rt_val = registre[inst.rt].content << inst.sa;

	registre[inst.rd].content = rt_val;

	return 0;
}

int print(){
	printf("Fonction SLL\n");
	return 0;
}