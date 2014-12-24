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


	int test = (unsigned int) registre[inst.rs].content < (unsigned int) ( (int) inst.immediate);

	registre[inst.rt].content = test;

	return 0;
}

int print(){
	printf("Fonction SLTIU\n");
	return 0;
}