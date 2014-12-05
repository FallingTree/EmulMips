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

//Fonction qui permet de trouver le segment correspondant à une adresse donnée
int trouver_seg_adresse(int adresse,pm_glob param){
	int i;

	//Cas où l'adresse appartient aux segments non modifiables
	if (adresse >= 0xff7ff000) return -1;

	 for ( i= 0; i < (*(param.p_memory))->nseg-1; i++ ) {
		if ( (*(param.p_memory))->seg[i].start._32 <= adresse && (*(param.p_memory))->seg[i+1].start._32>adresse) return i;
	}
	
	//Cas où il s'agit du dernier segment
	if ( (*(param.p_memory))->seg[i+1].start._32<=adresse) return i+1;


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


