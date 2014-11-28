
//----------------------------------------------------------------------------------------------------------------  
// File : disasm.h 
// Authors : Ammar Mian, Ambre Davat
// Avec participation de François Cayre, Nicolas Castagné, François Portet
//
// Projet C Grenoble INP - Phelma 2A SICOM 2014 : Emulateur Mips
// Fichier contenant les fonctions utiles à l'utilisation de l'interpréteur ainsi que les commandes de celui-ci
//
//---------------------------------------------------------------------------------------------------------------- 

#ifndef _DISASM_H_
#define _DISASM_H_

#include "mem.h"
#include "dico.h"
#include "interpreteur.h"

int trouver_seg_text (pm_glob param, int* p_i_text, unsigned int * p_adrtext, unsigned int *p_size_text);
int trouver_etiquette (unsigned int adresse, pm_glob param, int* p_i_etiquette);
int disasmcmd(interpreteur inter,pm_glob param);
int decode_instruction(word motlu, INST* p_instruction_disasm, pm_glob param);

#endif /* _DISASM_H_ */

