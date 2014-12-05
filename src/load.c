/*
 Fonctions nécessaires au chargement d'un fichier elf
 */


#include<stdio.h>
#include<stdlib.h>
#include "common/bits.h"
#include "common/notify.h"
#include "load.h"
#include "mem.h"

 #include "elf/relocator.h"



/*--------------------------------------------------------------------------  */
/**
 * @param fp le fichier elf original
 * @param seg le segment à reloger
 * @param mem l'ensemble des segments
 *
 * @brief Cette fonction effectue la relocation du segment passé en parametres
 * @brief l'ensemble des segments doit déjà avoir été chargé en memoire.
 *
 */
void reloc_segment(FILE* fp, segment seg, mem memory,unsigned int endianness,stab symtab) {
    byte *ehdr    = __elf_get_ehdr( fp );
    uint32_t  scnsz  = 0;
    Elf32_Rel *rel = NULL;
    char* reloc_name = malloc(strlen(seg.name)+strlen(RELOC_PREFIX_STR)+1);
    scntab section_tab;
    
 
    unsigned int S,P, V, A, AHL, AHI, ALO;
    int i;

   byte* tab_byte = calloc(4,sizeof(*tab_byte));

    

    // on recompose le nom de la section
    memcpy(reloc_name,RELOC_PREFIX_STR,strlen(RELOC_PREFIX_STR)+1);
    strcat(reloc_name,seg.name);

    // on récupere le tableau de relocation et la table des sections
    rel = (Elf32_Rel *)elf_extract_scn_by_name( ehdr, fp, reloc_name, &scnsz, NULL );
    elf_load_scntab(fp ,32, &section_tab);

    if (rel == NULL) DEBUG_MSG("Pas de relocation pour le segment \n %s",seg.name);
     
   
    



    if (rel != NULL &&seg.content!=NULL && seg.size._32!=0) {

        INFO_MSG("--------------Relocation de %s-------------------\n",seg.name) ;
        INFO_MSG("Nombre de symboles a reloger: %ld\n",scnsz/sizeof(*rel)) ;



	for (i=0; i<scnsz/sizeof(*rel); i++){

		
		FLIP_ENDIANNESS((rel+i)->r_info);
		FLIP_ENDIANNESS((rel+i)->r_offset);

		P = (rel+i)->r_offset;

		word mot = *((word*) &seg.content[P]);
		FLIP_ENDIANNESS( mot );
		

		S = symtab.sym[ELF32_R_SYM((rel+i)->r_info)].addr._32;

		//printf("TYPE : %d \n SYM : %d \n",ELF32_R_TYPE((rel+i)->r_info),ELF32_R_SYM((rel+i)->r_info)); //debug
		//printf("Relocation du symbole : %s \n",symtab.sym[ELF32_R_SYM((rel+i)->r_info)].name);
		//printf("Adresse du symbole : 0x%08x\n",symtab.sym[ELF32_R_SYM((rel+i)->r_info)].addr._32);

		//printf("Mot avant relocation : %08x\n",mot);
		switch (ELF32_R_TYPE((rel+i)->r_info)){
		
			case (2) :
				A = mot;
				V = S + A;
				
				FLIP_ENDIANNESS(V);
				*((word*) &seg.content[P])= V;
				
				break;

			case (4) :
				A = mot & 0x00FFFFFF;
				V =  ((A <<2)|((P & 0xf0000000)+S))>>2;
				
				
				mot = (mot & 0xFF000000) | (V & 0x00FFFFFF);
				FLIP_ENDIANNESS(mot);
				*((word*) &seg.content[P])= mot;
				
				break;

			case (5) :
				AHI = mot & 0x0000FFFF;
				
				uint32_t ad = (rel+i+1)->r_offset;

				FLIP_ENDIANNESS(ad);

				ALO = *((word*) &seg.content[ad]);
				FLIP_ENDIANNESS(ALO);
				ALO = ALO& 0x0000FFFF;

				AHL =  (AHI << 16) + (short)(ALO);
				//printf("AHI : %08x, ALO : %08x et AHL : %08x \n",AHI,ALO,AHL);
				V = ( AHL + S - ((short)(AHL + S))) >> 16;
				//printf("V : : %08x\n",V);
				mot = (mot&0xFFFF0000) | (V&0x0000FFFF);
				FLIP_ENDIANNESS(mot);
				*((word*) &seg.content[P])= mot;

				break;

			case (6) :
				ALO = mot;
				//AHI = *((word*) &seg.content[(rel+i-1)->r_offset]) ;
				//FLIP_ENDIANNESS(AHI);
				AHL =  (AHI << 16) + (short)(ALO);
				V = AHL + S;
				
				mot = (mot&0xFFFF0000) | (V&0x0000FFFF);
				FLIP_ENDIANNESS(mot);
				*((word*) &seg.content[P])= mot;

				break;

			default :
				ERROR_MSG("Erreur dans le type de la relocation dans le segment %s\n",seg.name);

		}
		
		mot = *((word*) &seg.content[P]);
		FLIP_ENDIANNESS( mot );
		//printf("Mot après relocation : %08x\n",mot);
	}



    }
    del_scntab(section_tab);
    free( rel );
    free( reloc_name );
    free( ehdr );

}



