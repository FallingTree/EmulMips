
//----------------------------------------------------------------------------------------------------------------  
// File : disasm.c 
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


 #include "disasm.h"
 #include "load.h"
 #include "is_type.h"



int disasmcmd(interpreteur inter,pm_glob param) 
{
	mem memory = *(param.p_memory);

	//On vérifie qu'un fichier a été chargé en mémoire
	if (memory==NULL)
	{
  		WARNING_MSG("Il faut d'abord charger un fichier objet/n");
		return 1;
	}	

	char *token = NULL;   
	token = get_next_token (inter);
	if (token==NULL) return 1;

	while (token!=NULL)
	{

		//--------------------------Vérification des paramètres--------------------------//


		if (!is_range(token)) return 1;

		//On extrait les deux adresses de la plage
		char* ad1 =NULL;
		char* ad2 =NULL;
		int cas = (strchr(token, ':')==NULL);	//Il y a deux types de plages. Celles avec un + et celles avec : 
						     	//Si cas=0 l'adresse de fin de plage est ad2, sinon c'est ad1+ad2		
		ad1 = strtok( token, ":+");
		ad2 = strtok(NULL, ":+");	 

		//On récupère le numéro du segment .text dans la mémoire
		int i_text=0;
		if (memory->seg[i_text].name==NULL) 
		{
			WARNING_MSG("Les segments de la mémoire n'ont pas été chargés");
			return 1;
		}		
		while (strcmp(memory->seg[i_text].name,".text")!=0)
		{
			i_text=i_text+1;
			if (i_text>NB_SECTIONS) 
			{	WARNING_MSG("La mémoire ne contient pas de segment .text\n");
				return 1;
			}
		}

		//Récupération de la valeur de l'adresse du segment .text

		unsigned int adrtext;
		switch( SCN_WIDTH( memory->seg[i_text].attr ) ) 
		{
           	case 32 :
			adrtext = memory->seg[i_text].start._32;
                	break;
            	default :
                	return 1;
            	}
		
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
			WARNING_MSG("La seconde adresse doit être plus grande que la première\n");
			return 1;
		}

		//On vérifie que la plage appartient bien au segment .text de la mémoire

		if ((adrtext > vad1)||(vad2 > adrtext + 0x00001000))

		{
			WARNING_MSG("La plage n'appartient pas au segment .text\n");
			return 1;
		}

		//On vérifie que les deux adresses sont multiples de 4. 

		if (vad1%4!=0) 
		{	
			WARNING_MSG("La première adresse n'est pas multiple de 4\n");		
			return 1;
		}

		if (vad2%4!=0)
		{	
			WARNING_MSG("La deuxième adresse n'est pas multiple de 4\n");		
			return 1;
		}


		//--------------------------Actions de la commande--------------------------//
		


		unsigned int size_text ;
		size_text = memory->seg[i_text].size._32 ; //On récupère la taille du segment .text

		int position = vad1 - adrtext; //Indice du tableau contenant les données de .text auquel se trouve le premier byte de l'instruction.

		Instruction * tab_instructions = *(param.p_tab_instructions); //Tableau des mnémoniques
		int nb_instructions = 41; //Nombre d'instructions du dictionnaire.

		word motlu ;//L'instruction lue


		while ((position < size_text)&&(position<vad2-adrtext)) //Tant qu'on n'a pas atteint la fin des données .text, ni la fin de la plage
		{ 
			printf("%x :: ", adrtext + position); //Affichage de l'adresse virtuelle de l'instruction lue. 
			motlu = trouver_mot_adresse(adrtext+position, param); //On lit l'instruction.
			printf("%08x    ",motlu);

			int iinst = 0 ; //Indice du mnémonique lu dans le dictionnaire

			unsigned int masque = tab_instructions[iinst].masque; //Premier masque du dictionnaire
			unsigned int mnemo = tab_instructions[iinst].mnemonique; //Premier mnémonique du dictionnaire

			int test = ((motlu & masque)!= mnemo);//Test qui définit l'arrêt de la boucle while (voir ci-après)

			if (motlu==0) printf("NOP\n"); //Si on lit un mot nul, on sait que l'instruction est NOP.

			else //----------------Reconnaissance des instructions autres que NOP---------------------
			
			{
				//Comparaison du mot lu aux mnémoniques du dictionnaire avec une opération bit à bit
		
				while (test &&(iinst<nb_instructions)) //Tant qu'on n'a pas trouvé le bon mnemonique (càd test = 0) 
									//et qu'on n'a pas atteint la fin du tableau
				{					
					iinst++;
					masque = tab_instructions[iinst].masque; //On passe au masque suivant
					mnemo = tab_instructions[iinst].mnemonique; //On passe au mnémonique suivant
					
					test = ((motlu & masque)!= mnemo);


					//La boucle s'arrête si l'expression binaire du mnémonique apparaît dans l'expression binaire de bylu.
					//On isole donc la partie de bylu correspondant au mnémonique grâce à une opération bit à bit, 
					//puis on la compare au mnémonique.

				}
	
				//Affichage des instructions 

				
				char* t = tab_instructions[iinst].type;
				unsigned int rd, rs, rt, sa, immediate, target, offset;
				reg *registre = param.p_registre;

			
				if (iinst == nb_instructions) printf("Instruction inconnue\n");
				else
				{
					printf("%s ",tab_instructions[iinst].nom);
					if (strcmp(t,"R")==0)
					{
						rd = (motlu & 63488)/2048; // 63488 = 1111100000000000 en binaire, 
										//on divise par 2048 = 2puis11 pour se ramener au numéro du registre
						rs = (motlu & 65011712)/2097152; //65011712 = 1111100000000000000000 en binaire, 2097152 = 2puis21
						rt = (motlu & 2031616)/65536 ;//2031616 = 111110000000000000000 en binaire, 65536 = 2puis16
						sa = (motlu & 1984)/ 64; //1984 = 11111000000 en binaire et 64 = 2puis6 				

						//On traite à part le cas de la commande SEB qui nécessite une valeur spéciale sa précise
						if ((strcmp(tab_instructions[iinst].nom,"SEB")==0)&&(sa=16))
						{
							printf("%s %s %u\n", registre[rd].name, registre[rt].name, sa);
						}
						else
						{
						//Les opérandes à afficher dépendent de l'instruction (voir nomenclature_dictionnaire.txt)

							if (tab_instructions[iinst].var_op[0]&&tab_instructions[iinst].var_op[1]
										&&tab_instructions[iinst].var_op[2] ) 
								printf("%s %s %s\n", registre[rd].name, registre[rs].name, registre[rt].name);

							else if (tab_instructions[iinst].var_op[0]&&tab_instructions[iinst].var_op[1]
											&&tab_instructions[iinst].var_op[3]) 
								printf("%s %s %u\n", registre[rd].name, registre[rt].name, sa);
	
							else if (tab_instructions[iinst].var_op[0]&&tab_instructions[iinst].var_op[1]) 
								printf("%s %s\n", registre[rs].name, registre[rt].name);

							else if (tab_instructions[iinst].var_op[0]&&tab_instructions[iinst].var_op[2]) 
								printf("%s %s\n", registre[rd].name, registre[rs].name);

							else if (tab_instructions[iinst].var_op[0]) 
								printf("%s\n", registre[rs].name);
	
							else if (tab_instructions[iinst].var_op[2]) 
								printf("%s\n", registre[rd].name);
	
							else printf("\n");				
						}			
					}
					else if (strcmp(t,"I")==0)
					{
						rs = (motlu & 65011712)/2097152; //65011712 = 1111100000000000000000 en binaire, 2097152 = 2puis21
						rt = (motlu & 2031616)/65536 ;//2031616 = 111110000000000000000 en binaire, 65536 = 2puis16
						immediate = (motlu & 65535) ;//65535 = 1111111111111111
						offset = immediate *4 ;//Le codage de l'offset économise deux zéros, 
								//car l'adresse d'un mot est forcément multiple de 4
								//Pour avoir la bonne adresse, il faut donc multiplier par 4.

						if (tab_instructions[iinst].var_op[0]&&tab_instructions[iinst].var_op[1]
								&&tab_instructions[iinst].var_op[4] ) 
							printf("%s %s %u\n", registre[rt].name, registre[rs].name, immediate);

						else if (tab_instructions[iinst].var_op[0]&&tab_instructions[iinst].var_op[1]
								&&tab_instructions[iinst].var_op[6] ) 
							printf("%s %s %u\n", registre[rs].name, registre[rt].name, offset);

						else if (tab_instructions[iinst].var_op[0]&&tab_instructions[iinst].var_op[4]) 
							printf("%s %u\n", registre[rs].name, immediate);
	
						else if (tab_instructions[iinst].var_op[1]&&tab_instructions[iinst].var_op[4]) 
							printf("%s %u\n", registre[rt].name, immediate);
					
						else WARNING_MSG("Opérandes mal définis");
					
	
					}
					else if (strcmp(t,"T")==0)
					{
						target = (motlu & 67108863); //67108863 = 11111111111111111111111111
									//L'instruction ne contient que 26bits.
						target = target*4; //On ajoute 2bits car l'adresse est forcément un multiple de 4.
						target = (target | ((adrtext + position + 4) & 4026531840));
							//Enfin, on ajoute les 4bits de poids forts du compteur programme
							//adrtext + position + 4 est l'adresse de l'instruction suivante
							//4026531840 = 11110000000000000000000000000000 est le masque 
							//qui permet d'obtenir les poids forts

				
						printf("%u\n",  target);
					}
				        else WARNING_MSG("Type d'instruction inconnu\n");

				}

			
			}
			position = position+4; 
		}

		

		//--------------------------On recommence avec le token suivant--------------------------//

		token = get_next_token (inter);
	}
	
	return 0;
}





