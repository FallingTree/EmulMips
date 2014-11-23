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

	target_offset = registre[inst.offset].content + registre[34].content;
	a = registre[inst.rs].content; //On force l'interprétation du registre comme entier codé sur 32 bits				
	condition = (a > 0);
	if (condition) registre[34].content = registre[34].content + target_offset ;
	return 0;
}

int print(){
	printf("Fonction BGTZ\n");
	return 0;
}