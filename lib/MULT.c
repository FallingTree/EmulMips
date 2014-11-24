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
	int64_t val_s; //valeur sur 64bits signée
	int32_t a, b ; //Valeur signée sur 32bits

	a = registre[inst.rs].content ; //On impose l'interprétation des valeurs des registres comme entiers codés sur 32 bits
	b = registre[inst.rt].content ;			
	val_s = a * b;

	registre[32].content = (int32_t) ((val_s & 0xffffffff00000000) >> 32);
	registre[33].content = (int32_t) (val_s & 0x00000000ffffffff);


	return 0;
}

int print(){
	printf("Fonction MULT\n");
	return 0;
}