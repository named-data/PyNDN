pyndndir = $(PYTHON_SITE_PKG)/pyndn

SUBDIRS = namecrypto

pyndn_LTLIBRARIES = _pyndn.la
noinst_HEADERS = \
	pyndn.h \
	key_utils.h \
	methods.h \
	methods_contentobject.h \
	methods_handle.h \
	methods_interest.h \
	methods_key.h \
	methods_name.h \
	methods_signature.h \
	methods_signedinfo.h \
	objects.h \
	python_hdr.h \
	util.h

_pyndn_la_SOURCES = \
	pyndn.c \
	key_utils.c \
	methods.c \
	methods_contentobject.c \
	methods_handle.c \
	methods_interest.c \
	methods_key.c \
	methods_name.c \
	methods_signature.c \
	methods_signedinfo.c \
	objects.c \
	util.c

noinst_HEADERS += \
	methods_namecrypto.h
_pyndn_la_SOURCES += \
	methods_namecrypto.c

#_pyndn_la_SOURCES += $(noinst_HEADERS:.h=.h.gch)

#-O3 -pedantic -Wall -Wextra -fdiagnostics-show-option -Winvalid-pch
#-Wstrict-prototypes -Wmissing-prototypes -Wshadow -Wconversion -Werror
_pyndn_la_CPPFLAGS = -std=c99 -Wall -Wextra -Winvalid-pch -Wstrict-prototypes
_pyndn_la_CPPFLAGS += -Wmissing-prototypes -Wshadow -fdiagnostics-show-option
_pyndn_la_CPPFLAGS += @PYTHON_CPPFLAGS@ @NDN_INCLUDES@ @OPENSSL_INCLUDES@ -Wno-unknown-pragmas
_pyndn_la_LIBADD = namecrypto/libnamecrypto.la @NDN_LIBS@ @OPENSSL_LIBS@
_pyndn_la_LDFLAGS = -avoid-version -module @PYTHON_LDFLAGS@ @NDN_LDFLAGS@ @OPENSSL_LDFLAGS@

.h.h.gch:
	$(CC) -c $<

SUFFIXES = .h .h.gch
