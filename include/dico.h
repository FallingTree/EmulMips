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


//Définition des structures d'instructions en champ de bits
typedef struct { unsigned int funt:6, sp:5, rd:5, rt:5, rs:5, opcode:6;} SR;
typedef struct { unsigned int opcode:6, rs:5, rt:5, immediate:16;} SI;
typedef struct { unsigned int opcode:6, adress:26;} ST;

//Permet de se retrouver dans le tableau des variables des opérateurs
enum {RS=0,RT,RD,SA,IMMEDIATE,TARGET,OFFSET};


//Structure d'union pour contenir une instruction sous toutes ses formes possibles
typedef union {
	unsigned int mot;
	SR r;
	SI i;
	ST t;
	unsigned char octet[4];
} INST;


int visualiser_tab_instructions(Instruction* tab, int n);
int load_dico(Instruction** p_tab_instruction, char* nom_fichier, pm_glob param);



#endif /* _DICO_H_ */
