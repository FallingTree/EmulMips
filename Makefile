TARGET=emulMips

# noms des executables utilisés durant la compilation/edition des liens
CC=`which gcc`
LD=`which gcc`
RM=`which rm` -f
DOXYGEN=`which doxygen`

#options de compilation/edition des liens
INCLUDE=-I$(INCDIR)
CFLAGS=-Wall $(INCLUDE)
LFLAGS=-lreadline -lm -lcurses
CFLAGS_DBG=$(CFLAGS) -g -DDEBUG -DVERBOSE
CFLAGS_RLS=$(CFLAGS)
CFLAGS_LIB=$(CFLAGS) -shared -fpic 
CFLAGS_PGM=$(CFLAGS) -c 


# definition des repertoires de source/destination
SRCDIR=src
INCDIR=include
DOCDIR=doc

#Librairie
LIBDIR=lib
LIB=$(wildcard $(LIBDIR)/*.c)
OBJ_LIB=$(LIB:.c=.lib)

#Librairie
PGMDIR = programmes
PGM=$(wildcard $(PGMDIR)/*.s)
OBJ_PGM=$(PGM:.s=.o)

# les fichiers dont on peut se débarasser
GARBAGE=*~ $(SRCDIR)/*~ $(INCDIR)/*~ $(TESTDIR)/*~ $(SRCDIR)/*.orig $(INCDIR)/*.orig

# ou se trouve les sources (i.e., le *.c)
SRC=$(wildcard $(SRCDIR)/*.c)

# les objets avec l'option DEBUG s'appeleront fichier.dbg.o
# ceux de la release fichier.rls.o
OBJ_DBG=$(SRC:.c=.dbg.o)
OBJ_RLS=$(SRC:.c=.rls.o)

# 1er target (celui executé par défaut quand make est lancé sans nom de cible) 
# affiche l'aide
all : 
	@echo ""
	@echo "Usage:"
	@echo ""
	@echo "make debug   => build DEBUG   version"
	@echo "make release => build RELEASE version"
	@echo "make lib     => produce the library of the fuctions"
	@echo "make doc     => produce the doxygen documentation"
	@echo "make clean   => clean everything"
	@echo "make tarball => produce archive .tar.gz in ../ directory"

debug   : $(OBJ_DBG) $(OBJ_LIB)
	$(LD) $^ $(LFLAGS) -o $(TARGET)

release : $(OBJ_RLS) $(OBJ_LIB)
	$(LD) $^ $(LFLAGS) -o $(TARGET)

lib     : $(OBJ_LIB)
	 

%.lib   : %.c
	$(CC) $(CFLAGS_LIB) $(basename $<).c -o $(basename $<).lib

%.rls.o : %.c
	$(CC) $< $(CFLAGS_RLS) -c -o $(basename $<).rls.o

%.dbg.o : %.c
	$(CC) $< $(CFLAGS_DBG) -c -o $(basename $<).dbg.o

doc : 
	$(DOXYGEN)

clean : 
	$(RM) $(TARGET) $(SRCDIR)/*.o $(LIBDIR)/*.lib $(GARBAGE) 
	$(RM) -r $(DOCDIR)/*	

prgm :	$(OBJ_PGM)

%.o    : %.s
	mips-gcc $(CFLAGS_PGM) $(basename $<).s -c -o $(basename $<).o


# créé l'archive à envoyer à votre tuteur (pas de rar SVP! et interdiction absolu d'envoyer autre chose qu'une archive, il ne doit y avoir qu'un seul fichier joint dans l'e-mail !)
archive : 
	make clean 
	tar -czvf ../$(notdir $(PWD) )-`whoami`-`date +%d-%m-%H-%M`.tgz .
	echo "Fichier archive ../emulMips-`whoami`-`date +%d-%m-%H-%M`.tgz genere"
