# programme de test des instructions d'addition et de soustraction

.set nomove # demande Ã  l'assembleur de respecter l'ordre des instructions mais permet des optimisations de boucles

_start:
.global _start
ADDI $t1,$zero,12 
ADD $t2,$t1, 1
ADDIU $t3,$t2,5
ADDU $t4, $t3, 0x4

.data 
X: .word 4
