noinst_LTLIBRARIES = libnamecrypto.la

libnamecrypto_la_SOURCES = \
	authentication.c \
	encryption.c \
	toolkit.c

libnamecrypto_la_CPPFLAGS = -std=c99 -Wall -Wextra -Winvalid-pch -Wstrict-prototypes
libnamecrypto_la_CPPFLAGS += -Wmissing-prototypes -Wshadow -fdiagnostics-show-option
libnamecrypto_la_CPPFLAGS += -Wno-unknown-pragmas @NDN_INCLUDES@
libnamecrypto_la_LIBADD = @NDN_LIBS@
#libnamecrypto_la_LDFLAGS = -static
