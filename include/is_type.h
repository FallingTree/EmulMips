#ifndef _IS_TYPE_H_
#define _IS_TYPE_H_
#include "elf/elf.h"
#include "elf/syms.h"
#include "mem.h"

#include "common/bits.h"
#include "common/notify.h"
#include <ctype.h>
#include <limits.h>

/* type de token */
enum {HEXA,INTEGER32,INTEGER8,INTEGER,OBJET,REG,RANGE,UNKNOWN};


int is_hexa(char* chaine);
int is_integer32(char* chaine);
int is_integer8(char* chaine) ;
int is_integer(char* chaine);
int is_objet(char* chaine);
int is_reg(char* chaine);
int is_range(char* chaine);
int get_type(char* chaine);

#endif /* _IS_TYPE_H_ */
