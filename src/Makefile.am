bin_PROGRAMS = shadowvpn

lib_LTLIBRARIES = libshadowvpn.la

libshadowvpn_la_SOURCES = log.cc \
	log.h \
	crypto_secretbox_salsa208poly1305.h \
	crypto_secretbox_salsa208poly1305.cc \
	crypto.h \
	crypto.cc \
	shell.h \
	shell.cc \
	vpn.h \
	vpn.cc \
	args.h \
	args.cc \
	shadowvpn.h 

shadowvpn_SOURCES = main.cc

shadowvpn_LDADD = libshadowvpn.la
