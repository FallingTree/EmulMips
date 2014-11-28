#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <limits.h>

#include "emul.h"
#include "mem.h"
#include "is_type.h"
#include "common/notify.h"

//Fonction qui permet de trouver le segment correspondant à une adresse donnée
int trouver_seg_adresse(int adresse,pm_glob param){
	int i;

	 for ( i= 0; i < (*(param.p_memory))->nseg-1; i++ ) {
		if ( (*(param.p_memory))->seg[i].start._32 <= adresse && (*(param.p_memory))->seg[i+1].start._32>adresse) return i;
	}
	
	//Cas où il s'agit du dernier segment
	if ( (*(param.p_memory))->seg[i+1].start._32<=adresse) return i+1;

	return -1; //Si ne se trouve pas dans la mémoire
}

int exec(unsigned int* jump, pm_glob param, INST inst){

	reg *registre = param.p_registre;


	unsigned int base = registre[inst.rs].content ; //On impose l'interprétation des valeurs des registres comme entiers codés sur 32 bits
	byte rt_val = (registre[inst.rt].content & 0x000000ff);
	short int off_val = (short) inst.offset;			
	unsigned int adresse = base + off_val;

	int seg = trouver_seg_adresse(adresse,param);

	byte * p_content_mem = (*(param.p_memory))->seg[seg].content + (adresse - (*(param.p_memory))->seg[seg].start._32);

	* p_content_mem = rt_val;

	return 0;
}

int print(){
	printf("Fonction SB\n");
	return 0;
}