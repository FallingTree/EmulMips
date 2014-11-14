
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

int disasmcmd(interpreteur inter,pm_glob param);
unsigned int decode_instructions(pm_glob param);

#endif /* _DISASM_H_ */

