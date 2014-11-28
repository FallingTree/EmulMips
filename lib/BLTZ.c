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
	unsigned int target_offset;
	int32_t a ; //Valeur signée sur 32bits

	target_offset = inst.offset*4;
	a = registre[inst.rs].content; //On force l'interprétation du registre comme entier codé sur 32 bits				
	condition = (a < 0);
	if (condition) * p_jump = registre[34].content + target_offset ;
	return 0;
}

int print(){
	printf("Fonction BLTZ\n");
	return 0;
}
