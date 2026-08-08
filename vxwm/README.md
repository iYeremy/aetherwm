# vxwm

Window manager of `aetherwm`, rewritten on top of vanilla dwm 6.7 as a clean
base. Every feature of the original `vxwmdotfiles` fork has been migrated into
self-contained modules under `modules/`. Compiles with
`-Werror -Wall -Wextra`; `make check` re-verifies each source file with the
strict flags. The migration plan lives in `../docs/ANALISIS.md`.

## Estructura

```
vxwm/
├── vxwm.c           # núcleo (fork de dwm 6.7, renombrado)
├── vxwm.h           # API del núcleo expuesta a los módulos
├── drw.c / drw.h    # librería de dibujo (drw)
├── util.c / util.h  # utilidades (die, ecalloc, MAX/MIN/LENGTH)
├── config.def.h     # plantilla de configuración (edítala)
├── config.h         # copia activa, generada por `make` a partir de config.def.h
├── config.mk        # versión, flags estrictos, librerías X
├── Makefile         # all / check / install / uninstall / dist
├── rvx              # utilidad de reinicio
├── vxwm.1           # página man
├── LICENSE          # MIT (dwm)
└── modules/         # framework de módulos
    ├── modules.h    # fuente única de los switches 0/1 de cada módulo
    ├── MODULES.md   # contrato: cómo escribir un módulo
    └── <módulo>/    # un .c + .h (TU separado) por módulo
```

## Compilar

```sh
make && make check   # check re-verifica cada fuente con los flags estrictos
sudo make install    # instala vxwm, rvx y vxwm.1
```

## Configurar

Edita `config.def.h` (teclas, colores, layouts, reglas) y activa/desactiva
funcionalidades con los switches `0/1` de `modules/modules.h`. Los cambios se
aplican recompilando:

```sh
make clean install
```

`make` regenera `config.h` desde `config.def.h` automáticamente.

## Módulos migrados

`autostart`, `betterresize`, `directionalfocus`, `directionalmove`, `etf`,
`ewmh_tags`, `externalbars`, `fullscreen`, `gaps`, `infinitetags`,
`moveresizekbd`, `warptoclient`, `windowmap`, `xrdb`, `zoom`. Ver
`modules/MODULES.md` para el contrato y `docs/ANALISIS.md` para el mapeo de
comportamiento vs. el original.

## Reiniciar

```sh
rvx        # reinicia vxwm
rvx -d     # reinicia dentro de un dbus-run-session nuevo
```
