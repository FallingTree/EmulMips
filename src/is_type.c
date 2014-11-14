/*
 Fonctions de vérifications des types
 */


#include<stdio.h>
#include<stdlib.h>
#include "common/bits.h"
#include "common/notify.h"
 #include "is_type.h"


 /* teste si un token est une valeur hexa
 * 
 *@param chaine : le token à analyser
 *@return 0 si non-hexa, non null autrement
 */ 
int is_hexa(char* chaine) {
    int i=2;
    int x;
    if (!(chaine!=NULL && strlen(chaine)>2&& chaine[0]=='0' && chaine[1]=='x')) return 0;
    else 
	{	while (chaine[i]!='\0')
			{	
				x = chaine[i];
				if (! isxdigit(x)) return 0;
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
			else if (! strcmp(chaine,"hi")) resu = 2;
			else if (! strcmp(chaine,"lo")) resu = 2;
			else if (! strcmp(chaine,"pc")) resu = 2;
			else if (! strcmp(chaine,"sp")) resu = 2;			
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
			else if (! strcmp(chaine,"$ra")) resu = 3;
			else if (! strcmp(chaine,"$at")) resu = 3;
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




