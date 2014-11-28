//----------------------------------------------------------------------------------------------------------------  
// File : interpreteur.c 
// Authors : Ammar Mian, Ambre Davat
// Avec participation de François Cayre, Nicolas Castagné, François Portet
//
// Projet C Grenoble INP - Phelma 2A SICOM 2014 : Emulateur Mips
// Fichier contenant les fonctions utiles à l'utilisation de l'interpréteur ainsi que les commandes de celui-ci
//
//---------------------------------------------------------------------------------------------------------------- 




#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>


 #include "interpreteur.h"
 #include "is_type.h"
 #include "load.h"
 #include "disasm.h"
 #include "emul.h"

//Fonction permettant de découper un mot en hexadécimal en un tableau de byte correspondants aux parties (de 2) du mot

void decouper_word_hexa(char* chaine, byte** tab){


	char *adresse_c;
	int i=0;
	int k=0;
	char b0[5], b1[5], b2[5], b3[5]; //Contenants des mots

// ------------------------ Vérifications d'usage ----------------------------------------	
	if (chaine==NULL) return ;

	while (chaine[i]!='\0'){
		i++;
	}

	if (i==0) return ;
	if (i==2) return ;


	//S'il sagit d'une adresse sur plus de 8 chiffres
	if (i>10)
	{
		return ;
	}


// ------------------------ Normalisation de la chaine -------------------------------------
	adresse_c=strdup("00000000");


	//On copie la portion de la chaine non nulle
	for (k = 10-i; k <= 7; ++k)
	{
		adresse_c[k]=chaine[2+k-(10-i)];
	}
	

// ------------------------ Découpage de la chaîne -----------------------------------------

	b0[0]='0';
	b0[1]='x';
	b0[2]=adresse_c[6];
	b0[3]=adresse_c[7];
	b0[4]='\0';
	(*tab)[0]=convertir_string_add(b0);
	//printf("b0 : %s\n",b0);	//debug
	//printf("b0 converti : Ox%x\n",(*tab)[0]); //debug


	b1[0]='0';
	b1[1]='x';
	b1[2]=adresse_c[4];
	b1[3]=adresse_c[5];
	b1[4]='\0';
	(*tab)[1]=convertir_string_add(b1);
	//printf("b1 : %s\n",b1);

	b2[0]='0';
	b2[1]='x';
	b2[2]=adresse_c[2];
	b2[3]=adresse_c[3];
	b2[4]='\0';
	(*tab)[2]=convertir_string_add(b2);
	//printf("b2 : %s\n",b2);

	b3[0]='0';
	b3[1]='x';
	b3[2]=adresse_c[0];
	b3[3]=adresse_c[1];
	b3[4]='\0';
	(*tab)[3]=convertir_string_add(b3);
	//printf("b3 : %s\n",b3);




}

//Fonction qui permet de trouver le segment correspondant à une adresse donnée
int trouver_seg_adresse(int adresse,pm_glob param){
	int i;

	 for ( i= 0; i < (*(param.p_memory))->nseg-1; i++ ) {
		if ( (*(param.p_memory))->seg[i].start._32 <= adresse && (*(param.p_memory))->seg[i+1].start._32>adresse) return i;
	}
	
	//Cas où il s'agit du dernier segment
	if ( (*(param.p_memory))->seg[i+1].start._32<=adresse) return i+1;

	return -1; //Si ne se trouve pas dans la mémoire
}

//Fonction qui permet d'obtenir la valeur d'un mot dans un segment à partir de l'adresse virtuelle de ce mot
//(cette fonction ne fait pas de vérification ; c'est aux fonctions qui l'utilisent de vérifier que l'adresse passée en paramètre est correcte)

