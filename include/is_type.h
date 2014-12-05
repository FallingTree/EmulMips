#ifndef _IS_TYPE_H_
#define _IS_TYPE_H_
#include "elf/elf.h"
#include "elf/syms.h"
#include "mem.h"

#include "common/bits.h"
#include "common/notify.h"
#include <math.h>
#include <ctype.h>
#include <limits.h>

/* type de token */
enum {HEXA32, HEXA8,INTEGER32,INTEGER8,OBJET,REG,RANGE,UNKNOWN};

unsigned int convertir_string_add(char* string);
int is_hexa32(char* chaine);
int is_hexa8(char* chaine);
int is_integer32(char* chaine);
int is_integer8(char* chaine) ;
int is_objet(char* chaine);
int is_reg(char* chaine);
int is_range(char* chaine);
int get_type(char* chaine);

#endif /* _IS_TYPE_H_ */
