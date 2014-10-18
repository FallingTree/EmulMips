
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <limits.h>

 #include "interpreteur.h"
 #include "load.h"



/**
 * allocation et init interpreteur
 * @return un pointeur vers une structure allouée dynamiquement
 */
interpreteur init_inter(void) {
    interpreteur inter = calloc(1,sizeof(*inter));
    if (inter ==NULL)
        ERROR_MSG("impossible d'allouer un nouvel interpreteur");
    return inter;
}

/**
 * desallocation de l'interpreteur
 * @param inter le pointeur vers l'interpreteur à libérer
 */
void del_inter(interpreteur inter) {
		if (inter !=NULL)
    	free(inter);
}

/** 
 * return le prochain token de la chaine actuellement
 * analysée par l'interpreteur
 * La fonction utilise une variable interne de l'interpreteur
 * pour gérer les premiers appels a strtok
 * @inter le pointeur vers l'interpreteur
 * @return un pointeur vers le token ou NULL
 */ 
char* get_next_token(interpreteur inter) {

  char       *token = NULL;
  char       *delim = " \t\n";
  
  if ( inter->first_token == 0 ) {
    token = strtok_r( inter->input, delim, &(inter->from) ); 
    inter->first_token = 1;
  }
  else {
    token = strtok_r( NULL, delim, &(inter->from) );
  }

  if ( NULL == token ) {
    inter->first_token = 0;
  }

  return token;
}




 /* teste si un token est une valeur hexa
 * 
 *@param chaine : le token à analyser
 *@return 0 si non-hexa, non null autrement
 */ 
int is_hexa(char* chaine) {
    int i=2;
    if (!(chaine!=NULL && strlen(chaine)>2&& chaine[0]=='0' && chaine[1]=='x')) return 0;
    else 
	{	while (chaine[i]!='\0')
			{	
				if ( !(chaine[i]>='0'&& chaine[i] <='9')
					&& !(chaine[i]>='A'&& chaine[i]<='F')
					&& !(chaine[i]>='a'&& chaine[i]<='f')) return 0;
				i++;
			}
		return  1;
	}
}

 /* teste si un token est un entier signé sur 32 bits
 * 
 *@param chaine : le token à analyser
 *@return 0 si non integer32, non null autrement
 */ 
int is_integer32(char* chaine) 
{	char ** endptr = NULL;
	long int val = strtol(chaine,endptr,10);
    	if (chaine!=NULL && strlen(chaine)>2&& chaine[0]=='0' && chaine[1]=='x') return 0; //on élimine les hexadécimaux
	return ((val<=INT_MAX)&&(val>=INT_MIN));
	
}


 /* teste si un token est un entier signé sur 8 bits
 * 
 *@param chaine : le token à analyser
 *@return 0 si non integer8, non null autrement
 */
 
int is_integer8(char* chaine) 
{
	int i=1;
	if (chaine[0]!='-'&&( !(chaine[i]>='0'&& chaine[i] <='9'))) return 0; //Le premier symbole peut être un - ou un entier
	while (chaine[i]!='\0')
	{	
		if ( !(chaine[i]>='0'&& chaine[i] <='9')) return 0;
		i++;
	}
	if (i>4) return 0; //ceci évitera que la fonction atoi soit appliquée à un entier codé sur plus de 32 bits
	int val = atoi(chaine); //val est l'entier contenu dans chaine
	return ((val<=127)&&(val>=-128)) ; //valeurs maximale et minimale codables sur 8 bits
	
}

 /* teste si un token est un entier signé sur 8 ou 32 bits
 * 
 *@param chaine : le token à analyser
 *@return 0 si non integer8, non null autrement
 */ 
int is_integer(char* chaine) 
{
	return (is_integer8(chaine)||is_integer32(chaine));	
}




 /* teste si un token est un fichier objet
 * 
 *@param chaine : le token à analyser
 *@return 0 si non-objet, non null autrement
 */ 

int is_objet(char* chaine)
{	
	if (chaine!=NULL)
	{
	unsigned int l = strlen(chaine);
	if (l>2 &&chaine[l-2]=='.' && chaine[l-1]=='o') return 1;
	}
	
	return 0;
}


 /* teste si un token est un registre
 * 
 *@param chaine : le token à analyser
 *@return 0 si non-registre, non null autrement
 */ 

