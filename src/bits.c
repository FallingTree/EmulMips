/**
 * @file bits.c
 * @author François Cayre <francois.cayre@grenoble-inp.fr>
 * @date Sat Nov  2 18:48:18 2013
 * @brief Bit twiddling.
 *
 * Bit twiddling.
 */

#include "common/types.h"
#include "common/bits.h"


/**
 * Simple way of guessing the endianness of the machine on which this code is executed
 * @return The endianness type
 * @see types.h
 */

unsigned int get_host_endianness( void )
{
    unsigned int var = 0x000000FF;
    return ((((unsigned char*) &var)[0]) == 0xFF)?LSB:MSB;
}

/**
 * takes blocks of memory and swaps the content of each block of sz bytes.
 * For instance :
 *     __flip_endianness(p, 2): swaps every pair of bytes
 *     __flip_endianness(p, 4): swaps every 4-byte block
 *     __flip_endianness(p, 1): has no effect
 * @param blocks the memory space to swap
 * @param sz the size (in byte) of each block to swap
 * @return the address of the swapped blocks (which is not changed by the function)
 */
byte * __flip_endianness( byte * blocks, uint sz ) {
    uint i;

    for ( i= 0; i< sz/2; i++ )
    {
        byte tmp = blocks[i];
        blocks[i] = blocks[sz-1-i];
        blocks[sz-1-i] = tmp;
    }

    return blocks;
}

void decouper_word(byte** tab_byte, word mot){

	
	*tab_byte[0] = (byte) FLIP_ENDIANNESS( mot ) & 0x000000ff ;
	*tab_byte[1] = (byte) FLIP_ENDIANNESS( mot ) & 0x0000ff00 ;
	*tab_byte[2] = (byte) FLIP_ENDIANNESS( mot ) & 0x00ff0000 ;
	*tab_byte[3] = (byte) FLIP_ENDIANNESS( mot ) & 0xff000000 ;


}
