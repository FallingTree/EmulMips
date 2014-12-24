//----------------------------------------------------------------------------------------------------------------  
// File : trouver.c 
// Authors : Ammar Mian, Ambre Davat
// Avec participation de François Cayre, Nicolas Castagné, François Portet
//
// Projet C Grenoble INP - Phelma 2A SICOM 2014 : Emulateur Mips
// Fichier contenant des fonctions de recherche dans la mémoire
//
//---------------------------------------------------------------------------------------------------------------- 



#include <stdlib.h>
#include <stdio.h>
#include "interpreteur.h"
#include "is_type.h"
 #include "elf/syms.h"
 #include "load.h"

//Cette fonction modifie les pointeurs p_i_text, p_adrtext et p_size_text pour obtenir le numéro du segment .text,
//son adresse et sa taille

int trouver_seg_text (pm_glob param, int* p_i_text, unsigned int * p_adrtext, unsigned int *p_size_text)
{
	mem memory = *(param.p_memory);

	//On récupère le numéro du segment .text dans la mémoire
	int i_text=0;
	if (memory->seg[i_text].name==NULL) 
	{
		WARNING_MSG("Les segments de la mémoire n'ont pas été chargés");
		return 1;
	}		
	while (strcmp(memory->seg[i_text].name,".text")!=0)
	{
		i_text=i_text+1;
		if (i_text>NB_SECTIONS) 
		{	WARNING_MSG("La mémoire ne contient pas de segment .text\n");
			return 1;
		}
	}

	//Récupération de la valeur de l'adresse du segment .text

	unsigned int adrtext;
	switch( SCN_WIDTH( memory->seg[i_text].attr ) ) 
	{
   	case 32 :
		adrtext = memory->seg[i_text].start._32;
        	break;
    	default :
        	return 1;
    	}

	*p_size_text = memory->seg[i_text].size._32 ; //On récupère la taille du segment .text

	*p_i_text = i_text;
	*p_adrtext = adrtext;
	return 0;
		
}

//Si une étiquette marque l'adresse passée en paramètre, on l'affiche

int trouver_etiquette (unsigned int adresse, pm_glob param, int* p_i_etiquette)
{
	stab symtab = *param.p_symtab;
	int i;
	int i_seg; //Numéro du segment auquel appartient l'étiquette
	unsigned int adr_seg ; //Adresse du segment auquel appartient l'étiquette
	
	for (i=0; i < (symtab.size) ; i++)
	{
		if (symtab.sym[i].type != file)
		{

			i_seg = symtab.sym[i].scnidx;
			adr_seg = (*(param.p_memory))->seg[i_seg].start._32;

			//printf("\n\n nom : %s   adresse_etiquette %x     numéro_seg %d\n",symtab.sym[i].name, symtab.sym[i].addr._32, i_seg);

			//printf("  %x    \n",symtab.sym[i].addr._32 + adr_seg);

			if ((symtab.sym[i].addr._32 + adr_seg - 0x00001000) == adresse)
			{
				*p_i_etiquette = i;
				return 1;
			}
		}			
	}

	return 0;
}

//Fonction qui permet de trouver le segment correspondant à une adresse donnée
int trouver_seg_adresse(uint32_t adresse,pm_glob param){
	int i;
	
	
	if (adresse < 0xff7fe000) {
		 for ( i= 3; i < (*(param.p_memory))->nseg-1; i++ ) {
			if ( (*(param.p_memory))->seg[i].start._32 <= adresse && (*(param.p_memory))->seg[i+1].start._32>adresse) return i;
		}
	
		//Cas où il s'agit du dernier segment
		if ( (*(param.p_memory))->seg[i+1].start._32<=adresse) return i+1;
	
	}
	
	// Cas d'un segment de la libc
	else {
		
		//Cas où il s'agit du premier segment de la libc
		if ( (*(param.p_memory))->seg[0].start._32<=adresse) return 0;

		for ( i= 0; i < 2; i++ ) {
			if ( (*(param.p_memory))->seg[i].start._32 >= adresse && (*(param.p_memory))->seg[i+1].start._32<adresse) {
			return i+1;
			}
		}
		

	}

	return -1; //Si ne se trouve pas dans la mémoire

	
}

//Fonction qui permet d'obtenir la valeur d'un mot dans un segment à partir de l'adresse virtuelle de ce mot
//Si la zone mémoire à laquelle la fonction essaye d'accéder n'est pas allouée, le mot de retour vaut 0.

word trouver_mot_adresse(int adresse, pm_glob param)
{
	mem memory = *(param.p_memory);//la mémoire

	//Rappel : memory->seg[i_text].content est un tableau qui (si un fichier elf a été chargé) contient les bytes de la section .text
	//Si l'adresse virtuelle du mot lu est 0x5000, alors les bytes du mot sont contenus dans les cases 0, 1, 2 et 3 du tableau .content.
	//Si l'adresse virtuelle du mot lu est 0x5004, alors les bytes du mot sont contenus dans les cases 4, 5, 6, 7 tableau .content.
	//...

	word motlu ;//le mot lu
	byte b1, b2, b3, b4 ; // les bytes constituant le mot

	int i_seg = trouver_seg_adresse(adresse, param); //le numéro du segment où se trouve le mot
	int position = adresse - memory->seg[i_seg].start._32; //Position du byte dans le segment
	int size_seg = memory->seg[i_seg].size._32; //Taille du segment calculé

	if (position >=size_seg) return 0;
	
	//Lecture des bytes
	b1 = memory->seg[i_seg].content[position] ;
	b2 = memory->seg[i_seg].content[position+1] ;
	b3 = memory->seg[i_seg].content[position+2] ;
	b4 = memory->seg[i_seg].content[position+3] ;

	//Calcul du mot (le Mips est un processeur de type big endian donc le byte de poids fort se trouve à l'adresse la plus basse)

	motlu = b4 + b3*0x100 + b2*0x10000 + b1*0x1000000;

	return motlu;

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


