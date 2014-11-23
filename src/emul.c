


//----------------------------------------------------------------------------------------------------------------  
// File : interpreteur.h
// Authors : Ammar Mian, Ambre Davat
// Avec participation de François Cayre, Nicolas Castagné, François Portet
//
// Projet C Grenoble INP - Phelma 2A SICOM 2014 : Emulateur Mips
//
//---------------------------------------------------------------------------------------------------------------- 

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <limits.h>

 #include "emul.h"
 #include "mem.h"
 #include "is_type.h"
 #include "common/notify.h"

int emul (unsigned int * p_jump, pm_glob param, INST inst)
{
	reg *registre = param.p_registre;
	char * nom = inst.nom;
	long int val_s; //valeur sur 32bits signée
	int condition;
	unsigned int target_offset;
	int32_t a, b ; //Valeur signée sur 32bits
	int i,load_fonction=0;


	for (i = 0; i < *(param.p_nb_instr); ++i)
	{
		//printf("instruction %s et dans le tableau %s \n",nom,(*(param.p_tab_instructions))[i].nom);
		if (nom==(*(param.p_tab_instructions))[i].nom)
		{
			// printf("Youpi !\n");//debug
			load_fonction = ((*(param.p_tab_instructions))[i].fonction)(p_jump,param,inst);
			if (load_fonction)
			{
				printf("Erreur : instruction %s non exéctuée\n",nom);
				return 0;
			}

			return 1;
		}
		
	}

	printf("Erreur : instruction %s non exéctuée\n",nom);
	return 0;

}

