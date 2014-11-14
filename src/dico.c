//----------------------------------------------------------------------------------------------------------------  
// File : dico.c 
// Authors : Ammar Mian, Ambre Davat
// Avec participation de François Cayre, Nicolas Castagné, François Portet
//
// Projet C Grenoble INP - Phelma 2A SICOM 2014 : Emulateur Mips
// Fichier contenant les fonctions relatives à la lecture du dictionn
//
//---------------------------------------------------------------------------------------------------------------- 



#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "dico.h"


//Permet d'afficher la table chargée en mémoire
int visualiser_tab_instructions(Instruction* tab, int n)
{
	int i;
	int j;

	printf("---------------------- Tableau des instructions -------------------------------\n \n");

	if (tab==NULL)
	{
		printf("Erreur : Table vide\n");
		printf("---------------------------------------------------------------------------------\n");
		return 0;
	}

	for (i = 0; i<n; ++i)
	{
		printf("%s : \n Masque : %8x \n Mnémonique : %8x\n Type : %s \n",tab[i].nom,tab[i].masque,tab[i].mnemonique,tab[i].type);
		printf(" Zones utilisées : ");
		for (j = 0; j < 7; ++j)
		{
			printf("%d ",tab[i].var_op[j]);
		}
		printf("\n Nombre d'opérandes : %d\n \n",tab[i].nb_operandes);
	}

	printf("--------------------------------------------------------------------------------\n");

	return 1;
}



int load_dico(Instruction** p_tab_instruction, char* nom_fichier, pm_glob param){

	int i=0;
	int nb_instructions=0;
	char chaine_lue[512];

	//On ouvre le fichier contenant le dictionnaire
	FILE* dico=fopen(nom_fichier,"r");

	char nom[512];
	unsigned int masque;
	unsigned int mnemonique;
	char type[512];
	int nb_operandes;

	
	
	//Gestion d'erreur : Si le dico ne s'est pas ouvert
	if(!dico)
	{
		printf("Erreur d'ouverture du dictionnaire d'instructions !\n");
		return 0;
	}

	//On saute la première ligne du dico
	fgets(chaine_lue,40,dico);
	//printf("Chaine lue : %s",test); //debug

	//Lecture du nombre d'instructions
	fgets(chaine_lue, 30, dico);
	//printf("Chaine lue : %s",chaine_lue); //debug
	sscanf(chaine_lue,"%*s %*s %*s %d",&nb_instructions);
	//printf("Nombre d'instructions : %d\n",nb_instructions); //debug
	*p_tab_instruction=calloc(nb_instructions,sizeof(**p_tab_instruction));

	param.nb_instr = nb_instructions;

	// On saute la 3e ligne
	fgets(chaine_lue, 100, dico);
	fgets(chaine_lue, 100, dico);
	fgets(chaine_lue, 100, dico);
	//printf("Chaine lue : %s",chaine_lue); //debug


	//On lit les instructions
	while (i<nb_instructions){

		//On alloue la mémoire dans la table des instructions pour le tableau var_op
		(*p_tab_instruction)[i].var_op=calloc(6,sizeof(*((*p_tab_instruction)[i].var_op)));

		fgets(chaine_lue, 100, dico);
		//printf("Chaine lue : %s",chaine_lue); //debug

		sscanf(chaine_lue,"%s %x %x %s %d %d %d %d %d %d %d %d",nom,&masque,&mnemonique,type,(*p_tab_instruction)[i].var_op+RS,(*p_tab_instruction)[i].var_op+RT,(*p_tab_instruction)[i].var_op+RD,(*p_tab_instruction)[i].var_op+SA,(*p_tab_instruction)[i].var_op+IMMEDIATE,(*p_tab_instruction)[i].var_op+TARGET,(*p_tab_instruction)[i].var_op+OFFSET,&nb_operandes);


		//printf("%s %x %x %s %d %d %d %d %d %d\n",nom,masque,mnemonique,type,(*p_tab_instruction)[i].var_op[RS],(*p_tab_instruction)[i].var_op[RT],(*p_tab_instruction)[i].var_op[RD],(*p_tab_instruction)[i].var_op[SA],(*p_tab_instruction)[i].var_op[IMMEDIATE],(*p_tab_instruction)[i].var_op[ADDRESS]);

		(*p_tab_instruction)[i].nom=strdup(nom);
		printf("%s\n",(*p_tab_instruction)[i].nom); //debug
		(*p_tab_instruction)[i].masque=masque;
		(*p_tab_instruction)[i].mnemonique=mnemonique;
		(*p_tab_instruction)[i].type=strdup(type);
		(*p_tab_instruction)[i].nb_operandes=nb_operandes;

		i++;
	}
	

	//visualiser_tab_instructions(*p_tab_instruction,nb_instructions); //debug
	
	
	fclose(dico);

	return 1;
}
