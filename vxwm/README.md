# vxwm

Window manager de `aetherwm`, reescrito sobre dwm 6.7 (vanilla) como base
limpia. **Estado: Fase 1 (esqueleto).** Compila sin warnings con
`-Werror -Wall -Wextra`; aún no incluye los módulos del vxwm original
(migración prevista en fases 2–4, ver `../docs/ANALISIS.md`).

## Estructura

```
vxwm/
├── vxwm.c           # núcleo (fork de dwm 6.7, renombrado)
├── drw.c / drw.h    # librería de dibujo (drw)
├── util.c / util.h  # utilidades (die, ecalloc, MAX/MIN/LENGTH)
├── config.def.h     # plantilla de configuración (edítala)
├── config.h         # copia activa, generada por el Makefile
├── config.mk        # versión, flags estrictos, librerías X
├── Makefile         # all / check / install / uninstall / dist
├── rvx              # utilidad de reinicio
├── vxwm.1           # página man
├── LICENSE          # MIT (dwm)
└── modules/         # esqueleto del framework de módulos (contrato)
```

## Compilar

```sh
make && make check   # check re-verifica cada fuente con los flags estrictos
sudo make install    # instala vxwm, rvx y vxwm.1
```

## Configurar

Edita `config.def.h` (teclas, colores, layouts, reglas). Los cambios se aplican
recompilando:

```sh
make clean install
```

Si prefieres mantener tu config bajo git, copia `config.def.h` a `config.h` y
edita `config.h` (está en `.gitignore` del repositorio).

## Reiniciar

```sh
rvx        # reinicia vxwm
rvx -d     # reinicia dentro de un dbus-run-session nuevo
```
