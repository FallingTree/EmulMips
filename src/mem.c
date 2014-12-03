
/**
 * @file mem.c
 * @author François Cayre <francois.cayre@grenoble-inp.fr>
 * @date Fri Nov  1 01:37:19 2013
 * @brief Memory.
 *
 * Memory.
 */

#include <strings.h>
#include <string.h>

#include "mem.h"
#include "common/bits.h"
#include "common/notify.h"



// strdup is defined in the string.h header but included only for glibc >=2.12.
// the following declaration bypasses the version declaration
char *strdup( const char * );



//Initialise les registres avec les noms et en mettant le contenu à 0.

void init_reg(reg *registre)
{
    
    registre[0].content=0;
    registre[0].name=strdup("$zero");


    registre[1].content=0;
    registre[1].name=strdup("$at");

    registre[2].content=0;
    registre[2].name=strdup("$v0");

    registre[3].content=0;
    registre[3].name=strdup("$v1");

    registre[4].content=0;
    registre[4].name=strdup("$a0");

    registre[5].content=0;
    registre[5].name=strdup("$a1");

    registre[6].content=0;
    registre[6].name=strdup("$a2");

    registre[7].content=0;
    registre[7].name=strdup("$a3");

    registre[8].content=0;
    registre[8].name=strdup("$t0");

    registre[9].content=0;
    registre[9].name=strdup("$t1");

    registre[10].content=0;
    registre[10].name=strdup("$t2");

    registre[11].content=0;
    registre[11].name=strdup("$t3");

    registre[12].content=0;
    registre[12].name=strdup("$t4");

    registre[13].content=0;
    registre[13].name=strdup("$t5");

    registre[14].content=0;
    registre[14].name=strdup("$t6");

    registre[15].content=0;
    registre[15].name=strdup("$t7");

    registre[16].content=0;
    registre[16].name=strdup("$s0");

    registre[17].content=0;
    registre[17].name=strdup("$s1");

    registre[18].content=0;
    registre[18].name=strdup("$s2");

    registre[19].content=0;
    registre[19].name=strdup("$s3");

    registre[20].content=0;
    registre[20].name=strdup("$s4");

    registre[21].content=0;
    registre[21].name=strdup("$s5");

    registre[22].content=0;
    registre[22].name=strdup("$s6");

    registre[23].content=0;
    registre[23].name=strdup("$s7");

    registre[24].content=0;
    registre[24].name=strdup("$t8");

    registre[25].content=0;
    registre[25].name=strdup("$t9");

    registre[26].content=0;
    registre[26].name=strdup("$k0");

    registre[27].content=0;
    registre[27].name=strdup("$k1");

    registre[28].content=0;
    registre[28].name=strdup("$gp");

    registre[29].content=0;
    registre[29].name=strdup("sp");

    registre[30].content=0;
    registre[30].name=strdup("$fp");

    registre[31].content=0;
    registre[31].name=strdup("$ra");

    registre[32].content=0;
    registre[32].name=strdup("hi");

    registre[33].content=0;
    registre[33].name=strdup("lo");

    registre[34].content=0;
    registre[34].name=strdup("pc");
}


/**
 * Creates a virtual memory and performs the necessary memory allocations
 * @param size  the number of segments that composes this virtual memory
 * @return the new allocated virtual memory or NULL in case of failure
 */


mem  init_mem( uint32_t nseg ) {

    mem vm = calloc( 1, sizeof( *vm ) );

    if ( NULL == vm ) {
        WARNING_MSG( "Unable to allocate host memory for vmem" );
        return NULL;
    }
    else {
        uint i;

        vm->seg = calloc( nseg, sizeof( *(vm->seg) ) );
        if ( NULL == vm->seg ) {
            WARNING_MSG( "Unable to allocate host memory for vmem segment" );
            free( vm );
            return NULL;
        }

        // each segment is initialised to a null value
        // Note that though this is unnecessary since a calloc is used
        // this permits future evolution of the default initialisation values
        for ( i= 0; i< nseg; i++ ) {
            vm->seg[i].name      = NULL;
            vm->seg[i].content   = NULL;
            vm->seg[i].start._64 = 0x0;
            vm->seg[i].size._64  = 0x0;
            vm->seg[i].attr      = 0x0;
        }

        vm->nseg = nseg;
    }

    return vm;
}

/**
 * allocates an available segment in the virtual memory to the section named 'name'.
 * @param name the name of the segment (e.g., ".text", ".data"...)
 * @param attribute an integer value corresponding to the access and execution permissions of the segment (@see mem.h)
 * @return 0 in case of success, a non null value otherwise
 */
int attach_scn_to_mem( mem vm, char *name, uint32_t attr ) {

    if ( NULL != vm ) {
        uint i;

        for ( i= 0; i< vm->nseg; i++ ) {
            if ( NULL == vm->seg[i].name ) {
                vm->seg[i].attr = attr;
                vm->seg[i].name = strdup( name );
                return 0;
            }
        }

        WARNING_MSG( "All vmem segments already allocated" );
        return 1;
    }
    else {
        WARNING_MSG( "Virtual memory not initialized" );
        return 1;
    }
}

