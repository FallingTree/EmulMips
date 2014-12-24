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
#include "dico.h"
#include "disasm.h"
#include "emul.h"
#include "trouver.h"

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

//Allocation de l'interpréteur

    interpreteur inter=init_inter(); /* structure gardant les infos et états de l'interpreteur*/
    FILE *fp = NULL; /* le flux dans lequel les commande seront lues : stdin (mode shell) ou un fichier */
    pm_glob param;  // Paramètres globaux que l'on retrouvera comme argument de plusieurs fonctions
    
    mem memory=NULL; // memoire virtuelle, c'est elle qui contiendra toute les données du programme
    param.p_memory=&memory;  
    
    param.p_registre=calloc(NB_REG,sizeof(*(param.p_registre))); //intialisation des registres
    init_reg(param.p_registre); 
    reg *registre = param.p_registre; 
    
    stab symtab=new_stab(0);		// table des symboles
    stab symtab_libc=new_stab(0);	// table des symboles de la libc
    param.p_symtab= &symtab;
    param.p_symtab_libc= &symtab_libc; 

    FILE * pf_elf=0; //Pointeur pour ouvrir le fichier elf


    Liste_int_t liste_bp=creer_liste_int_t(); //Initialisation de la liste des break points
    param.p_liste_bp=&liste_bp;
   // visualiser_liste_int_t(*(param.p_liste_bp)); debug
   
   int nb_instr;
   param.p_nb_instr =&nb_instr;

   unsigned int last_disasm = 0;
   param.p_last_disasm = &last_disasm;

   //Chargement du dictionnaire
   Instruction* tab_instructions;
   param.p_tab_instructions=&tab_instructions;
   int load_ok;
   load_ok=load_dico(param.p_tab_instructions,"dictionnaire.txt", param);



//Initialisation de l'interpréteur

    /*par defaut : mode shell interactif */
    fp = stdin;
    inter->mode = INTERACTIF; 
    inter->etat = NOT_STARTED;
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




//Paramètres nécessaires à l'exécution du programme (run)

	int resu_emul; //Valeur de retour de la fonction emul

	int i_text=0; //Indice du segment .text dans la mémoire
	unsigned int adrtext = 0; //Adresse du segment .text dans la mémoire
	unsigned int size_text; //Taille du segment .text

   	INST instruction; //Instruction suivante du programme

	word motlu ;//L'instruction lue
	unsigned int jump; //Adresse de saut

	//Tests aux étages du pipeline
	int i_if ;
	int i_id ;
	int i_ex ;

