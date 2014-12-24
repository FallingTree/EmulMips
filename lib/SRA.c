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
	int signe = (registre[inst.rt].content & 0x80000000) >> 31 ; //Bit du signe
	uint32_t ajout;

	if (signe==0) registre[inst.rd].content = (registre[inst.rt].content >> inst.sa) ; 
			//Si le bit de signe est 0, le SRA est équivalent à un SRL
 	else
	{
		ajout = 0xffffffff << (32 - inst.sa) ;
		registre[inst.rd].content = (registre[inst.rt].content >> inst.sa) | ajout ; 
			//On ajoute le bit de signe dans les bits laissés vides par le décalage

	}
	return 0;
}

int print(){
	printf("Fonction SRA\n");
	return 0;
}