/**
 * Sets a segment with all the information about a section read from a ELF file
 * @param vm the virtual memory in which the segment to fill in can be found
 * @param name the name of the segment to look for (e.g., ".text", ".data"...)
 * @param size the size of the section content (in bytes)
 * @param start the virtual address in the virtual memory at which the beginning of the segment is located
 * @param content the real address at which the bytes of the section is located
 * @return 0 if succeed, a non null value otherwise
 */

int fill_mem_scn( mem vm, char *name, vsize sz, vaddr start, byte *content ) {

    if ( NULL != vm ) {
        uint i;

        for ( i= 0; i< vm->nseg; i++ ) {
            if ( 0 == strncasecmp( vm->seg[i].name, name, strlen(name) ) ) {
                switch( SCN_WIDTH( vm->seg[i].attr ) ) {
                case 32 :
                    vm->seg[i].size._32 = sz._32;
                    vm->seg[i].start._32 = start._32;
                    vm->seg[i].content = calloc( 1, sz._32 );
                    if ( NULL == vm->seg[i].content ) {
                        WARNING_MSG( "Unable to allocate host memory for vmem segment %s content", name );
                        vm->seg[i].size._32 = 0;
                        vm->seg[i].start._32 = 0;
                        return 1;
                    }
                    if ( NULL != content ) {
                        memcpy( vm->seg[i].content, content, sz._32 );
                    }
                    return 0;
                    break;
                case 64 :
                    vm->seg[i].size._64 = sz._64;
                    vm->seg[i].start._64 = start._64;
                    vm->seg[i].content = calloc( 1, sz._64 );
                    if ( NULL == vm->seg[i].content ) {
                        WARNING_MSG( "Unable to allocate host memory for vmem segment %s content", name );
                        vm->seg[i].size._64 = 0;
                        vm->seg[i].start._64 = 0;
                        return 1;
                    }
                    if ( NULL != content ) {
                        memcpy( vm->seg[i].content, content, sz._64 );
                    }
                    return 0;
                    break;
                default :
                    WARNING_MSG( "Wrong width" );
                    return 1;
                }
                return 0;
            }
        }

        WARNING_MSG( "Segment %s not attached yet", name );
        return 1;
    }
    else {
        WARNING_MSG( "Virtual memory not initialized" );
        return 1;
    }
}


/**
* prints the content of a virtual memory
* @param a virtual memory
*/

void print_mem( mem vm ) {
    if ( NULL != vm ) {
        uint i;
        uint n = 0;

        for ( i= 0; i< vm->nseg; i++ ) {
            if ( UNDEF != SCN_RIGHTS( vm->seg[i].attr ) ) {
                n++;
            }
        }

        printf( "Virtual memory map (%u segments)\n", n );

        for ( i= 0; i< vm->nseg; i++ ) {

            if ( UNDEF == SCN_RIGHTS( vm->seg[i].attr ) ) {
                continue;
            }

            printf( "%-8s\t", vm->seg[i].name );
            switch( SCN_RIGHTS( vm->seg[i].attr ) ) {
            case R__ :
                printf( "r--" );
                break;
            case RW_ :
                printf( "rw-" );
                break;
            case R_X :
                printf( "r-x" );
                break;
            default :
                printf( "???" );
            }
            printf( "\tVaddr: " );
            switch( SCN_WIDTH( vm->seg[i].attr ) ) {
            case 32 :
                printf( "0x%08x", vm->seg[i].start._32 );
                break;
            case 64 :
                printf( "0x%016lx", (long unsigned int)vm->seg[i].start._64 );
                break;
            default :
                printf( "?????????" );
                break;
            }
            printf( "\tSize: " );
            switch( SCN_WIDTH( vm->seg[i].attr ) ) {
            case 32 :
                printf( "%u", vm->seg[i].size._32 );
                break;
            case 64 :
                printf( "%lu", (long unsigned int)vm->seg[i].size._64 );
                break;
            default :
                printf( "?????????" );
                break;
            }
            printf( " bytes\n" );
            /*
            printf( " bytes\t(%s)\n", vm->seg[i].content==NULL?"empty":"filled" );
            */
        }
    }

    return;
}


/**
 * Destroys a virtual memory by making all the necessary free operations
 * @param a virtual memory
 */
void del_mem( mem vm ) {

    if ( NULL != vm ) {

        if ( NULL != vm->seg ) {
            uint i;

            for ( i= 0; i< vm->nseg; i++ ) {
                if ( NULL != vm->seg[i].content ) {
                    free( vm->seg[i].content );
                }
                if ( NULL != vm->seg[i].name ) {
                    free( vm->seg[i].name );
                }
            }

            free( vm->seg );
        }

        free( vm );
    }

    return;
}


