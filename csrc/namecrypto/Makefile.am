noinst_LTLIBRARIES = libnamecrypto.la

noinst_HEADERS = \
	authentication.h \
	encryption.h \
	toolkit.h

libnamecrypto_la_SOURCES = \
	authentication.c \
	encryption.c \
	toolkit.c

libnamecrypto_la_CFLAGS  = -std=c99 -Wall -Wextra -Winvalid-pch -Wstrict-prototypes
libnamecrypto_la_CFLAGS += -Wmissing-prototypes -Wshadow -fdiagnostics-show-option
libnamecrypto_la_CFLAGS += -Wno-unknown-pragmas
libnamecrypto_la_CPPFLAGS = @NDN_INCLUDES@
libnamecrypto_la_LIBADD = @NDN_LIBS@
