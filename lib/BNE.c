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
	int condition;
	int32_t target_offset;
	target_offset = inst.offset*4;
	printf("%d", target_offset);

	condition = (registre[inst.rs].content != registre[inst.rt].content);
	if (condition) * p_jump = registre[34].content + target_offset ;
	return 0;
}

int print(){
	printf("Fonction BNE\n");
	return 0;
}
