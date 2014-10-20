//----------------------------------------------------------------------------------------------------------------  
// File : liste.c 
// Authors : Ammar Mian, Ambre Davat
// Avec participation de François Cayre, Nicolas Castagné, François Portet
//
// Projet C Grenoble INP - Phelma 2A SICOM 2014 : Emulateur Mips
// Ce fichier contient toutes les fonctions nécessaires pour manipuler une liste d'entier
//
//---------------------------------------------------------------------------------------------------------------- 

#include <stdio.h>
#include <stdlib.h>
#include "common/liste.h"



//Fonction affiche_int
//------------------------------------------------------------------------------------------------

//Affiche la valeur d'un int

void affiche_int(int i)
{
    printf("%08x\n",i);
}

//------------------------------------------------------------------------------------------------




// Fontion creer_liste_int_t
//------------------------------------------------------------------------------------------------

//Permet de créer une liste d'entiers

Liste_int_t creer_liste_int_t()
{
    return NULL;
}

//------------------------------------------------------------------------------------------------




// Fontion est_vide_int
//------------------------------------------------------------------------------------------------

//Permet de savoir si une liste est vide ou non

int est_vide_int(Liste_int_t l)
{
    return !l;
}

//------------------------------------------------------------------------------------------------




// Fontion visualiser_liste_int_t
//------------------------------------------------------------------------------------------------

//Permet de visualiser une liste d'entiers

void visualiser_liste_int_t(Liste_int_t l)
{

    int i=0;
    Liste_int_t p=l;
    if (est_vide_int(l))
    {
        printf("Pas de Break Points\n");
    }
    while (!est_vide_int(p))
    {
        printf("#%d 0x",i);
        affiche_int(p->val);  
        p=p->suiv;
        i++;
    }
	puts("");
}

//------------------------------------------------------------------------------------------------




// Fontion ajout_tete_int
//------------------------------------------------------------------------------------------------

//Permet d'ajouter un élément en tête de liste

Liste_int_t ajout_tete_int(int c, Liste_int_t l)
{
    Liste_int_t p=calloc(1, sizeof(*p));
    if (p==NULL)
    {
        return NULL;
    }
    p->val=c;
    p->suiv=l;
    return p;
}

//------------------------------------------------------------------------------------------------




// Fontion supprimer_tete_int
//------------------------------------------------------------------------------------------------

//Permet de supprimer la tête de la liste

Liste_int_t supprimer_tete_int(Liste_int_t l)
{
    Liste_int_t p;
    if (est_vide_int(l))
    {
        return NULL;
    }
    p=l->suiv;
    free(l);
    return p;
}

//------------------------------------------------------------------------------------------------




// Fontion ajout_queue_int
//------------------------------------------------------------------------------------------------

//Permet d'ajouter un élément en queue de la liste

Liste_int_t ajout_queue_int(int c, Liste_int_t l)
{
    Liste_int_t q=l;

	if (l==NULL)
	{
		 return ajout_tete_int(c,l);
    }

    while (q->suiv!=NULL)		//ou	for(q=l; q->suiv!=NULL; q=q->suiv) ;
    {
        q=q->suiv;
	}
    q->suiv=ajout_tete_int(c,q->suiv);
    return l;
}

//------------------------------------------------------------------------------------------------




// Fontion compter_elements_int
//------------------------------------------------------------------------------------------------

//Renvoie le nombre d'éléments de la liste

int compter_elements_int(Liste_int_t l)
{
    int i=0;
    Liste_int_t p=l;
    while (!est_vide_int(p))
    {
        i++;
        p=p->suiv;
    }

    return i;
}

//------------------------------------------------------------------------------------------------




// Fontion supprimen_int
//------------------------------------------------------------------------------------------------

//Permet de supprimer le nième élément de la liste