int is_reg(char* chaine)
{	
	unsigned int l = strlen(chaine); //La chaîne se finit par le caractère '/0'	
	int resu;	
	switch (l)
	{	
		case 2 : //registres $0 à $9
			if (chaine[0]=='$' && chaine[1]>='0' && chaine[1]<='9')	resu = 1;
			else if (chaine[0]=='h' && chaine[1]=='i') resu = 2;  
			else if (chaine[0]=='l' && chaine[1]=='o') resu = 2;
			else if (chaine[0]=='p' && chaine[1]=='c') resu = 2;
			else if (chaine[0]=='s' && chaine[1]=='p') resu = 2;			
			else resu = 0;
			break;

		case 3 : 
			if (chaine[0]=='$' && chaine[1]>='1' && chaine[1]<='2' && chaine[2]>='0' && chaine[2]<='9') resu = 1; //registres $10 à $29
			else if (chaine[0]=='$' && chaine[1]=='3' && (chaine[2]=='0' ||  chaine[2]=='1')) resu = 1; //registres $30 et $31
			else if (chaine[0]=='$' && chaine[1]=='v' && (chaine[2]=='0' ||  chaine[2]=='1')) resu = 1; //registres $v0 et $v1
			else if (chaine[0]=='$' && chaine[1]=='a' && chaine[2]<='3' && chaine[2]>='0') resu = 3; //registres $a0 à $a3
			else if (chaine[0]=='$' && chaine[1]=='t' && chaine[2]<='9' && chaine[2]>='0') resu = 3; //registres $t0 à $t9
			else if (chaine[0]=='$' && chaine[1]=='s' && chaine[2]<='7' && chaine[2]>='0') resu = 3; //registres $s0 à $s7
			else if (chaine[0]=='$' && chaine[1]=='k' && (chaine[2]=='0' ||  chaine[2]=='1')) resu = 3; //registres $k0 et $k1
			else if (chaine[0]=='$' && chaine[2]=='p' && (chaine[1]=='g' || chaine[1]=='s' || chaine[1]=='f' )) resu = 3; //registres $gp $sp et $fp
			else if (chaine[0]=='$' && chaine[1]=='r' && chaine[2]=='a') resu = 3;
			else if (chaine[0]=='$' && chaine[1]=='a' && chaine[2]=='t') resu = 3;
			else resu = 0;
			break;
		
		case 5 :
			resu = !(strcmp (chaine, "$zero"));
			break;

		default : 
			resu = 0; 
			break;
	}
	return resu;
}


 /* teste si un token est une plage
 * 
 *@param chaine : le token à analyser
 *@return 0 si non-plage, non null autrement
 */ 

int is_range(char* chaine)
{   	
	
	
	char c[100];
	strcpy(c,chaine);
        char *token1 ; char * token2 ;
	token1 = strtok( c, ":+");
	token2 = strtok(NULL, ":+");

	if (token2==NULL) return 0; //si la chaine ne contient ni : ni +

	if (strchr(chaine, ':')!=NULL)
	{
		if (is_hexa(token1)&&is_hexa(token2)) return 1;
		else return 0;
	}
	else return (is_hexa(token1)&&is_integer(token2));

}







/**
 * retourne le type du token 
 * @param chaine le token à analyser
 * @return un entier correspondant au type du token
 * 
 */
int get_type(char* chaine) {
    if (is_hexa(chaine))
        return HEXA;
    if (is_objet(chaine))
	return OBJET;
    if (is_reg(chaine))
	return REG;
    if (is_range(chaine))
	return RANGE;
    if (is_integer32(chaine))
	return INTEGER32;
    if (is_integer8(chaine))
	return INTEGER8;
    if (is_integer(chaine))
	return INTEGER;
    return UNKNOWN;
}






/*************************************************************\
Les commandes de l'émulateur.

 Dans cette version, deux commandes :
	"test" qui attend un nombre strictement positifs d'hexa strictement positifs et affiche ce(s) nombre() +1 dans le terminal
	"exit" qui quitte l'émulateur

 \*************************************************************/

/**
 * version de la commande test qui prend en entrée un hexa
 * la fonction verfiie si le nombre est >=0
 * @param hexValue la valeur à afficher
 * @return 0 en case de succes, un nombre positif sinon
 */
int _testcmd(int hexValue) {
    if(hexValue <= 0) {
        WARNING_MSG("command %s  - invalid call. The argument should be positive.\n", "test");
        return 2;
    }
    fprintf(stdout, "CMD TEST RESULT 0x%x\n", hexValue + 1);

    return 0;
}


