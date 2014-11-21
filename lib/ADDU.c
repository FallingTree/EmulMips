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

	registre[inst.rd].content = registre[inst.rs].content + registre[inst.rt].content ;
	return 0;
}

int print(){
	printf("Fonction ADDU\n");
	return 0;
}