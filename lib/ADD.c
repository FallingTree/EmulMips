#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <limits.h>

#include "mem.h"
#include "is_type.h"
#include "common/notify.h"

int exec(unsigned int* jump, pm_glob param, INST inst){

	reg *registre = param.p_registre;
	char * nom = inst.nom;
	unsigned long int val;
	int condition;
	unsigned int target_offset;

	printf("Youpi !!!!!!!!!!!!!\n");
	val = registre[inst.rs].content + inst.immediate;

			if ((val>UINT_MAX)||(val<0))
				WARNING_MSG("Attention, résultat non codable sur 32bits. Le registre n'est pas modifié.");
			else registre[inst.rd].content = val ;
			return 0;
}

int print(){
	printf("Fonction ADD\n");
	return 0;
}