/**
 * version de la commande test qui analyse la chaîne entrée à 
 * l'interpreteur. Si la commande est correcte elle est executée. 
 * Si la commande contient plusieurs parametres valides, elle 
 * est excutée plusieurs fois.
 * @param inter l'interpreteur qui demande l'analyse
 * @return 0 en case de succes, un nombre positif sinon
 */

int testcmd(interpreteur inter) {
    DEBUG_MSG("Chaine : %s", inter->input);
    int return_value=0;
    
		int no_args=1;
    /* la commande test attend un argument hexa */
    int hexValue;
    char * token=NULL;
    
    /* la boucle permet d'executé la commande de manière recurrente*/ 
    /* i.e., tant qu'il y a un hexa on continue*/ 
    while((token = get_next_token(inter))!=NULL && return_value==0) {
    		no_args=0;
        switch(get_type(token)) {
        case HEXA:
            sscanf(token,"%x",&hexValue);
            return_value = _testcmd(hexValue);
            break;
        default :
            WARNING_MSG("value %s is not a valid argument of command %s\n",token,"testcmd");
            return 1;
        }
    }


    if(no_args){
            WARNING_MSG("no argument given to command %s\n","testcmd");
            return 1;
        }


    return return_value;
}


/**
 * commande exit qui ne necessite pas d'analyse syntaxique
 * @param inter l'interpreteur qui demande l'analyse
 * @return 0 en case de succes, un nombre positif sinon
 */
int exitcmd(interpreteur inter) {
    INFO_MSG("Bye bye !");
    return -1;
}



/*Pour l'instant les fonctions se contentent de reconnaîre
*les bons parametres d'entree
*/


int loadcmd(interpreteur inter,pm_glob param,FILE * pf_elf) {
	
	//On récupère le nom du fichier à ouvrir
	char *nom_fichier = get_next_token (inter);

	//Ainsi que l'adresse du premier segment
	char* adresse=get_next_token (inter);

	if (is_objet(nom_fichier)) 
	{	
		return load (param.p_memory,param.p_symtab,pf_elf,nom_fichier);
		
	}
	else 
	{
		printf("La commande load prend en parametre un fichier elf\n");
		return 1;
	}
}


int dispcmd(interpreteur inter,pm_glob param) {
	char *token = NULL;   
	int i=0;
	char reg_nom[3];
	int reg_num;
	token = get_next_token (inter);
	if (token==NULL) return 1; //les fonctions is_type acceptent que token = NULL
				
	if (strcmp(token,"mem")==0)
		{
			token = get_next_token (inter);
			if (token==NULL) return 1; //Si pas d'argument derrière mem

			if (strcmp(token,"map")==0) { //Dans le cas où on veut afficher la map mémoire
				if (*(param.p_memory)==NULL)
				{
					WARNING_MSG("Aucun fichier chargé");
					return -2;
				}
				printf("\n------ Sections en mémoire ------\n") ;
    			print_mem(*(param.p_memory));
				return 0;
			}
			else if (is_range(token)) //Dans le cas où on veut afficher une plage mémoire 
			{
				while (token!=NULL)
				{
					if (!is_range(token)) return 1;
					token = get_next_token (inter);
				}
				return 0;
			}
			else return 1; 			
		}
	
	else if (strcmp(token,"reg")==0)		//Dans le cas d'un affichage de registre
		{
			token = get_next_token (inter);
			if (token==NULL) return 1;  // Si aucun paramètre de la commande
			if (strcmp(token,"all")==0) 	// Si on veut afficher tous les registres
			{
				for (i = 0; i < NB_REG; ++i)
				{
					printf("%s : 0x",param.p_registre[i].name);
					printf("%08x",param.p_registre[i].content);
					printf("\n");
				}
				return 0;
			}
			else if (is_reg(token))	//Si maintenant on veut afficher des registres en particulier 
			{
				
				while (token!=NULL)
				{
					if (!is_reg(token)) return 1;	//S'il ne s'agit pas d'un registre on sort
					
					if (is_reg(token)==1)		//Si on a un nombre de registre entre 0 et 31
					{
						reg_nom[0]=token[1];
						reg_nom[1]=token[2];	//On supprime le '$' dans le nom du registre
						reg_nom[2]=token[3];
						reg_num=atoi(reg_nom); //On convertit le numéro du registre
						printf("%s : 0x",param.p_registre[reg_num].name);
						printf("%08x",param.p_registre[reg_num].content);
						printf("\n");
					}

					//S'il s'agit d'un mnémonique
					if (is_reg(token)==2)
					{
		
						for (i = 0; i < NB_REG; ++i)
						{
							//On parcourt le tableau pour sélectionner le registre qui nous intéressent
							if (strcmp(token,param.p_registre[i].name)==0)	
							{
								printf("%s : 0x",param.p_registre[i].name);
								printf("%08x",param.p_registre[i].content);
								printf("\n");
							}
						}
					}

					
					if (is_reg(token)==3){

						reg_nom[0]=token[1];
						reg_nom[1]=token[2];	//On supprime le '$' dans le nom du registre
						reg_nom[2]=token[3];
					
						for (i = 0; i < NB_REG; ++i)
						{
							//On parcourt le tableau pour sélectionner les registres qui nous intéressent
							if (strcmp(reg_nom,param.p_registre[i].name)==0)	
							{
								printf("%s : 0x",param.p_registre[i].name);
								printf("%08x",param.p_registre[i].content);
								printf("\n");
							}
						}


					}
					

					token = get_next_token (inter);


				}
				return 0;				
			}			
			else return 1;
		}

	else return 1;
}