// Fonction permettant de verifier si une chaine de caracteres
// est bien dans la liste des symboles du fichier ELF
// parametres :
// 		name : le nom de la chaine recherchée
//  symtab : la table des symboles
//
// retourne 1 si present, 0 sinon
int is_in_symbols(char* name, stab symtab) {
    int i;
    for (i=0; i<symtab.size; i++) {
        if (!strcmp(symtab.sym[i].name,name)) return 1;
    }
    return 0;
}

// Cette fonction calcule le nombre de segments à prevoir
// Elle cherche dans les symboles si les sections predefinies
// s'y trouve
// parametres :
//  symtab : la table des symboles
//
// retourne le nombre de sections trouvées

unsigned int get_nsegments(stab symtab,char* section_names[],int nb_sections) {
    unsigned int n=0;
    int i;
    for (i=0; i<nb_sections; i++) {
        if (is_in_symbols(section_names[i],symtab)) n++;
    }
    return n;
}


// fonction permettant d'extraire une section du fichier ELF et de la chargée dans le segment du même nom
// parametres :
//   fp         : le pointeur du fichier ELF
//   memory     : la structure de mémoire virtuelle
//   scn        : le nom de la section à charger
//   permission : l'entier représentant les droits de lecture/ecriture/execution
//   add_start  : l'addresse virtuelle à laquelle la section doit être chargée
//
// retourne 0 en cas de succes, une valeur non nulle sinon
int elf_load_section_in_memory(FILE* fp, mem memory, char* scn,unsigned int permissions,unsigned long long add_start) {
    byte *ehdr    = __elf_get_ehdr( fp );
    byte *content = NULL;
    uint  textsz  = 0;
    vsize sz;
    vaddr addr;

    byte *useless = elf_extract_section_header_table( ehdr, fp );
    free( useless );

    if ( NULL == ehdr ) {
        WARNING_MSG( "Can't read ELF file" );
        return 1;
    }

    if ( 1 == attach_scn_to_mem(memory, scn, SCN_ATTR( WIDTH_FROM_EHDR( ehdr ), permissions ) ) ) {
        WARNING_MSG( "Unable to create %s section", scn );
        free( ehdr );
        return 1;
    }

    content = elf_extract_scn_by_name( ehdr, fp, scn, &textsz, NULL );
    if ( NULL == content ) {
        WARNING_MSG( "Corrupted ELF file" );
        free( ehdr );
        return 1;
    }

    switch( WIDTH_FROM_EHDR(ehdr) ) {
    case 32 :
        sz._32   = textsz/*+8*/; /* +8: In case adding a final sys_exit is needed */
        addr._32 = add_start;
        break;
    case 64 :
        sz._64   = textsz/*+8*/; /* +8: In case adding a final sys_exit is needed */
        addr._64 = add_start;
        break;
    default :
        WARNING_MSG( "Wrong machine width" );
        return 1;
    }

    if ( 1 == fill_mem_scn(memory, scn, sz, addr, content ) ) {
        free( ehdr );
        free( content );
        WARNING_MSG( "Unable to fill in %s segment", scn );
        return 1;
    }

    free( content );
    free( ehdr );

    return 0;
}

// fonction affichant les octets d'un segment sur la sortie standard
// parametres :
//   seg        : le segment de la mémoire virtuelle à afficher

void print_segment_raw_content(segment* seg) {
    int k;
    int word =0;
    if (seg!=NULL && seg->size._32>0) {
        for(k=0; k<seg->size._32; k+=4) {
            if(k%16==0) printf("\n  0x%08x ",k);
            word = *((unsigned int *) (seg->content+k));
            FLIP_ENDIANNESS(word);
            printf("%08x ",	word);
        }
    }
}


