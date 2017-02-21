bin_PROGRAMS = shadowvpn

lib_LTLIBRARIES = libshadowvpn.la

libshadowvpn_la_SOURCES = log.c \
	log.h \
	crypto_secretbox_salsa208poly1305.h \
	crypto_secretbox_salsa208poly1305.c \
	crypto.h \
	crypto.c \
	shell.h \
	shell.c \
	vpn.h \
	vpn.c \
	args.h \
	args.c \
	shadowvpn.h 

shadowvpn_SOURCES = main.c

shadowvpn_LDADD = libshadowvpn.la