int disasmcmd(interpreteur inter) 
{
	char *token = NULL;   
	token = get_next_token (inter);
	if (token==NULL) return 1;
	int resu = 0;	
	while ((token!=NULL)&&(resu==0))
	{
		printf("%s\n",token);
		if (!is_range(token)) resu = 1;
		token = get_next_token (inter);
	}

	return resu;
}

int setcmd(interpreteur inter) 
{
	char *token = NULL;   
	token = get_next_token (inter);
	if (token==NULL) return 1;	
	
	if (strcmp(token,"mem")==0)
	{
		token = get_next_token (inter);
		if (token==NULL) return 1;	
		if (strcmp(token,"byte")==0)
		{	
			token = get_next_token (inter);	
			if (token==NULL) return 1;
			if (is_hexa(token))
			{
				token = get_next_token (inter);
				if (is_integer(token)) return 0;
				else return 1;				
			}
			else return 1;
		}
		
		else if (strcmp(token,"word")==0)
		{	
			token = get_next_token (inter);	
			if (is_hexa(token))
			{
				token = get_next_token (inter);
				if (is_integer(token)) return 0;
				else return 1;				
			}
			else return 1;
		}
		else return 1;
	}	
	else if (strcmp(token,"reg")==0)
	{
		token=get_next_token (inter);
		if (is_reg(token))
			{
				token = get_next_token (inter);
				if (is_integer(token)) return 0;
				else return 1;				
			}
		else return 1;
	}
	else return 1;
	
}



int assertcmd (interpreteur inter)
{
	char *token = NULL;   
	token = get_next_token (inter);
	if (token==NULL) return 1; 	
	
	if (strcmp(token,"reg")==0)
	{
		token = get_next_token (inter);	
		if (is_reg(token))
		{
			token = get_next_token (inter);	
			if (is_integer(token)) 

			{	
				return 0;
			}			
			else return 1;	
		}
		else return 1;
	}

	else if (strcmp(token,"word")==0)
	{
		token = get_next_token (inter);	
		if (is_hexa(token))
		{
			token = get_next_token (inter);	
			if (is_integer(token)) return 0;
			else return 1;	
		}
		else return 1;
	}

	else if (strcmp(token,"byte")==0)
	{
		token = get_next_token (inter);	
		if (is_hexa(token))
		{
			token = get_next_token (inter);	
			if (is_integer(token)) return 0;
			else return 1;	
		}
		else return 1;
	}

	else return 1;	
}



void debugcmd (interpreteur inter)
{
        inter->mode = INTERACTIF;

}

void resumecmd (interpreteur inter)
{
        inter->mode = SCRIPT;
}

int runcmd(interpreteur inter) 
{
	char *token = NULL;   
	token = get_next_token (inter);

	if (is_hexa(token)) return 0;
	else return 1;
}

void stepcmd (interpreteur inter)
{
	
}

