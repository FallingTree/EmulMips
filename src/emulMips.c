/**
 * @file emulMips.c
 * @author François Cayre, Nicolas Castagné, François Portet
 * @brief Main pour le début du projet émulateur MIPS.
 *
 */

//A modifier : execmd et loadcmd pour mettre en paramètre mem, symtab et pf_elf

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <limits.h>

#include "interpreteur.h"
#include "load.h"
#include "mem.h"


/**
 * Programme principal
 */
int main ( int argc, char *argv[] ) {


    /* exemples d'utilisation des macros du fichier notify.h */
    INFO_MSG("Un message INFO_MSG : Debut du programme %s", argv[0]); /* macro INFO_MSG */
    WARNING_MSG("Un message WARNING_MSG !"); /* macro INFO_MSG */
    DEBUG_MSG("Un message DEBUG_MSG !"); /* macro DEBUG_MSG : uniquement si compil en mode DEBUG_MSG */


    interpreteur inter=init_inter(); /* structure gardant les infos et états de l'interpreteur*/
    FILE *fp = NULL; /* le flux dans lequel les commande seront lues : stdin (mode shell) ou un fichier */
    mem memory=NULL;  // memoire virtuelle, c'est elle qui contiendra toute les données du programme
    reg registre[NB_REG]; //intialisation des registres
    stab symtab= new_stab(0); // table des symboles
    FILE * pf_elf=0; //Pointeur pour ouvrir le fichier elf
    
    
    //On initialise les registres
    registre[0].content=calloc(1,32);
    registre[0].name=strdup("zero");

     registre[1].content=calloc(1,32);
    registre[1].name=strdup("at");

     registre[2].content=calloc(1,32);
    registre[2].name=strdup("v0");

     registre[3].content=calloc(1,32);
    registre[3].name=strdup("v1");

     registre[4].content=calloc(1,32);
    registre[4].name=strdup("a0");

     registre[4].content=calloc(1,32);
    registre[4].name=strdup("a1");

     registre[5].content=calloc(1,32);
    registre[5].name=strdup("a2");

     registre[6].content=calloc(1,32);
    registre[6].name=strdup("a3");

     registre[7].content=calloc(1,32);
    registre[7].name=strdup("t0");

     registre[8].content=calloc(1,32);
    registre[8].name=strdup("t1");

     registre[9].content=calloc(1,32);
    registre[9].name=strdup("t2");

     registre[10].content=calloc(1,32);
    registre[10].name=strdup("t3");

     registre[11].content=calloc(1,32);
    registre[11].name=strdup("t4");

     registre[12].content=calloc(1,32);
    registre[12].name=strdup("t5");

     registre[13].content=calloc(1,32);
    registre[13].name=strdup("t6");

     registre[14].content=calloc(1,32);
    registre[14].name=strdup("t7");

     registre[15].content=calloc(1,32);
    registre[15].name=strdup("s0");

     registre[16].content=calloc(1,32);
    registre[16].name=strdup("s1");

     registre[17].content=calloc(1,32);
    registre[17].name=strdup("s2");

     registre[18].content=calloc(1,32);
    registre[18].name=strdup("s3");

     registre[19].content=calloc(1,32);
    registre[19].name=strdup("s4");

     registre[20].content=calloc(1,32);
    registre[20].name=strdup("s5");

     registre[21].content=calloc(1,32);
    registre[21].name=strdup("s6");

     registre[22].content=calloc(1,32);
    registre[22].name=strdup("s7");

     registre[23].content=calloc(1,32);
    registre[23].name=strdup("t8");

     registre[24].content=calloc(1,32);
    registre[24].name=strdup("t9");

     registre[25].content=calloc(1,32);
    registre[25].name=strdup("k0");

     registre[26].content=calloc(1,32);
    registre[26].name=strdup("k1");

     registre[27].content=calloc(1,32);
    registre[27].name=strdup("gp");

     registre[28].content=calloc(1,32);
    registre[28].name=strdup("sp");

     registre[29].content=calloc(1,32);
    registre[29].name=strdup("fp");

     registre[30].content=calloc(1,32);
    registre[30].name=strdup("ra");

     registre[31].content=calloc(1,32);
    registre[31].name=strdup("HI");

     registre[32].content=calloc(1,32);
    registre[32].name=strdup("L0");

     registre[33].content=calloc(1,32);
    registre[33].name=strdup("PC");



    //Vérification que la forme des paramètres du programme est bien respectée
    if ( argc > 2 ) {
        usage_ERROR_MSG( argv[0] );
        exit( EXIT_FAILURE );
    }
    if(argc == 2 && strcmp(argv[1], "-h") == 0) {
        usage_ERROR_MSG( argv[0] );
        exit( EXIT_SUCCESS );
    }

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

    /* boucle infinie : lit puis execute une cmd en boucle */
    while ( 1 ) {



        if (acquire_line( fp,  inter)  == 0 ) {
            /* Une nouvelle ligne a ete acquise dans le flux fp*/

            int res = execute_cmd(inter,&memory,registre,&symtab,pf_elf); /* execution de la commande */
	     
            // traitement des erreurs
            switch(res) {
            case 0:
		printf("La commande s'est exécutée correctement\n");
        
                break;
            case 1:
		WARNING_MSG("Les paramètres de la commande sont erronés.\nConsulter l'aide avec la commande help.\n");
        
	        break;
	    case -1:
                /* sortie propre du programme */
                if ( fp != stdin ) {
                    fclose( fp );
                }
                del_mem(memory);
                del_stab(symtab);
                del_inter(inter);
                exit(EXIT_SUCCESS);
                
                break;
	    case -2 :
                
		break;
            default:
                /* erreur durant l'execution de la commande */
                /* En mode "fichier" toute erreur implique la fin du programme ! */
                if (inter->mode == SCRIPT) {
                    fclose( fp );
                    del_inter(inter);
                    del_mem(memory);
                    del_stab(symtab);
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
            del_mem(memory);
            del_stab(symtab);
            exit(EXIT_SUCCESS);
        }
    }
    /* tous les cas de sortie du programme sont gérés plus haut*/
    ERROR_MSG("SHOULD NEVER BE HERE\n");
}

