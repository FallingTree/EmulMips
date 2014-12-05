#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <limits.h>

#include "emul.h"
#include "mem.h"
#include "is_type.h"
#include "common/notify.h"

int exec(unsigned int* jump, pm_glob param, INST inst)
{

	switch (param.p_registre[2].content) //On teste la valeur du registre $v0

	{
		case 1 :

			printf("%u", param.p_registre[2].content);
			break;	
		
		case 4 :

			break;	

		case 5 :				

			break;	
		
		case 8 :
	
			break;

		case 10 : 
			
			break;

		default :
			WARNING_MSG("Code système inconnu");

			break;		

	
	}




	return 0;
}

int print()
{
	printf("Fonction SYSCALL\n");
	return 0;
}