int breakcmd(interpreteur inter) 
{
	char *token = NULL;   
	token = get_next_token (inter);
	if (token==NULL) return 1; 

	if (strcmp(token,"add")==0)
	{
		token = get_next_token (inter);		
		if (is_hexa(token))
		{
			while (token!=NULL)
			{
				token = get_next_token (inter);
				if (!is_hexa(token)) return 1;			
			}
			return 0;
		}
		else return 1;		

	}

	else if (strcmp(token,"del")==0)
	{
		token = get_next_token (inter);
		if (is_hexa(token)) return 0;
		else if (strcmp(token,"all")==0) return 0;
		else return 1;
	}

	else if (strcmp(token,"list")==0) return 0;

	else return 1;
}


void helpcmd(interpreteur inter)
{
	char *token = NULL;   
	token = get_next_token (inter);
	if (token==NULL)
	{
		printf("erreur : la commande help nécessite un paramètre\n");
	}	

	else if(strcmp(token, "assert") == 0) 
	{
		printf("Commande assert\n");
        	printf("test la valeur d'un registre		: assert reg <registre> <valeur>\n");
        	printf("test la valeur d'un mot en mémoire	: assert word <adresse> <valeur>\n");
        	printf("test la valeur d'un octet en mémoire	: assert byte <adresse> <valeur>\n");

	}

	else if(strcmp(token, "break") == 0) 
	{
		printf("Commande break\n");
        	printf("ajout de points d'arrêt		: break add <adresse>+\n");
        	printf("suppression de points d'arrêt	: break del <adresse>+|all\n");
        	printf("affichage des points d'arrêt	: break list <adresse>+\n");

	}

	else if(strcmp(token, "disasm") == 0) 
	{
		printf("Commande disasm\n");
		printf("affichage du code assembleur	: disasm <plage>+\n");
	}

	else if(strcmp(token, "disp") == 0) 
	{
		printf("Commande disp\n");
        	printf("affichage les données mémoire	: disp mem <plage>+\n");
        	printf("affichage la carte mémoire	: disp mem map\n");
        	printf("affichage des registres		: disp reg <registre>+\n");
	}
	
	else if(strcmp(token, "help") == 0) 
	{
		printf("Commande help\n");
        	printf("help assert | break | debug | disasm | disp | exit | help | load | resume | run | set | step\n");
	}

	else if(strcmp(token, "load") == 0) 
	{
		printf("Commande load\n");
        	printf("chargement du programme 	: load <file> [<adresse>]\n");
	}

	else if(strcmp(token, "run") == 0) 
	{
		printf("Commande run\n");
        	printf("\n");
	}

	else if(strcmp(token, "set") == 0) 
	{
		printf("Commande set\n");
        	printf("\n");
	}

	else if(strcmp(token, "step") == 0) 
	{
		printf("Commande step\n");
        	printf("\n");
	}
	else printf("Commande inconnue. Voir help help.\n");
}



/*************************************************************\
 Les deux fonctions principales de l'émulateur.
	execute_cmd: parse la commande et l'execute en appelant la bonne fonction C
	acquire_line : recupere une ligne (donc une "commande") dans le flux
 \*************************************************************/


/**
*
* @brief parse la chaine courante de l'interpreteur à la recherche d'une commande, et execute cette commande.
* @param inter l'interpreteur qui demande l'analyse
* @return 0 si la commande s'est exécutée avec succès (0)
* @return -1 si c'est la commande exit. Dans ce cas, le programme doit se terminer. (-1)
* @return -2 si la commande n'est pas reconnue. (-2)
* @return 1 si erreur d'execution de la commande
*/
int execute_cmd(interpreteur inter,pm_glob param,FILE * pf_elf) {
    DEBUG_MSG("input '%s'", inter->input);
    char cmdStr[MAX_STR];
    memset( cmdStr, '\0', MAX_STR );

    /* gestion des commandes vides, commentaires, etc*/
    if(strlen(inter->input) == 0
            || sscanf(inter->input, "%s", cmdStr) == 0
            || strlen(cmdStr) == 0
            || cmdStr[0] == '#') { /* ligne commence par # => commentaire*/
        return 0;
    }

    /*on identifie la commande avec un premier appel à get_next_token*/
    char * token = get_next_token(inter);

    if(strcmp(token, "exit") == 0) {
        return exitcmd(inter);
    }
    else if(strcmp(token, "test") == 0) {
        return testcmd(inter);
    }
    else if(strcmp(token, "load") == 0) {
        return loadcmd(inter,param,pf_elf);
    }
    else if(strcmp(token, "disp") == 0) {
        return dispcmd(inter,param);
    }
    else if(strcmp(token, "disasm") == 0) {
        return disasmcmd(inter);
    }
    else if(strcmp(token, "set") == 0) {
        return setcmd(inter);
    }
    else if(strcmp(token, "assert") == 0) {
        return assertcmd(inter);
    }
    else if(strcmp(token, "debug") == 0) {
	debugcmd(inter);
    }
    else if(strcmp(token, "resume") == 0) {
        resumecmd(inter);
    }
    else if(strcmp(token, "run") == 0) {
        return runcmd(inter);
    }
    else if(strcmp(token, "step") == 0) {
        stepcmd(inter);
    }
    else if(strcmp(token, "break") == 0) {
        return breakcmd(inter);
    }
    else if(strcmp(token, "help") == 0) {
        helpcmd(inter);
    }
    else {
	WARNING_MSG("Commande inconnue : '%s'\n", cmdStr);
    }
	return -2;
}