word trouver_mot_adresse(int adresse, pm_glob param)
{
	mem memory = *(param.p_memory);//la mémoire

	//Rappel : memory->seg[i_text].content est un tableau qui (si un fichier elf a été chargé) contient les bytes de la section .text
	//Si l'adresse virtuelle du mot lu est 0x5000, alors les bytes du mot sont contenus dans les cases 0, 1, 2 et 3 du tableau .content.
	//Si l'adresse virtuelle du mot lu est 0x5004, alors les bytes du mot sont contenus dans les cases 4, 5, 6, 7 tableau .content.
	//...


	word motlu ;//le mot lu
	int iseg = trouver_seg_adresse(adresse, param); //le numéro du segment où se trouve le mot
	int position = adresse - memory->seg[iseg].start._32; //Position du premier byte dans le segment
	byte b1, b2, b3, b4 ; // les bytes constituant le mot

	//Lecture des bytes
	b1 = memory->seg[iseg].content[position] ;
	b2 = memory->seg[iseg].content[position+1] ;
	b3 = memory->seg[iseg].content[position+2] ;
	b4 = memory->seg[iseg].content[position+3] ;

	//Calcul du mot (le Mips est un processeur de type big endian)
	//16 puis 2 = 256
	//16 puis 4 = 65536
	//16 puis 6 = 16777216

	motlu = b4 + b3*256 + b2*65536 + b1*16777216;


	return motlu;

}

//Fonction qui permet d'obtenir la valeur d'un byte dans un segment à partir de l'adresse virtuelle de ce byte
//(cette fonction ne fait pas de vérification ; c'est aux fonctions qui l'utilisent de vérifier que l'adresse passée en paramètre est correcte)

byte trouver_byte_adresse(int adresse, pm_glob param)
{
	mem memory = *(param.p_memory);//la mémoire
	byte bylu ;//le byte lu
	int iseg = trouver_seg_adresse(adresse, param); //le numéro du segment où se trouve le mot
	int position = adresse - memory->seg[iseg].start._32; //Position du byte dans le segment

	bylu = memory->seg[iseg].content[position] ;
	return bylu;

}


//Fonction qui convertit une adresse hexadécimale (chaine de caractère) en un entier 
//Attention l'adresse doir etre écrite au maximum sur 8 chiffres sinon renvoie 0
int convertir_string_add(char* chaine){
	
	char *adresse_c;
	int i=0;
	int j=0;
	int k=0;
	int val=0;

// ------------------------ Vérifications d'usage ----------------------------------------	
	if (chaine==NULL) return 0;

	while (chaine[i]!='\0'){
		i++;
	}

	if (i==0) return 0;
	if (i==2) return 0;


	//S'il sagit d'une adresse sur plus de 8 chiffres
	if (i>10)
	{
		return 0;
	}


// ------------------------ Normalisation de la chaine -------------------------------------
	adresse_c=strdup("00000000");


	//On copie la portion de la chaine non nulle
	for (k = 10-i; k <= 7; ++k)
	{
		adresse_c[k]=chaine[2+k-(10-i)];
	}

	//printf("%s\n",adresse_c); debug

	
// ------------------------ Conversion en entier -------------------------------------------
	for ( j = 7; j >= 0; --j)
	{
		if (adresse_c[j]>='0' && adresse_c[j]<='9'){		//S'il sagit d'un chiffre, on retire 48 pour trouver le bon chiffre
			val+=(adresse_c[j]-48)*(int)pow(16,7-j);
		    //printf("Cas 2 : caratère %c %d * %d\n",adresse_c[j],adresse_c[j]-48, (int)pow(16,7-j)); debug

		}

		else if(adresse_c[j]>='a' && adresse_c[j]<='f')		//S'il sagit d'une lettre minuscule, on retire 87
		{
			val+=(adresse_c[j]-87)*(int)pow(16,7-j);
			//printf("Cas 2 : caratère %c %d * %d\n",adresse_c[j],adresse_c[j]-87, (int)pow(16,7-j)); debug
		}

		else if (adresse_c[j]>='A' && adresse_c[j]<='F')	//S'il sagit d'une lettre majuscule, on retire 55
		{
			val+=(adresse_c[j]-55)*(int)pow(16,7-j);
			//printf("Cas 2 : caratère %c %d * %d\n",adresse_c[j],adresse_c[j]-55, (int)pow(16,7-j)); debug
		}

		else 
		{
		//printf("Cas 4 : 0\n"); debug
		val+=0;
		}	
	}
	// printf("Valeur finale :%d\n",val); debug
	return val;


}




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
    char* token=get_next_token (inter);

    if (!(token==NULL)) return 1;

    INFO_MSG("Bye bye !");
    return -1;
}



