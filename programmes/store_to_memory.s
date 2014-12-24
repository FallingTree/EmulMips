ADDI $a0, $zero, 0x3000		#Adresse de début du segment .text
SW $t0, 0xc($a0)	 	#On remplace le mot à l'adresse 0x300c par celui contenu par le registre $t0
NOP
NOP
SB $t1, 0x1b($a0)		#On remplace le byte à l'adresse 0x301b par celui contenu par le registre $t1
NOP
ADDI $a2, $zero, 53

