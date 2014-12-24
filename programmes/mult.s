# programme de test des instructions mult

.set nomove # demande Ã  l'assembleur de respecter l'ordre des instructions mais permet des optimisations de boucles

_start:
.global _start

addi $t1,$zero, 8
addi $t2,$zero, 2
mult $t1, $t2
