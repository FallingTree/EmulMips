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

	WARNING_MSG("Breakpoint");
	return 0;

int print(){
	printf("Fonction BREAK\n");
	return 0;
}