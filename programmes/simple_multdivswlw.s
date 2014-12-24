# Attention ne pas essayer d'assembler ou d'executer ce code les
# LW et SW ne seront pas corrects car elles accÃ¨dent directement 
# Ã  la section TEXT. SW ecrase notamment la premiÃ¨re instruction
# avec un NOP...
# le div est par dÃ©faut entourÃ© d'instructions pour gerer la
# division par zero. Dans le .o ces instructions ont Ã©tÃ© enlevÃ©es
# le code .o n'est donc compatible qu'avec l'emulateur lorsque 
# la gestion de la relocation n'est pas active et en assumant une 
# section TEXT logÃ©e Ã  0x0
 
# code qui ne fait rien d'autre que tester les instructions suivantes


addi $t1,$zero,2
addi $t2,$zero,4
mult $t2,$t1
div $t2,$t1
mfhi $t3
mflo $t4
lw $t5,0x28($zero) # recuperer le code de MFHI
sw $zero,0($zero) #ecrase la premiere instruction avec un NOP

