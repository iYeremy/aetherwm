# vxwm - Versatile X Window Manager
# See LICENSE file for copyright and license details.
#
# Targets:
#   all       build the vxwm binary
#   check     verify every source compiles with the strict flags (no warnings)
#   install   install vxwm, rvx and the man page
#   uninstall remove installed files
#   dist      build a source tarball
#   clean     remove build artifacts

include config.mk

SRC = drw.c vxwm.c util.c \
	modules/autostart/autostart.c \
	modules/betterresize/betterresize.c \
	modules/bsp/bsp.c \
	modules/directionalfocus/directionalfocus.c \
	modules/directionalmove/directionalmove.c \
	modules/etf/etf.c \
	modules/ewmh_tags/ewmh_tags.c \
	modules/externalbars/externalbars.c \
	modules/fullscreen/fullscreen.c \
	modules/gaps/gaps.c \
	modules/infinitetags/infinitetags.c \
	modules/moveresizekbd/moveresizekbd.c \
	modules/warptoclient/warptoclient.c \
	modules/windowmap/windowmap.c \
	modules/xrdb/xrdb.c \
	modules/zoom/zoom.c
OBJ = ${SRC:.c=.o}

all: vxwm

.c.o:
	${CC} -c -o $@ ${CFLAGS} $<

${OBJ}: config.h config.mk

config.h: config.def.h
	cp config.def.h $@

vxwm: ${OBJ}
	${CC} -o $@ ${OBJ} ${LDFLAGS}

# Re-parse every translation unit with the strict flags. Warnings are errors,
# so any violation fails the build.
check: config.h
	@for f in ${SRC}; do \
		echo "  checking $$f"; \
		${CC} -fsyntax-only ${CFLAGS} $$f || exit 1; \
	done
	@echo "OK: no warnings."

clean:
	rm -f vxwm ${OBJ} vxwm-${VERSION}.tar.gz

dist: clean
	mkdir -p vxwm-${VERSION}
	cp -R LICENSE Makefile README.md config.def.h config.mk\
		vxwm.1 drw.h util.h ${SRC} rvx modules vxwm-${VERSION}
	tar -cf vxwm-${VERSION}.tar vxwm-${VERSION}
	gzip vxwm-${VERSION}.tar
	rm -rf vxwm-${VERSION}

install: all
	mkdir -p ${DESTDIR}${PREFIX}/bin
	cp -f vxwm ${DESTDIR}${PREFIX}/bin/vxwm
	chmod 755 ${DESTDIR}${PREFIX}/bin/vxwm
	cp -f rvx ${DESTDIR}${PREFIX}/bin/rvx
	chmod 755 ${DESTDIR}${PREFIX}/bin/rvx
	mkdir -p ${DESTDIR}${MANPREFIX}/man1
	sed "s/VERSION/${VERSION}/g" < vxwm.1 > ${DESTDIR}${MANPREFIX}/man1/vxwm.1
	chmod 644 ${DESTDIR}${MANPREFIX}/man1/vxwm.1

uninstall:
	rm -f ${DESTDIR}${PREFIX}/bin/vxwm\
		${DESTDIR}${PREFIX}/bin/rvx\
		${DESTDIR}${MANPREFIX}/man1/vxwm.1

.PHONY: all check clean dist install uninstall
