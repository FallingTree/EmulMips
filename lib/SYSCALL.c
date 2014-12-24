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



int exec(unsigned int* jump, pm_glob param, INST inst)
{

	unsigned int adresse, position;
	unsigned int * p_mot_lu;
	mem memory = *(param.p_memory);
        char* chaine;
	char chaine_lu[512];

	switch (param.p_registre[2].content) //On teste la valeur du registre $v0

	{	
		case 1 :

			printf("%u", param.p_registre[4].content);
			break;	
		
		case 4 :
			adresse = param.p_registre[4].content;
			int i_seg = trouver_seg_adresse(adresse,param);
			if ( i_seg == -1) printf("Erreur : Chaine non dans la zone mémoire\n");
			position = adresse - memory->seg[i_seg].start._32;
			chaine = (char*) &(memory->seg[i_seg].content[position]);
			printf("%s\n",chaine);
			break;	

		case 5 :
			p_mot_lu = &(param.p_registre[2].content);
			printf("Veuillez entrer un entier sur 32 bit : \n");
			scanf("%u", p_mot_lu);
			break;	
		
		case 8 :
			adresse = param.p_registre[4].content;
			i_seg = trouver_seg_adresse(adresse,param);
			if ( i_seg == -1) printf("Erreur : Adresse du buffer non dans la zone mémoire\n");
			position = adresse - memory->seg[i_seg].start._32;
			printf("Veuillez entrer une chaine de caractère : \n");
			scanf("%s", chaine_lu);
			strcpy((char*) &(memory->seg[i_seg].content[position]), chaine_lu);
			break;

		case 10 : 
			exit(-1);
			break;

		default :
			WARNING_MSG("Code système %d inconnu\n", param.p_registre[2].content);

			break;		

	
	}




	return 0;
}

int print()
{
	printf("Fonction SYSCALL\n");
	return 0;
}
