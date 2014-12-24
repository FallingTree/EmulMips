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
	int16_t offset = inst.offset;			
	unsigned int adresse = base + offset;

	if (adresse%4!=0) 
	{	
		WARNING_MSG("L'adresse calculée n'est pas multiple de 4\n");		
		return 1;
	}

	int i_seg = trouver_seg_adresse(adresse,param);
	int i_adresse_mem = adresse - (*(param.p_memory))->seg[i_seg].start._32; //Indice de l'adresse calculée dans le segment
	int size_seg = (*(param.p_memory))->seg[i_seg].size._32; //Taille du segment calculé
	int dataword = registre[inst.rt].content;

	if (i_adresse_mem >= size_seg) 
	{					
		ERROR_MSG("Erreur : Adresse hors de la zone allouée\n");			
		return 1;
	}

	//Le Mips est un processeur de type big endian, donc l'octet de poids fort se trouve à l'adresse la plus basse

	(*(param.p_memory))->seg[i_seg].content[i_adresse_mem+3]=(dataword&0xff);
	(*(param.p_memory))->seg[i_seg].content[i_adresse_mem+2]=(dataword&0xff00)>>8;
	(*(param.p_memory))->seg[i_seg].content[i_adresse_mem+1]=(dataword&0xff0000)>>16;
	(*(param.p_memory))->seg[i_seg].content[i_adresse_mem]=(dataword&0xff000000)>>24;

	return 0;
}

int print(){
	printf("Fonction SW\n");
	return 0;
}
