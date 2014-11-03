    //----------------------------------------------------------------------------------------------------------------  
// File : mem.h
// Authors : Ammar Mian, Ambre Davat
// Avec participation de François Cayre, Nicolas Castagné, François Portet
//
// Projet C Grenoble INP - Phelma 2A SICOM 2014 : Emulateur Mips
// Architecture mémoire de l'émulateur
//
//---------------------------------------------------------------------------------------------------------------- 

#ifndef _MEM_H_
#define _MEM_H_

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>

#include "common/types.h"
#include "elf/syms.h"
#include "common/liste.h"
#include "dico.h"



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

typedef struct {
    mem *p_memory;
    reg *p_registre;
    Liste_int_t *p_liste_bp; 
    stab *p_symtab;
    Instruction **p_tab_instructions; 
} pm_glob;

#define R__   1
#define RW_   2
#define R_X   3
#define UNDEF 7 /* For optionnal sections (.rodata) */

#define PGSZ          4  /* Page size:  4K */
#define STACKSZ       8  /* Stack size: 8M */
#define STACKSZ_BYTES (STACKSZ*1024*1024)
#define VSYCALLZ_BYTES 4096
#define NPAGES(b) ( (b) ? ( (b)>>10 ) ? ((b)>>10)/PGSZ : 1 : 0 )

#define ADDR_STACK 4286574592
#define ADDR_VSYCALL 4294963200


#define RODATA      0
#define TEXT        1
#define DATA        2
#define BSS         3
#define HEAP        4
#define LIBC_TEXT   5
#define LIBC_DATA   6
#define STACK       7
#define VSYSCALL    8 /* CAUTION: Make sure this one is the very last */

#define NB_REG    35 //Nombre de registres

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
