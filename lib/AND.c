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
	char * nom = inst.nom;
	long int val_s; //valeur sur 32bits signée
	int condition;
	unsigned int target_offset;
	int32_t a, b ; //Valeur signée sur 32bits

	a = registre[inst.rs].content ; //On impose l'interprétation des valeurs des registres comme entiers codés sur 32 bits
	b = inst.immediate ;			
	val_s = a + b;

	registre[inst.rt].content = registre[inst.rs].content & inst.immediate ;
	return 0;
}

int print(){
	printf("Fonction AND\n");
	return 0;
}