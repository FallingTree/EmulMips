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
	char * nom = inst.nom;
	long int val_s; //valeur sur 32bits signée
	int condition;
	unsigned int target_offset;
	int32_t a, b ; //Valeur signée sur 32bits

*p_jump = inst.target ;
return 0;
}

int print(){
	printf("Fonction J\n");
	return 0;
}