//Cette fonction inspirée de disasm désassemble toutes les instructions du fichier .text et les range dans un tableau d'instructions sans les afficher.
//Ce tableau est utilisé par la fonction run.
//Pour connaître les opérandes utilisés, il faut se référer au nom de l'instruction, ce qui est possible au moment de l'exécution, mais demanderait un grand nombre de cas au moment de l'affichage, c'est pourquoi disasm n'utilise pas ce tableau.

unsigned int decode_instructions(pm_glob param) 
{
	mem memory = *(param.p_memory);

	//On vérifie qu'un fichier a été chargé en mémoire
	if (memory==NULL)
	{
  		WARNING_MSG("Il faut d'abord charger un fichier objet/n");
		return 0;
	}	


	//On récupère le numéro du segment .text dans la mémoire
	int i_text=0;
	if (memory->seg[i_text].name==NULL) 
	{
		WARNING_MSG("Les segments de la mémoire n'ont pas été chargés");
		return 0;
	}		
	while (strcmp(memory->seg[i_text].name,".text")!=0)
	{
		i_text=i_text+1;
		if (i_text>NB_SECTIONS) 
		{	WARNING_MSG("La mémoire ne contient pas de segment .text\n");
			return 0;
		}
	}

	//Récupération de la valeur de l'adresse du segment .text

	unsigned int adrtext;
	switch( SCN_WIDTH( memory->seg[i_text].attr ) ) 
	{
       		case 32 :
			adrtext = memory->seg[i_text].start._32;
                	break;
            	default :
                	return 0;
        }
		
	unsigned int size_text  = memory->seg[i_text].size._32 ; //Taille du segment .text

	int position = 0; //Indice du tableau contenant les données de .text auquel se trouve le premier byte de l'instruction.

	Instruction * tab_instructions = *(param.p_tab_instructions); //Tableau des mnémoniques
	int nb_instructions = 41; //Nombre d'instructions du dictionnaire.

	word motlu ;//L'instruction lue

	int nb_disasm = size_text/4; //Nombre d'instructions à désassembler
	param.p_tab_instructions_disasm=calloc(nb_disasm, sizeof(**param.p_tab_instructions_disasm)); //Allocation du tableau des instructions désassemblées
	INST* tab_instructions_disasm = *param.p_tab_instructions_disasm;


	while (position < size_text) //Tant qu'on n'a pas atteint la fin des données .text
	{ 
		motlu = trouver_mot_adresse(adrtext+position, param); //On lit l'instruction.
			
		int iinst = 0 ; //Indice du mnémonique lu dans le dictionnaire

		unsigned int masque = tab_instructions[iinst].masque; //Premier masque du dictionnaire
		unsigned int mnemo = tab_instructions[iinst].mnemonique; //Premier mnémonique du dictionnaire

		int test = ((motlu & masque)!= mnemo);//Test qui définit l'arrêt de la boucle while (voir ci-après)

		if (motlu==0) printf("NOP\n"); //Si on lit un mot nul, on sait que l'instruction est NOP.

		else //----------------Reconnaissance des instructions autres que NOP---------------------
			
		{
			//Comparaison du mot lu aux mnémoniques du dictionnaire avec une opération bit à bit
		
			while (test &&(iinst<nb_instructions)) //Tant qu'on n'a pas trouvé le bon mnemonique (càd test = 0) 
								//et qu'on n'a pas atteint la fin du tableau
			{					
				iinst++;
				masque = tab_instructions[iinst].masque; //On passe au masque suivant
				mnemo = tab_instructions[iinst].mnemonique; //On passe au mnémonique suivant
					
				test = ((motlu & masque)!= mnemo);


				//La boucle s'arrête si l'expression binaire du mnémonique apparaît dans l'expression binaire de bylu.
				//On isole donc la partie de bylu correspondant au mnémonique grâce à une opération bit à bit, 
				//puis on la compare au mnémonique.

			}
	
			//Détermination des opérandes

				
			char* t = tab_instructions[iinst].type;
			unsigned int rd, rs, rt, sa, immediate, target, offset = 0;
			reg *registre = param.p_registre;

			
			if (iinst == nb_instructions) printf("Instruction inconnue\n");
			else
			{
				if (strcmp(t,"R")==0)
				{
					rd = (motlu & 63488)/2048; // 63488 = 1111100000000000 en binaire, 
									//on divise par 2048 = 2puis11 pour se ramener au numéro du registre
					rs = (motlu & 65011712)/2097152; //65011712 = 1111100000000000000000 en binaire, 2097152 = 2puis21
					rt = (motlu & 2031616)/65536 ;//2031616 = 111110000000000000000 en binaire, 65536 = 2puis16
					sa = (motlu & 1984)/ 64; //1984 = 11111000000 en binaire et 64 = 2puis6 				

									
				}
				else if (strcmp(t,"I")==0)
				{
					rs = (motlu & 65011712)/2097152; //65011712 = 1111100000000000000000 en binaire, 2097152 = 2puis21
					rt = (motlu & 2031616)/65536 ;//2031616 = 111110000000000000000 en binaire, 65536 = 2puis16
					immediate = (motlu & 65535) ;//65535 = 1111111111111111
					offset = immediate *4 ;//Le codage de l'offset économise deux zéros, 
							//car l'adresse d'un mot est forcément multiple de 4
							//Pour avoir la bonne adresse, il faut donc multiplier par 4.

					
	
				}
				else if (strcmp(t,"T")==0)
				{
					target = (motlu & 67108863); //67108863 = 11111111111111111111111111
								//L'instruction ne contient que 26bits.
					target = target*4; //On ajoute 2bits car l'adresse est forcément un multiple de 4.
					target = (target | ((adrtext + position + 4) & 4026531840));
						//Enfin, on ajoute les 4bits de poids forts du compteur programme
						//adrtext + position + 4 est l'adresse de l'instruction suivante
						//4026531840 = 11110000000000000000000000000000 est le masque 
						//qui permet d'obtenir les poids forts

			
				}
			       else WARNING_MSG("Type d'instruction inconnu\n");
	
			}
			

			//Remplissage du tableau tab_instructions_decodees

			tab_instructions_disasm[position/4].nom = tab_instructions[iinst].nom;
			tab_instructions_disasm[position/4].type = t;
			tab_instructions_disasm[position/4].rd = rd;
			tab_instructions_disasm[position/4].rs = rs;
			tab_instructions_disasm[position/4].rt = rt;
			tab_instructions_disasm[position/4].sa = sa;
			tab_instructions_disasm[position/4].immediate = immediate;
			tab_instructions_disasm[position/4].offset = offset;
			tab_instructions_disasm[position/4].target = target;	

			
		}
		position = position+4; 
	}


	return adrtext + (position - 4);//On retourne l'adresse de la dernière instruction désassemblée
}

