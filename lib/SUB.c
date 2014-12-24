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
	int64_t val_s; //valeur sur 32bits signée
	int32_t a, b ; //Valeur signée sur 32bits

	a = registre[inst.rs].content ; //On impose l'interprétation des valeurs des registres comme entiers codés sur 32 bits
	b = registre[inst.rt].content ;			
	val_s = (int64_t) a - b;

	if ((val_s>INT32_MAX) || (val_s<INT32_MIN))
		WARNING_MSG("Attention, résultat non codable sur 32bits. Le registre n'est pas modifié.");
	else registre[inst.rd].content = val_s ;
	return 0;

}

int print(){
	printf("Fonction SUB\n");
	return 0;
}
