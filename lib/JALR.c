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
	registre[inst.rd].content = registre[34].content + 8;
	*p_jump = registre[inst.rs].content ;
	return 0;
}

int print(){
	printf("Fonction JALR\n");
	return 0;
}