int loadcmd(interpreteur inter,pm_glob param,FILE * pf_elf) {
	
	unsigned int adrtext;
	int rload;
	int i_text;
	unsigned int size_text ;
	unsigned int ad_load;


	//On récupère le nom du fichier à ouvrir
	char *nom_fichier = get_next_token (inter);

	//Ainsi que l'adresse du premier segment
	
	if (is_objet(nom_fichier)) 
	{		

		char* adresse=get_next_token (inter);
		if (adresse!= NULL && !is_hexa(adresse)){
			WARNING_MSG("Attention : %s n'est pas une adresse\n",adresse);
			return 1;
		}

		if (!adresse)
		{
			
			ad_load = 0x3000;
			param.p_adresse_start = &ad_load;
			rload = load (param,pf_elf,nom_fichier);
			if (rload != 0) return 1;
		
			if (trouver_seg_text (param, &i_text, &adrtext, &size_text) != 0) return 1;
		
			param.p_registre[34].content = adrtext;	//Initialisation du registre pc
			*param.p_last_disasm = adrtext + size_text - 4;	//Récupération de l'adresse de la dernière instruction
		
			//stab32_print(*param.p_symtab); //Affichage de la table des symboles

			return 0;
		}

		ad_load=convertir_string_add(adresse);
		if (ad_load%0x1000 != 0)
		{
			WARNING_MSG("Attention : %s n'est pas une adresse multiple de 0x1000\n",adresse);
			return 1;
		}

		param.p_adresse_start = &ad_load;

		rload = load (param,pf_elf,nom_fichier);
		if (rload != 0) return 1;
	
		if (trouver_seg_text (param, &i_text, &adrtext, &size_text) != 0) return 1;
	
		param.p_registre[34].content = adrtext;	//Initialisation du registre pc
		*param.p_last_disasm = adrtext + size_text - 4;	//Récupération de l'adresse de la dernière instruction
	
		//stab32_print(*param.p_symtab); //Affichage de la table des symboles

		return 0;
		
		
	}
	else 
	{
		WARNING_MSG("La commande load prend en parametre un fichier elf\n");
		return 1;
	}
}



int dispcmd(interpreteur inter,pm_glob param) {
	char *token = NULL;   
	int i=0;
	char reg_nom[3];
	int reg_num;
	int k;
	int byte=0;
	token = get_next_token (inter);
	if (token==NULL) return 1; //les fonctions is_type acceptent que token = NULL
				
	if (strcmp(token,"mem")==0)
		{
			token = get_next_token (inter);
			if (token==NULL) return 1; //Si pas d'argument derrière mem
			if (*(param.p_memory)==NULL)
			{
				WARNING_MSG("Aucun fichier chargé");
				return -2;
		    }

			if (strcmp(token,"map")==0) { //Dans le cas où on veut afficher la map mémoire
				
				printf("\n------ Sections en mémoire ------\n") ;
    			print_mem(*(param.p_memory));
				return 0;
			}
			else if (is_range(token)) //Dans le cas où on veut afficher une plage mémoire 
			{

				while (token!=NULL)
				{
					if (!is_range(token)) return 1;
					//On extrait les deux adresses de la plage
					char* ad1 =NULL;
					char* ad2 =NULL;
					int cas = (strchr(token, ':')==NULL);	//Il y a deux types de plages. Celles avec un + et celles avec : 
												     	//Si cas=0 l'adresse de fin de plage est ad2, sinon c'est ad1+ad2		
					ad1 = strtok( token, ":+");
					ad2 = strtok(NULL, ":+");	 

					//Récupération des valeurs des adresses

					int vad1 = convertir_string_add (ad1);
					int vad2 ;
					char ** endptr2 = NULL;

					if (cas==0)
					{
						vad2 = convertir_string_add(ad2);
	
					}
					else 
					{
						vad2 = vad1 + strtol(ad2,endptr2,10);	
					}

					//On vérifie que la valeur de la deuxième adresse est plus grande que la première

					if (vad1>=vad2) 
					{
						printf("La seconde adresse doit être plus grande que la première");
						return 1;
					}

					//printf("Adresse 1 : %x\n",vad1);
					//printf("Adresse 2 : %x\n",vad2);

					//On récupère les segments correspondants des adresses
					int seg_1=trouver_seg_adresse(vad1, param);
					int seg_2=trouver_seg_adresse(vad2, param);

					if (seg_1==-1 || seg_2==-1 )
					{
						printf("Erreur : Adresse d'un segment non en mémoire\n");
						return 1;
					}

					if (seg_1==seg_2)
					{

						for (k=vad1; k < vad2 && k < (*(param.p_memory))->seg[seg_1].start._32 + (*(param.p_memory))->seg[seg_1].size._32 ; ++k)
						{
							byte=trouver_byte_adresse(k,param);
							printf("Ox%02x ",byte);
						}
						

       					 printf("\n");

       					 return 0;

					}

					if (seg_2>seg_1)
					{


					   for (k=vad1; k < (*(param.p_memory))->seg[seg_1].start._32 + (*(param.p_memory))->seg[seg_1].size._32 ; ++k)
						{
							byte=trouver_byte_adresse(k,param);
							printf("Ox%02x ",byte);
						}

						printf("\n");

       					 for(k=(*(param.p_memory))->seg[seg_2].start._32 ; k<vad2 && k < (*(param.p_memory))->seg[seg_2].start._32 + (*(param.p_memory))->seg[seg_2].size._32 ; k++)
       					 {
           		
                       		byte=trouver_byte_adresse(k,param);
							printf("Ox%02x ",byte);
       					 }

       					 printf("\n");

       					 return 0;


					}


					token = get_next_token (inter);
				}
				return 1;
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
					else
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
					

					token = get_next_token (inter);


				}
				return 0;				
			}			
			else return 1;
		}

	else return 1;
}



