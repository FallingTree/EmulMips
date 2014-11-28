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



	int rt_val = (byte) (registre[inst.rt].content & 0x000000ff);

	registre[inst.rd].content = rt_val;




	return 0;
}

int print(){
	printf("Fonction SEB\n");
	return 0;
}