//----------------------------------------------------------------------------------------------------------------  

    /* boucle infinie : lit puis execute une cmd en boucle */
    while ( 1 ) {

	//Exécution du programme en cours
	switch (inter->etat)
	{
		
		case NOT_STARTED :

			i_if = 0;
			i_id = 0;
			i_ex = 0;



			if (last_disasm==0) printf("Charger un fichier elf\n");

			else
			{	
				if (trouver_seg_text (param, &i_text, &adrtext, &size_text) != 0) return 1;		
				param.p_registre[34].content = adrtext;
			}	
			
			break;
	
		case RUN : //Etat d'éxécution de la fonction run

			while (inter->etat==RUN)
			{	
				jump = 0;

				DEBUG_MSG("pc : %x \n", registre[34].content);
				
				//---------PIPELINE SIMPLIFIE---------//


				//Exécution de l'instruction
				if (i_id)
				{	
					resu_emul = emul(&jump, param, instruction);
					if (!resu_emul) inter->etat = PAUSE;
					else
					{
						if (!jump) registre[34].content =registre[34].content + 4; //incrémentation du registre pc	
						i_ex = 1;
						DEBUG_MSG("Instruction exécutée : %s \n", instruction.nom);
					}				
				}

				//Désassemblage de l'instruction
				if (i_if) 
				{
					decode_instruction(motlu, &instruction, param);
					i_id = 1;
					DEBUG_MSG("Instruction désassemblée : %s\n", instruction.nom);

					if (jump)
					{
						emul(&jump, param, instruction);
						registre[34].content = jump; //Saut à l'adresse calculée par l'instruction de saut	
					}
				}

				//Extraction de l'instruction
					
					//On lit l'instruction à l'adresse contenue dans le registre pc.

					if (! i_if) motlu = trouver_mot_adresse(registre[34].content, param);
						//Lecture de la première instruction

					else if (registre[34].content > last_disasm + 4)
						motlu = 0; //Cas où il n'y a plus d'instruction à lire

					else motlu = trouver_mot_adresse(registre[34].content + 4, param);
 						//La valeur de pc est celle de l'instruction désassemblée, on lit l'instruction suivante.

					i_if = 1;

					DEBUG_MSG("Instruction extraite : %x\n\n", motlu);

				if (etre_dans_liste(param.p_registre[34].content,*(param.p_liste_bp))) 
				{
					WARNING_MSG("Breakpoint à l'adresse %08x",param.p_registre[34].content);
					inter->etat = PAUSE; 		//Si l'adresse de PC est un point d'arrêt, pause.	
				}
	
				if (registre[34].content > last_disasm)  //Si on a atteint la fin du segment .text, terminer.	
				{
					DEBUG_MSG("Pc = %x - Exécution du programme terminée\n", registre[34].content);
					inter->etat = TERM;
				}								

	
			}
			break;
				
		case RUN_PROCEDURE : //Etat d'éxécution de la fonction step

			jump = 0;
			while ((inter->etat==RUN_PROCEDURE)&&(!jump))
			{	
				
				DEBUG_MSG("pc : %x \n", registre[34].content);

				//---------PIPELINE SIMPLIFIE---------//


				//Exécution de l'instruction
				if (i_id)
				{	
					resu_emul = emul(&jump, param, instruction);
					if (!resu_emul) inter->etat = PAUSE;
					else
					{
						if (!jump) registre[34].content =registre[34].content + 4; //incrémentation du registre pc	
						i_ex = 1;
						DEBUG_MSG("Instruction exécutée : %s \n", instruction.nom);
					}
				}

				//Désassemblage de l'instruction
				if (i_if) 
				{
					decode_instruction(motlu, &instruction, param);
					i_id = 1;
					DEBUG_MSG("Instruction désassemblée : %s\n", instruction.nom);

					if (jump)
					{
						emul(&jump, param, instruction);
						registre[34].content = jump; //Saut à l'adresse calculée par l'instruction de saut	
					}
				}

				//Extraction de l'instruction
					
					//On lit l'instruction à l'adresse contenue dans le registre pc.

					if (! i_if) motlu = trouver_mot_adresse(registre[34].content, param);
						//Lecture de la première instruction
					else motlu = trouver_mot_adresse(registre[34].content + 4, param);
 						//La valeur de pc est celle de l'instruction désassemblée, on lit l'instruction suivante.

					i_if = 1;

					DEBUG_MSG("Instruction extraite : %x\n\n", motlu);	

				if (registre[34].content > last_disasm)  //Si on a atteint la fin du segment .text, terminer.	
				{
					DEBUG_MSG("Pc = %x - Exécution du programme terminée\n", registre[34].content);
					inter->etat = TERM;
				}
		
			}

			inter->etat = PAUSE;
			break;

		case RUN_1 : //Etat d'éxécution de la fonction step into

			i_ex = 0;
			jump = 0;
			while (! i_ex)
			{
				DEBUG_MSG("Exécution d'une instruction\n");
								jump = 0;

				DEBUG_MSG("pc : %x \n", registre[34].content);

				
				//---------PIPELINE SIMPLIFIE---------//


				//Exécution de l'instruction
				if (i_id)
				{		
					resu_emul = emul(&jump, param, instruction);
					if (!resu_emul) inter->etat = PAUSE;
					else
					{
						if (!jump) registre[34].content =registre[34].content + 4; //incrémentation du registre pc	
						i_ex = 1;
						DEBUG_MSG("Instruction exécutée : %s \n", instruction.nom);
					}
				}

				//Désassemblage de l'instruction
				if (i_if) 
				{
					decode_instruction(motlu, &instruction, param);
					i_id = 1;
					DEBUG_MSG("Instruction désassemblée : %s\n", instruction.nom);

					if (jump)
					{
						emul(&jump, param, instruction);
						registre[34].content = jump; //Saut à l'adresse calculée par l'instruction de saut	
					}
				}

				//Extraction de l'instruction
					
					//On lit l'instruction à l'adresse contenue dans le registre pc.

					if (! i_if) motlu = trouver_mot_adresse(registre[34].content, param);
						//Lecture de la première instruction
					else motlu = trouver_mot_adresse(registre[34].content + 4, param);
 						//La valeur de pc est celle de l'instruction désassemblée, on lit l'instruction suivante.

					i_if = 1;

					DEBUG_MSG("Instruction extraite : %x\n\n", motlu);					

			}
	
			if (registre[34].content > last_disasm)  //Si on a atteint la fin du segment .text, terminer.	
			{
				DEBUG_MSG("Pc = %x - Exécution du programme terminée\n", registre[34].content);
				inter->etat = TERM;
			}

			else inter->etat = PAUSE;
			break;
			
		case PAUSE :
			DEBUG_MSG("Programme en pause\n");
			break;
		
		case TERM :

			
			break;

	}
	




	//Exécution des commandes de l'interpréteur

        if (acquire_line( fp,  inter)  == 0 ) {
            /* Une nouvelle ligne a ete acquise dans le flux fp*/

            int res = execute_cmd(inter,param,pf_elf); /* execution de la commande */


		//stab32_print( symtab );

            // traitement des erreurs
            switch(res) {
            case 0:
		printf("La commande s'est exécutée correctement\n\n");
        
                break;

	    case -1:
                /* sortie propre du programme */
                if ( fp != stdin ) {
                    fclose( fp );
                }

                printf("Fermeture du programme : Au revoir !\n\n");

                del_mem(*(param.p_memory));
                del_stab(*(param.p_symtab));
                del_inter(inter);
                exit(EXIT_SUCCESS);
                
                break;
                
            default:

                WARNING_MSG("Erreur durant l'exécution de la commande, se référer à la commande help\n\n");
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

