# lsel - line selector
# See LICENSE file for copyright and license details.

include config.mk

SRC = difftime.c
OBJ = ${SRC:.c=.o}

all: options ${BIN}

options:
	@echo ${BIN} build options:
	@echo "CFLAGS   = ${CFLAGS}"
	@echo "LDFLAGS  = ${LDFLAGS}"
	@echo "CC       = ${CC}"

.c.o:
	@echo CC $<
	@${CC} -c ${CFLAGS} $<

${OBJ}: config.h config.mk

config.h:
	@echo creating $@ from config.def.h
	@cp config.def.h $@

${BIN}: ${OBJ}
	@echo CC -o $@
	@${CC} -o $@ ${OBJ} ${LDFLAGS}

clean:
	@echo cleaning
	@rm -f ${BIN} ${OBJ} ${BIN}-${VERSION}.tar.gz

dist: clean
	@echo creating dist tarball
	@mkdir -p ${BIN}-${VERSION}
	@cp -R LICENSE Makefile README.md ${BIN}.1 config.mk \
		${SRC} config.def.h ${BIN}-${VERSION}
	@tar -cf ${BIN}-${VERSION}.tar ${BIN}-${VERSION}
	@gzip ${BIN}-${VERSION}.tar
	@rm -rf ${BIN}-${VERSION}

install: all
	@echo installing executable file to ${DESTDIR}${PREFIX}/bin
	@mkdir -p ${DESTDIR}${PREFIX}/bin
	@cp -f ${BIN} ${DESTDIR}${PREFIX}/bin
	@chmod 755 ${DESTDIR}${PREFIX}/bin/${BIN}
	@echo installing manual page to ${DESTDIR}${MANPREFIX}/man1
	@mkdir -p ${DESTDIR}${MANPREFIX}/man1
	@sed "s/VERSION/${VERSION}/g" <${BIN}.1 >${DESTDIR}${MANPREFIX}/man1/${BIN}.1
	@chmod 644 ${DESTDIR}${MANPREFIX}/man1/${BIN}.1

uninstall:
	@echo removing executable file from ${DESTDIR}${PREFIX}/bin
	@rm -f ${DESTDIR}${PREFIX}/bin/${BIN}
	@echo removing manual page from ${DESTDIR}${MANPREFIX}/man1
	@rm -f ${DESTDIR}${MANPREFIX}/man1/${BIN}.1

.PHONY: all options clean dist install uninstall
