//----------------------------------------------------------------------------------------------------------------  
// File : liste.h
// Authors : Ammar Mian, Ambre Davat
// Avec participation de François Cayre, Nicolas Castagné, François Portet
//
// Projet C Grenoble INP - Phelma 2A SICOM 2014 : Emulateur Mips
// Définition des listes d'entiers
//
//---------------------------------------------------------------------------------------------------------------- 

#ifndef _LISTE_H_
#define _LISTE_H_

//Liste d'entiers

typedef struct maillon_int {
    int val;
    struct maillon_int* suiv;
} Maillon_int;

typedef Maillon_int* Liste_int_t;


void affiche_int(int c);
Liste_int_t creer_liste_int_t(void);
int est_vide_int(Liste_int_t l);
void visualiser_liste_int_t(Liste_int_t l);
Liste_int_t ajout_tete_int(int c, Liste_int_t l);
Liste_int_t supprimer_tete_int(Liste_int_t l);
Liste_int_t ajout_queue_int(int c, Liste_int_t l);
Liste_int_t supprimen_int(int n, Liste_int_t l);
Liste_int_t supprimer_break_point(Liste_int_t l, int adresse);
Liste_int_t supprimer_liste_bp(Liste_int_t l);
int compter_elements_int(Liste_int_t l);
int etre_dans_liste(int i,Liste_int_t l);

#endif /* _LISTE_H_ */
