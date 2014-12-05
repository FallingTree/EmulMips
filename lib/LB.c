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


//Fonction qui permet d'obtenir la valeur d'un byte dans un segment à partir de l'adresse virtuelle de ce byte
//Si la zone mémoire à laquelle la fonction essaye d'accéder n'est pas allouée, le byte de retour vaut 0.

byte trouver_byte_adresse(int adresse, pm_glob param)
{
	mem memory = *(param.p_memory);//la mémoire
	byte bylu ;//le byte lu
	int i_seg = trouver_seg_adresse(adresse, param); //le numéro du segment où se trouve le byte
	int position = adresse - memory->seg[i_seg].start._32; //Position du byte dans le segment
	int size_seg = memory->seg[i_seg].size._32; //Taille du segment calculé

	if (position >=size_seg) bylu = 0;
	else bylu = memory->seg[i_seg].content[position] ;

	return bylu;

}


int exec(unsigned int* jump, pm_glob param, INST inst){

	reg *registre = param.p_registre;


	unsigned int base = registre[inst.rs].content ; //On impose l'interprétation des valeurs des registres comme entiers codés sur 32 bits
	short int rt_val = (short) inst.offset;			
	unsigned int adresse = base + rt_val;

	registre[inst.rt].content =  trouver_byte_adresse( adresse, param); 


	return 0;
}

int print(){
	printf("Fonction LB\n");
	return 0;
}
