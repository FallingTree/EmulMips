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

	unsigned int base = registre[inst.rs].content ; //On impose l'interprétation des valeurs des registres comme entiers non signés codés sur 32 bits
	int16_t offset = inst.offset;			
	unsigned int adresse = base + offset;

	int32_t rt_val = registre[inst.rt].content ;

	printf("%x\n", base);
	printf("%x\n", offset);
	printf("%x\n", adresse);

	int i_seg = trouver_seg_adresse(adresse,param);
	int i_adresse_mem = adresse - (*(param.p_memory))->seg[i_seg].start._32; //Indice de l'adresse calculée dans le segment
	int size_seg = (*(param.p_memory))->seg[i_seg].size._32; //Taille du segment calculé


	if (i_adresse_mem >= size_seg) 
	{					
		WARNING_MSG("Erreur : Adresse hors de la zone allouée\n");			
		return 1;
	}



	byte * p_content_mem = (*(param.p_memory))->seg[i_seg].content + (i_adresse_mem);

	* p_content_mem = rt_val;

	return 0;
}

int print(){
	printf("Fonction SB\n");
	return 0;
}