int setcmd(interpreteur inter,pm_glob param) 
{
	char *token = NULL;
	char reg_nom[3]; 
	char* reg;
	int reg_num, adresse, valeur;
	byte* mot_cut=calloc(4,sizeof(*mot_cut));


	token = get_next_token (inter);
	if (token==NULL) return 1;	
	
	if (strcmp(token,"mem")==0)
	{
		if (*(param.p_memory)==NULL)
		{
			WARNING_MSG("Aucun fichier chargé");
			return -2;
		}
		token = get_next_token (inter);
		if (token==NULL) return 1;	
		if (strcmp(token,"byte")==0)
		{	
			token = get_next_token (inter);	
			if (token==NULL) return 1;
			if (is_hexa(token))
			{
				adresse=convertir_string_add(token);
				token = get_next_token (inter);

				if (is_hexa(token)){

				valeur = convertir_string_add(token);
				if (!(valeur<=127)&&(valeur>=-128)) return 1;

				int iseg = trouver_seg_adresse(adresse, param); //le numéro du segment où se trouve le mot
				int position = adresse - (*(param.p_memory))->seg[iseg].start._32; //Position du byte dans le segment
				
				if (position>(*(param.p_memory))->seg[iseg].size._32) 
				{
					WARNING_MSG("Erreur : Adresse hors de la zone allouée\n");
					return 1;
				}
				(*(param.p_memory))->seg[iseg].content[position]=valeur; //On met le nombre voulu dans la mémoire
				
				return 0;

				}

				if (is_integer8(token)){

				int iseg = trouver_seg_adresse(adresse, param); //le numéro du segment où se trouve le mot
				int position = adresse - (*(param.p_memory))->seg[iseg].start._32; //Position du byte dans le segment

				(*(param.p_memory))->seg[iseg].content[position]=atoi(token); //On met le nombre voulu dans la mémoire
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
				adresse=convertir_string_add(token);
				token = get_next_token (inter);

				if (is_hexa(token)){


					decouper_word_hexa(token,&mot_cut);
					valeur = convertir_string_add(token);
					if (is_integer8(token)) return 1;

					int iseg = trouver_seg_adresse(adresse, param); //le numéro du segment où se trouve le mot
					int position = adresse - (*(param.p_memory))->seg[iseg].start._32; //Position du byte de l'adresse dans le segment

					(*(param.p_memory))->seg[iseg].content[position]=mot_cut[0];
					(*(param.p_memory))->seg[iseg].content[position+1]=mot_cut[1];
					(*(param.p_memory))->seg[iseg].content[position+2]=mot_cut[2];
					(*(param.p_memory))->seg[iseg].content[position+3]=mot_cut[3];


					return 0;

				}
				
				else return 1;				
			}
			else return 1;
		}
		else return 1;
	}	
	
	if (strcmp(token,"reg")==0)	
	{

		token=get_next_token (inter);
		if (token==NULL) return 1;
		
		if (!is_reg(token)) return 1;	//S'il ne s'agit pas d'un registre on sort
					
		if (is_reg(token)==1)		//Si on a un nombre de registre entre 0 et 31
		{
			
			reg_nom[0]=token[1];
			reg_nom[1]=token[2];	//On supprime le '$' dans le nom du registre
			reg_nom[2]=token[3];
			reg_num=atoi(reg_nom); //On convertit le numéro du registre
			if (reg_num>NB_REG)
			{
				WARNING_MSG("Erreur : registre inconnu\n");
				return 1;
			}

			if (reg_num==0) return 0; //Si le registre à modifier est $zero, on le laisse à 0.

			token=get_next_token(inter); //On récupère la valeur à donner

			if (token==NULL) return 1;

			if (is_integer(token))
			{
				param.p_registre[reg_num].content=atoi(token);
				
				return 0;
			}

			if (is_hexa(token))
			{
				param.p_registre[reg_num].content=convertir_string_add(token);
				return 0;
			}


			return 1;

		}

		if (is_reg(token)==2 || is_reg(token)==3)// Mnémonique 
		{

			reg=strdup(token);
			
			token=get_next_token(inter); //On récupère la valeur à donner
			if (token==NULL) return 1;

			for (reg_num = 0; reg_num < NB_REG; ++reg_num)
			{

				//On parcourt le tableau pour sélectionner le registre qui nous intéressent
				if (strcmp(reg,param.p_registre[reg_num].name)==0)	
				{
					if (strcmp("$zero",param.p_registre[reg_num].name)==0) return 0; 
					//Si le registre à modifier est 0, on le laisse à 0.

					if (is_integer(token))
					{
						param.p_registre[reg_num].content=atoi(token);
						// printf("Valeur à mettre : %d\n",param.p_registre[reg_num].content); //debug
						free(reg);
						return 0;
					}

					if (is_hexa(token))
					{
						param.p_registre[reg_num].content=convertir_string_add(token);
						free(reg);
						return 0;
					}
				}
			}

			WARNING_MSG("Erreur : Registre inconnu\n");
			return 1;

		}
		
		return 1;
	}

	return 1;
	
}




int assertcmd (interpreteur inter, pm_glob param)
{
	reg *registre = param.p_registre;
	int i = 0 ;
	int test = 1;
	int addr;
	char *r ; //registre
	int reg_num;
	word w; 
	byte b; 

	char ** endptr = NULL;
	unsigned int val ; //valeur passée en paramètre
 
	char *token = NULL;   
	token = get_next_token (inter);
	if (token==NULL) return 1; 	
	
	if (strcmp(token,"reg")==0)
	{
		r = get_next_token (inter);
		if (r==NULL) return 1;

		if (is_reg(r)==1)		//Si on a un nombre de registre entre 0 et 31
		{
			
			r[0]=r[1];
			r[1]=r[2];	//On supprime le '$' dans le nom du registre
			r[2]=r[3];
			reg_num=atoi(r); //On convertit le numéro du registre
			if (reg_num>NB_REG)
			{
				WARNING_MSG("Erreur : registre inconnu\n");
				return 1;
			}

			token = get_next_token (inter);
			if (token==NULL) return 1;	
			
			if (is_integer(token)||is_hexa(token)) 

			{	

				if (is_integer(token)) val = strtol(token,endptr,10); 
 				if (is_hexa(token)) val = strtol(token,endptr,16); 
				if (val<0) val = 0xFFFFFFFF+val; //Les registres contiennent des entiers non signés.


				if (registre[reg_num].content==val) //teste si le registre a la valeur val
				{
					printf("Oui\n");
					return 0;
				}
				else 
				{
					printf("Non\n");				
					return 1;
				}			
			}			
			else 
			{ 
				WARNING_MSG("Le paramètre doit être un entier\n"); 
				return 1;
			}

		}

		if (is_reg(r)==2 || is_reg(r)==3)// Mnémonique 
		{
			token = get_next_token (inter);
			if (token==NULL) return 1;

			if (is_integer(token)||is_hexa(token)) 
			{	
				//On cherche l'indice du registre dont on a le nom
				while ((i<=34) && test)
				{
					test = strcmp(r,registre[i].name); //test s'annule si et seulement si la chaîne rmo et le nom du registre sont égaux
					i++;
				}

				if (is_integer(token)) val = strtol(token,endptr,10); 
 				if (is_hexa(token)) val = strtol(token,endptr,16); 
				if (val<0) val = 0xFFFFFFFF+val; //Les registres contiennent des entiers non signés.
				if (registre[i-1].content==val) //teste si le registre a la valeur val
				{
					printf("Oui\n");
					return 0;
				}
				else 
				{
					printf("Non\n");				
					return 1;
				}			
			}			
			else 
			{ 
				WARNING_MSG("Le paramètre doit être un entier\n"); 
				return 1;
			}	
		}
		else 
		{ 
			WARNING_MSG("Le paramètre doit être un registre\n"); 
			return 1;
		}
	}

	else if (strcmp(token,"word")==0)
	{
		token = get_next_token (inter);
		if (token==NULL) return 1;

		if (is_hexa(token))
		{
			addr = convertir_string_add (token);
			w = trouver_mot_adresse(addr, param);
			token = get_next_token (inter);	
			if (is_integer(token)) 			
			{	
				val = strtol(token,endptr,10); //valeur passée en paramètre

				if (w==val) //teste si le mot a la valeur val
				{
					printf("Oui\n");
					return 0;
				}
				else 
				{
					printf("Non\n");				
					return 1;
				}	
			}			
			else 
			{ 
				WARNING_MSG("Le paramètre doit être un entier codable sur 32bits\n"); 
				return 1;
			}
		}
		else 
		{
			WARNING_MSG("Le paramètre doit être une adresse");
			return 1;
		}
	}

	else if (strcmp(token,"byte")==0)
	{
		token = get_next_token (inter);
		if (token==NULL) return 1;

		if (is_hexa(token))
		{
			addr = convertir_string_add (token);
			b = trouver_byte_adresse(addr, param);	
			token = get_next_token (inter);	
			if (is_integer8(token)) 			
			{	
				val = strtol(token,endptr,10); //valeur passée en paramètre

				if (b==val) //teste si le byte a la valeur val
				{
					printf("Oui\n");
					return 0;
				}
				else 
				{
					printf("Non\n");				
					return 1;
				}	
			}			
			else 
			{ 
				WARNING_MSG("Le paramètre doit être un entier codable sur 8bits\n"); 
				return 1;
			}
		}
		else 
		{
			WARNING_MSG("Le paramètre doit être une adresse");
			return 1;
		}
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


int runcmd(interpreteur inter, pm_glob param) 
{
	mem memory = *(param.p_memory);
	char *token = NULL;   
	token = get_next_token (inter);
	unsigned int val ;
	unsigned int adrtext;
	unsigned int size_text;
	int i_text;



	//On vérifie qu'un fichier a été chargé en mémoire
	if (memory==NULL)
	{
  		WARNING_MSG("Il faut d'abord charger un fichier objet/n");
		return 1;
	}

	if (trouver_seg_text (param, &i_text, &adrtext, &size_text) != 0) return 1;

	//Résolution du cas où deux run sont lancés l'un à la suite de l'autre.
	//L'état NOT_STARTED est alors sauté	

	if (*param.p_last_disasm!=0)
	{
		if (param.p_registre[34].content > adrtext)
		{	
				
			printf("Réinitialisation des registres, suppression des breakpoints\n");
			param.p_registre[34].content = adrtext;
			*param.p_liste_bp=supprimer_liste_bp(*param.p_liste_bp);
		}

	}	

	//Vérification que l'adresse passée éventuellement en paramètre est correcte

	if (token==NULL) printf("Exécution à partir de l'adresse courante de pc : %x\n", param.p_registre[34].content);
	else if (is_hexa(token))
	{
		val = convertir_string_add (token);
		if (val%4!=0) 
		{	
			WARNING_MSG("Cette adresse n'est pas multiple de 4\n");		
			return 1;
		}
		if ((val<adrtext)||(val > adrtext + 0x00001000))

		{
			WARNING_MSG("L'adresse n'appartient pas au segment .text\n");
			return 1;
		}
	
		param.p_registre[34].content = val; //On charge le registre pc avec l'adresse fournie en paramètre

	}

	//Modification de l'état de l'interpréteur
	inter->etat = RUN;	

	return 0;
	

}
 
int stepcmd (interpreteur inter, pm_glob param)
{
	
	mem memory = *(param.p_memory);
	char *token = NULL;  
	int i_text=0;
	unsigned int adrtext;	
	unsigned int size_text;

	//On vérifie qu'un fichier a été chargé en mémoire
	if (memory==NULL)
	{
  		WARNING_MSG("Il faut d'abord charger un fichier objet\n");
		return 1;
	}	

	if (trouver_seg_text (param, &i_text, &adrtext, &size_text) != 0) return 1;
	

	//Résolution du cas où l'état NOT_STARTED est sauté	

	if (*param.p_last_disasm!=0)
	{
		if (param.p_registre[34].content > adrtext)
		{	
				
			printf("Réinitialisation des registres, suppression des breakpoints\n");
			param.p_registre[34].content = adrtext;
			*param.p_liste_bp=supprimer_liste_bp(*param.p_liste_bp);
		}

	}	

	size_text  = memory->seg[i_text].size._32 ; //Taille du segment .text

	token = get_next_token (inter);

	if (token==NULL) 
	{
		inter->etat = RUN ; 
		return 0;
	}					
	if (strcmp(token,"into")==0)
	{
		inter->etat = RUN_1 ;

		return 0;
	}
	return 1;	

}

int breakcmd(interpreteur inter,pm_glob param) {
	char *token = NULL;  
	int adresse;

	token = get_next_token (inter);

	if (token==NULL) return 1; //les fonctions is_type acceptent que token = NULL
				
	if (strcmp(token,"add")==0)
		{
			token = get_next_token (inter);
			if (token==NULL) return 1; //Si pas d'argument derrière add
			//printf("Le token est : %s\n",token);
			if (is_hexa(token))	//Si l'argument est bien une adresse 
			{
				
				while (token!=NULL)
				{

					if (!is_hexa(token)) return 1;	//S'il ne s'agit pas d'une adresse on sort
					// printf("Caratère %s :\n",token); debug

					adresse=convertir_string_add(token);			// On la convertit en nombre
					if (adresse==0) return 1;

					if (etre_dans_liste(adresse,*(param.p_liste_bp)))
					{
						WARNING_MSG("Breakpoint %s déjà dans la liste !\n",token);
					}

					else {

					// printf("L'adresse convertie est : %x\n",adresse); debug
					*(param.p_liste_bp)=ajout_tete_int(adresse,*(param.p_liste_bp)); //On la rajoute dans la liste des breakpoints
					// visualiser_liste_int_t(*(param.p_liste_bp)); debug
					}	

					token = get_next_token (inter);	

				}

				return 0;				
			}			
			
		}

	if (strcmp(token,"list")==0)	
	{
			token = get_next_token (inter);
			if (token!=NULL) return 1; //Si arguments derrière list
			visualiser_liste_int_t(*(param.p_liste_bp));
			return 0;
	}

	if (strcmp(token,"del")==0)
	{
			
		token = get_next_token (inter);
		if (token==NULL) return 1; //Si pas d'arguments derrière del
			
		if (strcmp(token,"all")==0){
		*(param.p_liste_bp)=supprimer_liste_bp(*(param.p_liste_bp));		

		}

		else {

			while (token!=NULL)
			{

				if (!is_hexa(token)) return 1;	//S'il ne s'agit pas d'une adresse on sort
				//printf("Caratère %s :\n",token); //debug

				adresse=convertir_string_add(token);			// On la convertit en nombre
				if (adresse==0) return 1;		

				//printf("L'adresse convertie est : %x\n",adresse); //debug
				*(param.p_liste_bp)=supprimer_break_point(*(param.p_liste_bp),adresse); //On la supprime de la liste des breakpoints si possible

				token = get_next_token (inter);

			}

		}

		return 0;
	}


	return 1;

}


void helpcmd(interpreteur inter)
{
	char *token = NULL;   
	token = get_next_token (inter);
	if (token==NULL)
	{
		WARNING_MSG("erreur : la commande help nécessite un paramètre\n");
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
	else WARNING_MSG("Commande inconnue. Voir help help.\n");
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
        return disasmcmd(inter,param) ;
    }
    else if(strcmp(token, "set") == 0) {
        return setcmd(inter, param);
    }
    else if(strcmp(token, "assert") == 0) {
        return assertcmd(inter, param);
    }
    else if(strcmp(token, "debug") == 0) {
	debugcmd(inter);
    }
    else if(strcmp(token, "resume") == 0) {
        resumecmd(inter);
    }
    else if(strcmp(token, "run") == 0) {
        return runcmd(inter, param);
    }
    else if(strcmp(token, "step") == 0) {
        return stepcmd(inter, param);
    }
    else if(strcmp(token, "break") == 0) {
        return breakcmd(inter,param);
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
