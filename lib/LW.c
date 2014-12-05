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

	byte base = registre[inst.rs].content ; //On impose l'interprétation des valeurs des registres comme entiers codés sur 32 bits
	short int off = (short int) inst.offset;			
	unsigned int adresse = base + off;

	if (adresse%4!=0) 
	{	
		WARNING_MSG("L'adresse calculée n'est pas multiple de 4\n");		
		return -1;
	}

	byte octect[4];
	int i_seg = trouver_seg_adresse(adresse, param); //Indice du segment qui contient l'adresse calculée
	int i_adresse_mem = adresse - (*(param.p_memory))->seg[i_seg].start._32; //Indice de l'adresse calculée dans le segment
	int size_seg = (*(param.p_memory))->seg[i_seg].size._32; //Taille du segment calculé

	//On considère que les parties vides de la mémoire ont la valeur 0
	if (i_adresse_mem >= size_seg) 
	{	
		registre[inst.rt].content = 0;
		return 0;
	}

	byte * p_content_mem = (*(param.p_memory))->seg[i_seg].content + (i_adresse_mem);

	octect[0] = *p_content_mem;
	octect[1] = * (p_content_mem + 1);
	octect[2] = * (p_content_mem + 2);
	octect[3] = * (p_content_mem + 3);

	printf("!!!\n");

	word mot= (octect[0] >> 24) + (octect[1] >> 16) + (octect[2] >> 8) + octect[0];


	registre[inst.rt].content = (int) mot;

	return 0;
}

int print(){
	printf("Fonction LW\n");
	return 0;
}
