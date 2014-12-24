# programme de test des instructions d'addition et de soustraction

.set nomove # demande Ã  l'assembleur de respecter l'ordre des instructions mais permet des optimisations de boucles

_start:
.global _start

ADDI $t0, $zero, 0xf0f
ADDI $t1, $zero, 0x0ff

AND $t2, $t0, $t1
ANDI $t3, $t0, 0x0ff

OR $t4, $t0, $t1
ORI $t5, $t0, 0x0f0

XOR $t6, $t0, $t1

ADDI $a0, $zero, 15
ADDI $a1, $zero, -20
SLT $s0, $a0, $a1
SLTI $s1, $a0, 20
SLTIU $s2, $a0, 16
SLTU $s3, $a0, 15

.data 
X: .word 4
