#ifndef _LOAD_H_
#define _LOAD_H_
#include "elf/elf.h"
#include "elf/syms.h"
#include "mem.h"

#include "common/bits.h"
#include "common/notify.h"

// On fixe ici une adresse basse dans la mémoire virtuelle. Le premier segment
// ira se loger à cette adresse.
#define START_MEM 0x3000

#define LIBC_MEM_END 0xff7ff000u
// nombre max de sections que l'on extraira du fichier ELF
#define NB_SECTIONS 4

// nom de chaque section
#define TEXT_SECTION_STR ".text"
#define RODATA_SECTION_STR ".rodata"
#define DATA_SECTION_STR ".data"
#define BSS_SECTION_STR ".bss"

//nom du prefix à appliquer pour la section
#define RELOC_PREFIX_STR ".rel"

#define PATH_TO_LIBC "./lib/libc.so"


void reloc_segment(FILE* fp, segment seg, mem memory,unsigned int endianness,stab* p_symtab,stab* symtab_libc,FILE* fp_libc);
int is_in_symbols(char* name, stab symtab);
unsigned int get_nsegments(stab symtab,char* section_names[],int nb_sections);
int elf_load_section_in_memory(FILE* fp, mem memory, char* scn,unsigned int permissions,unsigned long long add_start);
void print_segment_raw_content(segment* seg);
int load (pm_glob param, FILE* pf_elf,char* nom_fichier);

#endif /* _LOAD_H_ */
