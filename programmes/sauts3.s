ADDI $a0, $zero, 0x3010
JALR $a1, $a0		#Saute à l'adresse 0x3010 et modifie le registre $a1
ADDI $t0, $zero, 12	#Cette instruction n'est pas celle du delay slot, car un NOP est ajouté lors de la compilation
ADDI $t1, $zero, 15
ADDI $t2, $zero, 17	#Instruction à l'adresse 0x300c avant compilation, à l'adresse 0x3010 après compilation
ADDI $t3, $zero, 19
