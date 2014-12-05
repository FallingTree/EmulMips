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
	byte rt_val = (registre[inst.rt].content & 0x000000ff);
	short int off_val = (short) inst.offset;			
	unsigned int adresse = base + off_val;

	int i_seg = trouver_seg_adresse(adresse,param);
	int i_adresse_mem = adresse - (*(param.p_memory))->seg[i_seg].start._32; //Indice de l'adresse calculée dans le segment
	int size_seg = (*(param.p_memory))->seg[i_seg].size._32; //Taille du segment calculé

	byte * content_etendu;
	int i;



	if (i_adresse_mem >= size_seg) 
	{	
					
				
		WARNING_MSG("Attention : Adresse hors de la zone allouée\n Allocation des bytes manquants\n");

		content_etendu = calloc(i_adresse_mem, sizeof(byte)); //Allocation du contenu étendu
		
		//Les premiers bytes du tableau sont ceux déjà contenus dans la mémoire	
		for(i=0 ; i<size_seg ; i++)
		{
			content_etendu[i] = (*(param.p_memory))->seg[i_seg].content[i];
		}					

		//On initialise à 0 les bytes suivants
		for(i=size_seg ; i<=i_adresse_mem ; i++)
		{
			content_etendu[i] = 0;
		}
		
		(*(param.p_memory))->seg[i_seg].content = content_etendu;
				

	}

	byte * p_content_mem = (*(param.p_memory))->seg[i_seg].content + (i_adresse_mem);

	* p_content_mem = rt_val;

	return 0;
}

int print(){
	printf("Fonction SB\n");
	return 0;
}
