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

int trouver_seg_text (pm_glob param, int* p_i_text, unsigned int * p_adrtext, unsigned int *p_size_text);
int trouver_etiquette (unsigned int adresse, pm_glob param, int* p_i_etiquette);
int trouver_seg_adresse(uint32_t adresse,pm_glob param);
word trouver_mot_adresse(int adresse, pm_glob param);
byte trouver_byte_adresse(int adresse, pm_glob param);


#endif /* _TROUVER_H_ */
