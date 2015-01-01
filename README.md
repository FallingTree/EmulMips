EmulMips
================================================================================

Projet informatique Sicom 2014 - EmulMips 
Phelma Grenoble - INP
Authors : Ammar Mian / Ambre Davat

================================================================================


Ce dossier contient le code d'un projet informatique consistant à émuler un microprocesseur de type Mips.
Codé en language C.

pour compiler la version debuggable 
$ make debug 

pour compiler la version non debuggable
$ make release 

pour compiler les programmes assembleur
$ make prgm

pour générer la documentation
$ make doc

pour nettoyer
$ make clean

pour créer une archive
$ make archive


pour exécuter

  emulMips [fichier de scripts] 



les sources se trouvent dans
./src/

les includes dans 
./include/ 

les plugins dans
./lib

Les fichiers de script de test se trouvent dans 
./tests/

