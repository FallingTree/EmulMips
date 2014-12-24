//----------------------------------------------------------------------------------------------------------------  
// File : interpreteur.h
// Authors : Ammar Mian, Ambre Davat
// Avec participation de François Cayre, Nicolas Castagné, François Portet
//
// Projet C Grenoble INP - Phelma 2A SICOM 2014 : Emulateur Mips
//
//---------------------------------------------------------------------------------------------------------------- 

#ifndef _EMUL_H_
#define _EMUL_H_

 #include "mem.h"

int emul (unsigned int* p_jump, pm_glob param, INST inst);

#endif /* _EMUL_H_ */
