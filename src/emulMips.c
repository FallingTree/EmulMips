//----------------------------------------------------------------------------------------------------------------  
// File : emulMips.c 
// Authors : Ammar Mian, Ambre Davat
// Avec participation de François Cayre, Nicolas Castagné, François Portet
//
// Projet C Grenoble INP - Phelma 2A SICOM 2014 : Emulateur Mips
// Fichier principal du projet
//
//---------------------------------------------------------------------------------------------------------------- 



#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <limits.h>

#include "interpreteur.h"
#include "load.h"
#include "mem.h"



int main ( int argc, char *argv[] ) {


    printf("Bienvenue dans l'Emulateur MIPS !\n");


//-------------------------------------- Vérification paramètres ------------------------------------------------ 

    //Vérification que la forme des paramètres du programme est bien respectée
    if ( argc > 2 ) {
        usage_ERROR_MSG( argv[0] );
        exit( EXIT_FAILURE );
    }
    if(argc == 2 && strcmp(argv[1], "-h") == 0) {
        usage_ERROR_MSG( argv[0] );
        exit( EXIT_SUCCESS );
    }

//--------------------------------------------------------------------------------------------------------------- 


//-------------------------------------- Initialisation ---------------------------------------------------------

    interpreteur inter=init_inter(); /* structure gardant les infos et états de l'interpreteur*/
    FILE *fp = NULL; /* le flux dans lequel les commande seront lues : stdin (mode shell) ou un fichier */
    pm_glob param;  // Paramètres globaux que l'on retrouvera comme argument de plusieurs fonctions
    
    mem memory=NULL; // memoire virtuelle, c'est elle qui contiendra toute les données du programme
    param.p_memory=&memory;  
    
    param.p_registre=calloc(NB_REG,sizeof(*(param.p_registre))); //intialisation des registres
    init_reg(param.p_registre);  
    
    stab symtab=new_stab(0);		// table des symboles
    param.p_symtab= &symtab; 
    FILE * pf_elf=0; //Pointeur pour ouvrir le fichier elf


    Liste_int_t liste_bp=creer_liste_int_t(); //Initilaisation de la liste des break points
    param.p_liste_bp=&liste_bp;
   // visualiser_liste_int_t(*(param.p_liste_bp)); debug
   
    

//Initialisation de l'interpréteur

    /*par defaut : mode shell interactif */
    fp = stdin;
    inter->mode = INTERACTIF;
    if(argc == 2) {
        /* mode fichier de commandes */
        fp = fopen( argv[1], "r" );
        if ( fp == NULL ) {
            perror( "fopen" );
            exit( EXIT_FAILURE );
            return 0;
        }
        inter->mode = SCRIPT;
    }


//----------------------------------------------------------------------------------------------------------------  

    /* boucle infinie : lit puis execute une cmd en boucle */
    while ( 1 ) {



        if (acquire_line( fp,  inter)  == 0 ) {
            /* Une nouvelle ligne a ete acquise dans le flux fp*/

            int res = execute_cmd(inter,param,pf_elf); /* execution de la commande */
	     
            // traitement des erreurs
            switch(res) {
            case 0:
		printf("La commande s'est exécutée correctement\n");
        
                break;

	    case -1:
                /* sortie propre du programme */
                if ( fp != stdin ) {
                    fclose( fp );
                }

                printf("Fermeture du programme : Au revoir !\n");

                del_mem(*(param.p_memory));
                del_stab(*(param.p_symtab));
                del_inter(inter);
                exit(EXIT_SUCCESS);
                
                break;
                
            default:

                WARNING_MSG("Erreur durant l'exécution de la commande, se référer à la commande help\n");
                /* erreur durant l'execution de la commande */
                /* En mode "fichier" toute erreur implique la fin du programme ! */
                if (inter->mode == SCRIPT) {
                    fclose( fp );
                    del_inter(inter);
                    del_mem(*(param.p_memory));
                    del_stab(*(param.p_symtab));
                    /*macro ERROR_MSG : message d'erreur puis fin de programme ! */
                    ERROR_MSG("ERREUR DETECTEE. Aborts");
                    
                }
                break;
            }
        }
        if( inter->mode == SCRIPT && feof(fp) ) {
            /* mode fichier, fin de fichier => sortie propre du programme */
            DEBUG_MSG("FIN DE FICHIER");
            fclose( fp );
            del_inter(inter);
            del_mem(*(param.p_memory));
            del_stab(*(param.p_symtab));
            exit(EXIT_SUCCESS);
        }
    }
    /* tous les cas de sortie du programme sont gérés plus haut*/
    ERROR_MSG("SHOULD NEVER BE HERE\n");
}

