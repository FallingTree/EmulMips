#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <limits.h>

#include "emul.h"
#include "mem.h"
#include "is_type.h"
#include "common/notify.h"

int exec(unsigned int* p_jump, pm_glob param, INST inst){

	reg *registre = param.p_registre;

	*p_jump = inst.target*4 + ((registre[34].content + 4)&0xf0000000) ;

	return 0;
}

int print(){
	printf("Fonction J\n");
	return 0;
}
