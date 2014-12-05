#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <limits.h>

#include "emul.h"
#include "mem.h"
#include "is_type.h"
#include "trouver.h"
#include "common/notify.h"


int exec(unsigned int* jump, pm_glob param, INST inst){

	reg *registre = param.p_registre;

	unsigned int base = registre[inst.rs].content ; //On impose l'interprétation des valeurs des registres comme entiers codés sur 32 bits
	short int rt_val = (short) inst.offset;			
	unsigned int adresse = base + rt_val;

	registre[inst.rt].content =  trouver_byte_adresse( adresse, param); 

	return 0;
}

int print(){
	printf("Fonction LBU\n");
	return 0;
}