/**
 * @param in Input line (possibly very badly written).
 * @param out Line in a suitable form for further analysis.
 * @return nothing
 * @brief This function will prepare a line for further analysis.
 *
 * This function will prepare a line for further analysis and check for low-level syntax errors.
 * colon, brackets, commas are surrounded with blank; tabs are replaced with blanks.
 * negs '-' are attached to the following token (e.g., "toto -   56" -> "toto -56")  .
 */
void string_standardise( char* in, char* out ) {
    unsigned int i=0, j;

    for ( j= 0; i< strlen(in); i++ ) {

        /* insert blanks around special characters
        if (in[i]==':' || in[i]=='+' || in[i]=='~') {
            out[j++]=' ';
            out[j++]=in[i];
            out[j++]=' ';

        }
	else
	*/ //conflit avec la fonction is_range, car la plage est considérée comme deux tokens

        /* remove blanks after negation*/
        if (in[i]=='-') {
            out[j++]=' ';
            out[j++]=in[i];
            while (isblank((int) in[i+1])) i++;
        }

        /* insert one blank before comments */
        else if (in[i]=='#') {
            out[j++]=' ';
            out[j++]=in[i];
        }
        /* translate tabs into white spaces*/
        else if (isblank((int) in[i])) out[j++]=' ';
        else out[j++]=in[i];
    }
}


/**
 *
 * @brief extrait la prochaine ligne du flux fp.
 * Si fp ==stdin, utilise la librairie readline pour gestion d'historique de commande.
 *
 * @return 0 si succes.
 * @return un nombre non nul si aucune ligne lue
 */
int  acquire_line(FILE *fp, interpreteur inter) {
    char* chunk =NULL;

    memset(inter->input, '\0', MAX_STR );
    inter->first_token =0;
    if (inter->mode==SCRIPT) {
        // mode fichier
        // acquisition d'une ligne dans le fichier
        chunk =calloc(MAX_STR, sizeof(*chunk));
        char * ret = fgets(chunk, MAX_STR, fp );
        if(ret == NULL) {
            free(chunk);
            return 1;
        }
        // si windows remplace le \r du '\r\n' (fin de ligne windows) par \0
        if(strlen(ret) >1 && ret[strlen(ret) -2] == '\r') {
            ret[strlen(ret)-2] = '\0';
        }
        // si unix remplace le \n par \0
        else if(strlen(ret) >0 && ret[strlen(ret) -1] == '\n') {
            ret[strlen(ret)-1] = '\0';
        }

    }
    else {
        /* mode shell interactif */
        /* on utilise la librarie libreadline pour disposer d'un historique */
        chunk = readline( PROMPT_STRING );
        if (chunk == NULL || strlen(chunk) == 0) {
            /* commande vide... */
            if (chunk != NULL) free(chunk);
            return 1;
        }
        /* ajout de la commande a l'historique, librairie readline */
        add_history( chunk );

    }
    // standardisation de la ligne d'entrée (on met des espaces là ou il faut)
    string_standardise(chunk, inter->input);

    free( chunk ); // liberation de la mémoire allouée par la fonction readline() ou par calloc()

    DEBUG_MSG("Ligne acquise '%s'\n", inter->input); /* macro DEBUG_MSG : uniquement si compil en mode DEBUG_MSG */
    return 0;
}


/****************/
void usage_ERROR_MSG( char *command ) {
    fprintf( stderr, "Usage: %s [file.emul]\n   If no file is given, executes in Shell mode.", command );
}
