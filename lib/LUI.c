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

	int masque = 0xffff0000;
	registre[inst.rt].content =  (inst.immediate << 16) & masque;


	return 0;
}

int print(){
	printf("Fonction LUI\n");
	return 0;
}