
/**
 * @file mem.h
 * @author François Cayre <francois.cayre@grenoble-inp.fr>
 * @date Fri Nov  1 00:56:03 2013
 * @brief Memory.
 *
 * Memory.
 */

#ifndef _MEM_H_
#define _MEM_H_

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>

#include "common/types.h"
#include "common/types.h"
#include "elf/syms.h"
#include "common/liste.h"


typedef union {
    uint32_t _32;
    uint64_t _64;
} vsize;

typedef struct {
    char      *name;
    byte      *content;
    vaddr      start;
    vsize      size;
    uint32_t   attr;
} segment;

typedef struct {
    vsize       extent;
    uint32_t    nseg;
    segment    *seg;
} *mem;

typedef struct {
    unsigned int  content;
    char      *name;
} reg;


//Structure d'une instruction du dictionnaire


//Structure d'une instruction désassemblée de type quelconque
typedef struct 
{
    char *nom;
    char *type;
    unsigned int rd;
    unsigned int rs;
    unsigned int rt;
    unsigned int sa;
    unsigned int immediate;
    short int offset;
    unsigned int target;
} INST;

typedef struct pm_gl pm_glob;

typedef struct 
{
    char* nom;
    unsigned int masque;
    unsigned int mnemonique;
    char* type;
    int* var_op;
    int nb_operandes;
    int (*fonction)(unsigned int*,pm_glob,INST);

} Instruction;

struct pm_gl {
    mem *p_memory;
    reg *p_registre;
    Liste_int_t *p_liste_bp; 
    stab *p_symtab;
    stab* p_symtab_libc;
    Instruction **p_tab_instructions;
    int *p_nb_instr;
    unsigned int *p_last_disasm;
    unsigned int *p_adresse_start;
};


#define R__   1
#define RW_   2
#define R_X   3
#define UNDEF 7 /* For optionnal sections (.rodata) */


#define PGSZ 4 /* Page size: 4K */
#define STACKSZ 8 /* Stack size: 8M */
#define STACKSZ_BYTES (STACKSZ*1024*1024)
#define VSYCALLZ_BYTES 4096
#define NPAGES(b) ( (b) ? ( (b)>>10 ) ? ((b)>>10)/PGSZ : 1 : 0 )
#define ADDR_STACK 4286574592
#define ADDR_VSYCALL 4294963200

#define NB_REG 35


#define RODATA      0
#define TEXT        1
#define DATA        2
#define BSS         3
#define HEAP        4
#define LIBC_TEXT   5
#define LIBC_DATA   6
#define STACK       7
#define VSYSCALL    8 /* CAUTION: Make sure this one is the very last */

#define LAST_SEG    ((VSYSCALL)+1)

#define SCN_ATTR(width, rights)  (((width==0x1?32:64)<<8)+rights)
#define SCN_RIGHTS(attr)         (((attr)&0x000000ff))
#define SCN_WIDTH(attr)          (((attr))>>8)

void init_reg(reg *registre);
mem  init_mem( uint32_t nseg );
int attach_scn_to_mem( mem vm, char *name, uint32_t attr );
int fill_mem_scn( mem vm, char *name, vsize sz, vaddr start, byte *content );
void print_mem( mem vm );
void del_mem( mem vm );

#ifdef __cplusplus
}
#endif

#endif /* _MEM_H_ */
