#ifndef _INTERPRETEUR_H_
#define _INTERPRETEUR_H_
/* la librairie readline */
#include <readline/readline.h>
#include <readline/history.h>

/* macros de DEBUG_MSG fournies , etc */
#include "common/notify.h"

#include "elf/elf.h"
#include "elf/syms.h"
#include "mem.h"



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

/* type de token (exemple) */
enum {HEXA,INTEGER32,INTEGER8,INTEGER,OBJET,REG,RANGE,UNKNOWN};


/* mode d'interaction avec l'interpreteur (exemple)*/
typedef enum {INTERACTIF,SCRIPT,DEBUG_MODE} inter_mode;

/* structure passée en parametre qui contient la connaissance de l'état de
 * l'interpréteur 
 */
typedef struct {
    inter_mode mode;
    char input[MAX_STR];
    char * from;
    char first_token;
} *interpreteur;

interpreteur init_inter(void);
void del_inter(interpreteur inter);
char* get_next_token(interpreteur inter);
int is_hexa(char* chaine);
int is_integer32(char* chaine);
int is_integer8(char* chaine) ;
int is_objet(char* chaine);
int is_reg(char* chaine);
int is_range(char* chaine);
int get_type(char* chaine);
int _testcmd(int hexValue);
int testcmd(interpreteur inter);
int exitcmd(interpreteur inter);
int loadcmd(interpreteur inter,pm_glob param,FILE * pf_elf);
int dispcmd(interpreteur inter,pm_glob param);
int disasmcmd(interpreteur inter);
int setcmd(interpreteur inter) ;
int assertcmd (interpreteur inter);
void debugcmd (interpreteur inter);
void resumecmd (interpreteur inter);
void stepcmd (interpreteur inter);
int breakcmd(interpreteur inter);
void helpcmd(interpreteur inter);
int execute_cmd(interpreteur inter,pm_glob param,FILE * pf_elf);
void string_standardise( char* in, char* out );
int  acquire_line(FILE *fp, interpreteur inter);
void usage_ERROR_MSG( char *command );

#endif /* _INTERPRETEUR_H_ */