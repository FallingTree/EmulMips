//----------------------------------------------------------------------------------------------------------------  
// File : interpreteur.h
// Authors : Ammar Mian, Ambre Davat
// Avec participation de François Cayre, Nicolas Castagné, François Portet
//
// Projet C Grenoble INP - Phelma 2A SICOM 2014 : Emulateur Mips
//
//---------------------------------------------------------------------------------------------------------------- 

#ifndef _INTERPRETEUR_H_
#define _INTERPRETEUR_H_
/* la librairie readline */
#include <readline/readline.h>
#include <readline/history.h>
#include <math.h>

/* macros de DEBUG_MSG fournies , etc */
#include "common/notify.h"

#include "elf/elf.h"
#include "elf/syms.h"
#include "mem.h"
#include "is_type.h"


/* prompt du mode shell interactif */
#define PROMPT_STRING "MipsShell : > "

/* taille max pour nos chaines de char */
#define MAX_STR 1024


/*************************************************************\
Valeur de retour speciales lorsqu'on pour la fonction
	int parse_and_execute_cmd_string(char *input) ;
Toute autre valeur signifie qu'une erreur est survenue
 \*************************************************************/
/*#define CMD_OK_RETURN_VALUE 0
#define CMD_EXIT_RETURN_VALUE -1
#define CMD_UNKOWN_RETURN_VALUE -2
*/



/* mode d'interaction avec l'interpreteur */
typedef enum {INTERACTIF,SCRIPT,DEBUG_MODE} inter_mode;

/* état de run */
typedef enum {NOT_STARTED, RUN, TERM, PAUSE} run_etat;


/* structure passée en parametre qui contient la connaissance de l'état de
 * l'interpréteur 
 */
typedef struct {
    inter_mode mode;
    run_etat etat; 
    char input[MAX_STR];
    char * from;
    char first_token;
} *interpreteur;

void decouper_word_hexa(char* chaine, byte** tab);
int trouver_seg_adresse(int adresse,pm_glob param);
word trouver_mot_adresse(int adresse, pm_glob param);
byte trouver_byte_adresse(int adresse, pm_glob param);
int convertir_string_add(char* string);
interpreteur init_inter(void);
void del_inter(interpreteur inter);
char* get_next_token(interpreteur inter);
int _testcmd(int hexValue);
int testcmd(interpreteur inter);
int exitcmd(interpreteur inter);
int loadcmd(interpreteur inter,pm_glob param,FILE * pf_elf);
int dispcmd(interpreteur inter,pm_glob param);
int setcmd(interpreteur inter,pm_glob param);
int assertcmd (interpreteur inter,pm_glob param);
void debugcmd (interpreteur inter);
void resumecmd (interpreteur inter);
int runcmd(interpreteur inter, pm_glob param);
int stepcmd (interpreteur inter, pm_glob param);
int breakcmd(interpreteur inter, pm_glob param);
void helpcmd(interpreteur inter);
int execute_cmd(interpreteur inter,pm_glob param,FILE * pf_elf);
void string_standardise( char* in, char* out );
int  acquire_line(FILE *fp, interpreteur inter);
void usage_ERROR_MSG( char *command );

#endif /* _INTERPRETEUR_H_ */
