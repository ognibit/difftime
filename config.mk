# version
BIN=difftime
VERSION = 1.0.0

# Customize below to fit your system

# paths
PREFIX = /usr/local
MANPREFIX = ${PREFIX}/share/man

# includes and libs
INCS = -I. -I/usr/include
LIBS = -L/usr/lib 

# flags
PRGFLAGS = -DVERSION=\"${VERSION}\" 
CFLAGS = -std=c99 -pedantic -Wall -g ${INCS} ${PRGFLAGS}
LDFLAGS = -s ${LIBS}

# compiler and linker
CC = gcc
