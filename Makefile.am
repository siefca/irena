ALL_LINGUAS = pl en
always_built_SUBDIRS = src
sometimes_built_SUBDIRS = 

CFLAGS =
CXXFLAGS =

SUBDIRS = doc $(always_built_SUBDIRS)
EXTRA_DIST = TODO irena.spec INSTALL

## make rpms
rpm: Makefile
	$(MAKE) dist
	rpm -ta --clean $(PACKAGE)-$(VERSION).tar.gz
