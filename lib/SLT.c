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


	int test = registre[inst.rs].content < registre[inst.rt].content;

	registre[inst.rd].content = test;

	return 0;
}

int print(){
	printf("Fonction SLT\n");
	return 0;
}