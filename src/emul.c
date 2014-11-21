


//----------------------------------------------------------------------------------------------------------------  
// File : interpreteur.h
// Authors : Ammar Mian, Ambre Davat
// Avec participation de François Cayre, Nicolas Castagné, François Portet
//
// Projet C Grenoble INP - Phelma 2A SICOM 2014 : Emulateur Mips
//
//---------------------------------------------------------------------------------------------------------------- 

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <limits.h>

 #include "emul.h"
 #include "mem.h"
 #include "is_type.h"
 #include "common/notify.h"

int emul (pm_glob param, INST inst)
{
	reg *registre = param.p_registre;
	char * nom = inst.nom;
	unsigned long int val;
	int condition;
	unsigned int target_offset;

	if (strcmp(nom,"ADD")==0)
		{
			val = registre[inst.rs].content + inst.immediate;

			if ((val>UINT_MAX)||(val<0))
				WARNING_MSG("Attention, résultat non codable sur 32bits. Le registre n'est pas modifié.");
			else registre[inst.rd].content = val ;
			return 0;
		}

	if (strcmp(nom,"ADDI")==0)
		{
			val = registre[inst.rs].content + inst.immediate;

			if ((val>UINT_MAX)||(val<0))
				WARNING_MSG("Attention, résultat non codable sur 32bits. Le registre n'est pas modifié.");
			else registre[inst.rt].content = registre[inst.rs].content + inst.immediate ;
			return 0;
		}	

	if (strcmp(nom,"ADDIU")==0)
		{
			registre[inst.rt].content = registre[inst.rs].content + inst.immediate ;
			return 0;
		}	

	if (strcmp(nom,"ADDU")==0)
		{
			registre[inst.rd].content = registre[inst.rs].content + registre[inst.rt].content ;
			return 0;
		}

	if (strcmp(nom,"AND")==0)
		{
			registre[inst.rd].content = registre[inst.rs].content & registre[inst.rt].content ;
			return 0;
		}

	if (strcmp(nom,"ANDI")==0)
		{
			registre[inst.rt].content = registre[inst.rs].content & inst.immediate ;
			return 0;
		}

	if (strcmp(nom,"BEQ")==0)
		{
			target_offset = registre[inst.offset].content + registre[34].content;
			condition = (registre[inst.rs].content == registre[inst.rt].content);
			if (condition) registre[34].content = registre[34].content + target_offset  ;
			return 0;
		}

	if (strcmp(nom,"BGEZ")==0)
		{
			target_offset = registre[inst.offset].content + registre[34].content;
			condition = (registre[inst.rs].content >= 0);
			if (condition) registre[34].content = registre[34].content + target_offset  ;
			return 0;
		}

	if (strcmp(nom,"BGTZ")==0)
		{
			target_offset = registre[inst.offset].content + registre[34].content;
			condition = registre[inst.rs].content > 0;
			if (condition) registre[34].content = registre[34].content + target_offset ;
			return 0;
		}

	if (strcmp(nom,"BLEZ")==0)
		{
			target_offset = registre[inst.offset].content + registre[34].content;
			condition = (registre[inst.rs].content <= 0);
			if (condition) registre[34].content = registre[34].content + target_offset ;
			return 0;
		}

	if (strcmp(nom,"BLTZ")==0)
		{
			target_offset = registre[inst.offset].content + registre[34].content;
			condition = (registre[inst.rs].content < 0);
			if (condition) registre[34].content = registre[34].content + target_offset ;
			return 0;
		}

	if (strcmp(nom,"BNE")==0)
		{
			target_offset = registre[inst.offset].content + registre[34].content;
			condition = (registre[inst.rs].content != registre[inst.rt].content);
			if (condition) registre[34].content = registre[34].content + target_offset ;
			return 0;
		}

	if (strcmp(nom,"BREAK")==0)
		{
			WARNING_MSG("Breakpoint");
			return 0;
		}

	if (strcmp(nom,"DIV")==0)
	{
		registre[33].content = registre[inst.rs].content / registre[inst.rt].content;
		registre[32].content = registre[inst.rs].content % registre[inst.rt].content;
		return 0;
	}

	if (strcmp(nom,"J")==0)
	{
		registre[34].content = inst.target ;
		return 0;
	}

	if (strcmp(nom,"JAL")==0)
	{
		registre[31].content = registre[34].content + 8;
		registre[34].content = inst.target ;
		return 0;
	}

	if (strcmp(nom,"JALR")==0)
	{
		registre[inst.rd].content = registre[34].content + 8;
		registre[34].content = inst.target ;
		return 0;
	}

	if (strcmp(nom,"JR")==0)
	{
		registre[34].content = registre[inst.rs].content ;
		return 0;
	}

	else 
	{
		WARNING_MSG("Execution de l'instruction %s non définie", inst.nom);
		return 1;
	}	

}

