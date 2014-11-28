//----------------------------------------------------------------------------------------------------------------  
// File : liste.h
// Authors : Ammar Mian, Ambre Davat
// Avec participation de François Cayre, Nicolas Castagné, François Portet
//
// Projet C Grenoble INP - Phelma 2A SICOM 2014 : Emulateur Mips
// Définition des types d'instruction MIPS
//
//---------------------------------------------------------------------------------------------------------------- 


#ifndef _DICO_H_
#define _DICO_H_
#include "mem.h"



//Permet de se retrouver dans le tableau des variables des opérateurs
enum {RS=0,RT,RD,SA,IMMEDIATE,TARGET,OFFSET, BASE};


int visualiser_tab_instructions(Instruction* tab, int n);
int load_dico(Instruction** p_tab_instruction, char* nom_fichier, pm_glob param);



#endif /* _DICO_H_ */
