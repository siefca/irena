@SET_MAKE@

ALL_LINGUAS =
additional_DIST = KOPIOWANIE NEWS README AUTHORS TODO INSTALL irena.spec

bin_PROGRAMS = irena

irena_COMMON_SRC = confcache.cc \
		    confcache.hh \
		    config.cc \
		    config.hh \
		    erro.cc \
		    erro.hh \
		    document.cc \
		    document.hh \
		    doccontainer.cc \
		    doccontainer.hh \
		    docs.cc \
		    docs.hh \
		    inet.cc \
		    inet.hh \
		    server.cc \
		    server.hh \
		    irena.cc \
		    irena.hh

irena_SIGBLOCK = ../compat/sigblock.c ../compat/sigblock.h
irena_SNPRINTF = ../compat/snprintf.c

if HAVE_SIGBLOCK
irena_SIGBLOCK_OBJ =
else 
irena_SIGBLOCK_OBJ = ../compat/sigblock.o
endif 

if HAVE_SNPRINTF
irena_SNPRINTF_OBJ =
else 
irena_SNPRINTF_OBJ = ../compat/snprintf.o
endif 


irena_SOURCES = $(irena_COMMON_SRC)
irena_LDADD = $(irena_SIGBLOCK_OBJ) $(irena_SNPRINTF_OBJ)
#irena_DEPENDENCIES = $(irena_SIGBLOCK_OBJ) $(irena_SNPRINTF_OBJ)

if HAVE_SIGBLOCK
else 
../compat/sigblock.o: $(irena_SIGBLOCK)
endif 

if HAVE_SNPRINTF
else 
../compat/snprintf.o: $(irena_SNPRINTF)
endif 

EXTRA_DIST = $(irena_SIGBLOCK) $(irena_SNPRINTF) $(additional_DIST)

