//----------------------------------------------------------------------------------------------------------------  
// File : trouver.h
// Authors : Ammar Mian, Ambre Davat
// Avec participation de François Cayre, Nicolas Castagné, François Portet
//
// Projet C Grenoble INP - Phelma 2A SICOM 2014 : Emulateur Mips
//
//---------------------------------------------------------------------------------------------------------------- 

#ifndef _TROUVER_H_
#define _TROUVER_H_


int trouver_seg_adresse(int adresse,pm_glob param);
word trouver_mot_adresse(int adresse, pm_glob param);
byte trouver_byte_adresse(int adresse, pm_glob param);


#endif /* _TROUVER_H_ */
