# programme de test des instructions d'addition et de soustraction

.set nomove # demande Ã  l'assembleur de respecter l'ordre des instructions mais permet des optimisations de boucles

_start:
.global _start
ADDI $t0, $zero, 1000
ADDI $t1, $zero, 101
SUB $t2, $t0, $t1
SUB $t3, $t1, $t0
SUBU $t4, $t0, $t1
SUBU $t5, $t1, $t0


.data 
X: .word 4