// la foncrion load charge un fichier elf en entrée
//
int load (pm_glob param, FILE* pf_elf,char* nom_fichier)
{


    char* section_names[NB_SECTIONS]= {TEXT_SECTION_STR,RODATA_SECTION_STR,DATA_SECTION_STR,BSS_SECTION_STR};
    unsigned int segment_permissions[NB_SECTIONS]= {R_X,R__,RW_,RW_};
    unsigned int nsegments;
    int i=0,j=0;
    unsigned int type_machine;
    unsigned int endianness;   //little ou big endian
    unsigned int bus_width;    // 32 bits ou 64bits
    mem* memory = param.p_memory;
    stab* symtab = param.p_symtab;


    unsigned int next_segment_start = *(param.p_adresse_start); // compteur pour designer le début de la prochaine section


    if ((pf_elf = fopen(nom_fichier,"r")) == NULL) {
    ERROR_MSG("cannot open file %s", nom_fichier);
    return 1;
    }



       if (!assert_elf_file(pf_elf))
        ERROR_MSG("file %s is not an ELF file", nom_fichier);
 

    // recuperation des info de l'architecture
    elf_get_arch_info(pf_elf, &type_machine, &endianness, &bus_width);
    // et des symboles
    elf_load_symtab(pf_elf, bus_width, endianness, symtab);


    nsegments = get_nsegments(*symtab,section_names,NB_SECTIONS);

    // allouer la memoire virtuelle (On met nsegments+2 pour prendre en compte la stack et Vsyscall qui n'est pas lu dans le fichier)
    nsegments+=2;
    *memory=init_mem(nsegments);
	
    // Ne pas oublier d'allouer les differentes sections
    j=0;
    for (i=0; i<NB_SECTIONS; i++) {
        if (is_in_symbols(section_names[i],*symtab)) {
            elf_load_section_in_memory(pf_elf,*memory, section_names[i],segment_permissions[i],next_segment_start);
            next_segment_start+= (((*memory)->seg[j].size._32+0x1000)>>12 )<<12; // on arrondit au 1k suppérieur
            //print_segment_raw_content(&(*memory)->seg[j]);
            j++;
        }
    }

    //Allouer la pile (et donc modifier le nb de segments)

    // On remplit les champs mémoire du segment de la pile
    (*memory)->seg[j].name=strdup("[Stack]"); 
    (*memory)->seg[j].attr=SCN_ATTR(1,RW_);
    (*memory)->seg[j].size._32 = STACKSZ_BYTES;
    (*memory)->seg[j].start._32 = ADDR_STACK; 

    (*memory)->seg[j].content = calloc( 1,STACKSZ_BYTES);
    if ( NULL == (*memory)->seg[j].content ) {
                        printf( "Unable to allocate host memory for vmem segment Stack content");
                        (*memory)->seg[j].size._32 = 0;
                        (*memory)->seg[j].start._32 = 0;
                        return 1;
    }


    // On remplit les champs mémoire du segment Vsyscall
    (*memory)->seg[j+1].name=strdup("[Vsyscall]"); 
    (*memory)->seg[j+1].attr=SCN_ATTR(1,RW_);
    (*memory)->seg[j+1].size._32 = VSYCALLZ_BYTES;
    (*memory)->seg[j+1].start._32 = ADDR_VSYCALL; 

    (*memory)->seg[j+1].content = calloc(1,VSYCALLZ_BYTES);
    if ( NULL == (*memory)->seg[j].content ) {
                        printf( "Unable to allocate host memory for vmem segment Vsycall content");
                        (*memory)->seg[j].size._32 = 0;
                        (*memory)->seg[j].start._32 = 0;
                        return 1;
    }



    //------------ Remplissage de la table des symboles pour l'adresse des segments -------------- //

    for (i=0; i < nsegments; i++){
	
	symtab->sym[i+1].addr._32 = (*memory)->seg[i].start._32;

    }

    stab32_print( *symtab );

    //------------------------------------ Relocation -------------------------------------------- //
    for (i=0; i<nsegments; i++) {

	    reloc_segment(pf_elf, (*memory)->seg[i], *memory,endianness,*symtab);
	    
    }

    DEBUG_MSG("-------------- Relocation réussi -------------------\n");

    fclose(pf_elf);
    return 0;

}
