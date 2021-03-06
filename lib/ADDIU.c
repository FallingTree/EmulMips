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
	int32_t a ; //Valeur signée sur 32bits
	int16_t b; //Valeur signée sur 16bits

	a = registre[inst.rs].content ; //On impose l'interprétation des valeurs des registres comme entiers codés sur 32 bits
	b = inst.immediate ;	
	registre[inst.rt].content = (int32_t) a + b ;
	return 0;
}

int print(){
	printf("Fonction ADDIU\n");
	return 0;
}