Liste_int_t supprimen_int(int n, Liste_int_t l)
{
    Liste_int_t p=l;
    Liste_int_t q;

    int i;

    if (n==0)
    {
        printf("Pas de position 0\n");
        return l;
    }

    if (compter_elements_int(l)<n)
    {
        printf("Erreur, n trop grand\n");
        return l;
    }

    if (est_vide_int(l))
     {
        printf("Impossible de supprimer : Liste_int_t Vide\n");
        return NULL;
     }   
    
    if (n==1) return supprimer_tete_int(l);


    for (i = 1; i < n-1 && !est_vide_int(p); ++i)
    {
        p=p->suiv;
    }

    if (!est_vide_int(p->suiv))
    { 
        q=p->suiv->suiv;
        free(p->suiv);
        p->suiv=q;

    }    
    return l;
}

//------------------------------------------------------------------------------------------------

//Supprime un break point de la liste si présent
//------------------------------------------------------------------------------------------------
Liste_int_t supprimer_break_point(Liste_int_t l, int adresse)
{
    Liste_int_t p=l;
    Liste_int_t q=l;
    Liste_int_t r;

    if (est_vide_int(l))
     {
        printf("Impossible de supprimer : Liste des break points vide\n");
     }   
    
    if (est_vide_int(p->suiv) && p->val==adresse)   //Si unique élément d'une liste
    {
        return NULL;
    }


    while(!est_vide_int(p) && !(p->val==adresse))   // On se place au bon endroit
    {

        p=p->suiv;
    }


    if (est_vide_int(p))
    { 
        printf("Impossible de supprimer : Liste des break points ne contient pas 0x%x !\n",adresse);
    } 

    else if (est_vide_int(p->suiv))      //Si dernier élément de la liste
    {
        free(p);
        while(!est_vide_int(q->suiv->suiv))
        {
         q=q->suiv;
        }
        q->suiv=NULL;

    }

    else {                          //Si élément intermédiaire de la liste
        r=p->suiv->suiv;
        free(p->suiv);
        p->suiv=r;
    }
       
    return l;
}

//------------------------------------------------------------------------------------------------

// Fontion supprimer_liste_bp
//------------------------------------------------------------------------------------------------

//Permet de supprimer la liste des break points

Liste_int_t supprimer_liste_bp(Liste_int_t l)
{
    Liste_int_t p=l;
    Liste_int_t q;

    if (est_vide_int(l))
    {
        return NULL;
    }

    

    while(!est_vide_int(p)){
    q=p->suiv;
    free(p);
    p=q;
    }

    return NULL;
}

//------------------------------------------------------------------------------------------------



// Fontion copie_int
//------------------------------------------------------------------------------------------------

//Fait une copie de la liste

Liste_int_t copie_int(Liste_int_t l1)
{
    Liste_int_t p=l1;
    Liste_int_t l2=creer_liste_int_t();
    while (p!=NULL)
    {
		l2=ajout_queue_int(p->val,l2);
		p=p->suiv;
    }
	return l2;
}

//------------------------------------------------------------------------------------------------



// Fontion
//------------------------------------------------------------------------------------------------

//Concatène deux listes

Liste_int_t concat_int(Liste_int_t l1, Liste_int_t l2)
{
	Liste_int_t l3;
	l3=copie_int(l1);    
	Liste_int_t p=l3;
	if (est_vide_int(l1))
	{
		return l2;
	}
    while (p->suiv!=NULL)
    {
        p=p->suiv;
    }
    p->suiv=l2;
    return l3;
}

//------------------------------------------------------------------------------------------------



// Fontion etre_dans_liste
//------------------------------------------------------------------------------------------------

//Permet de vérifier si un élément est bien dans la liste

int etre_dans_liste(int i,Liste_int_t l)
{
    Liste_int_t p=l;
    while (!est_vide_int(p))
    {
        if (p->val==i)
        {
            return 1;
        }

        p=p->suiv;
    }

    return 0;
}

//------------------------------------------------------------------------------------------------

