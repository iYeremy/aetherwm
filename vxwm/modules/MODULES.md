# Contrato de módulos

Este documento define cómo se estructuran los módulos de `vxwm`. **Estado actual:
Fase 2 completa** — los módulos del núcleo existen, están cableados en `vxwm.c`
y el proyecto compila y enlaza con todos activados o con cualquier combinación
de desactivados.

## Principio

A diferencia del `vxwm` original (que inyectaba los módulos con `#include` en un
único translation unit), aquí cada módulo es un **`.c`/`.h` compilado por
separado** y enlazado con el núcleo. El núcleo (`vxwm.c`) expone una API mínima
en `vxwm.h` y los módulos se registran contra ella.

## Estructura de un módulo

```
modules/<nombre>/
├── <nombre>.c      # implementación (TU separado)
├── <nombre>.h      # API pública del módulo + documentación
└── README.md       # opcional: comportamiento, keybinds, dependencias
```

## Contrato que debe cumplir cada módulo

1. **`#include` antes del `#if`.** El `.c` incluye `vxwm.h` (y su `.h`) en la
   primera línea, ANTES de la guarda `#if MODULE`. Las macros de activación se
   definen en `modules/modules.h` vía `vxwm.h`; si la guarda precediera al
   include, el `#if` evaluaría a 0 y el TU se descartaría en silencio.
2. **TU nunca vacío.** Cuando el módulo está desactivado el cuerpo queda
   dentro del `#if 0`; un `typedef int vxwm_<modulo>_module_present;` tras el
   `#endif` mantiene el TU válido (ISO C prohíbe los TU vacíos y `-Werror`
   los rechaza). El Makefile compila siempre todos los `.c` y nunca necesita
   editarse.
3. **Solo API de `vxwm.h`.** Los módulos usan lo que declara `vxwm.h`
   (`dpy`, `root`, `selmon`, `scheme`, `tags`, `ntags`, ...) y las funciones
   del núcleo; nunca `#include "vxwm.c"`.
4. **Macro de activación.** Cada módulo define su propia macro `0/1` en
   `modules/modules.h` (fuente única; `vxwm.h` y `drw.h` la incluyen). Al
   añadir un módulo hay que añadirla aquí y su `.c` al `SRC` del Makefile.
5. **`<nombre>_init` para hooks de arranque.** El núcleo llama a
   `setup()`-time hooks como `loadxrdb()` y `ewmh_tags_init()`.
6. **Sin lógica en el núcleo.** Todo el comportamiento vive en el módulo; el
   núcleo solo llama a los hooks opcionales declarados en `vxwm.h`.

## Checklist para añadir un módulo nuevo

1. Crear `modules/<nombre>/<nombre>.{c,h}` cumpliendo el contrato.
2. Añadir la macro de activación a `modules/modules.h`.
3. Añadir `<nombre>.c` al `SRC` del Makefile.
4. Declarar en `vxwm.h` (o `vxwm.c`) los símbolos que el módulo consume y
   llamar los hooks del módulo desde el núcleo.
5. Documentar keybinds/reglas en `config.def.h` si aplica.
6. Ejecutar `make && make check` (obligatorio, sin warnings).

## Fase 2: módulos del núcleo (implementada y cableada)

| Módulo | Macro | Hooks en el núcleo |
|---|---|---|
| `windowmap` | `WINDOWMAP` | `showhide()` mapea/desmapea en vez de mover fuera de pantalla |
| `gaps` | `GAPS` | `gaps_tile` en `layouts[]`; `setgaps` en keybinds; `createmon()` init |
| `fullscreen` | `FULLSCREEN` | keybind `MOD+Shift+F` |
| `xrdb` | `XRDB` | `setup()` y keybind `MOD+Shift+R` recargan esquemas |
| `etf` | `ENHANCED_TOGGLE_FLOATING` | `manage()`, `setlayout()`, `configurerequest()` |
| `ewmh_tags` | `EWMH_TAGS` | `setup()`, `view()`, `toggleview()`, `focusmon()`, `manage()` |
| `externalbars` | `EXTERNAL_BARS` | `propertynotify()`, `updatebarpos()`, `createmon()` |

`modules/modules.h` incluye además los switches auxiliares
`FLOATING_LAYOUT_FLOATS_WINDOWS`, `RESTORE_SIZE_AND_POS_ETF` y la regla de
dependencias que fuerza `FLOATING_LAYOUT_FLOATS_WINDOWS` cuando
`ENHANCED_TOGGLE_FLOATING` está activo.

## Fases pendientes (desde docs/ANALISIS.md)

- **Fase 3** — infinitetags (el más complejo; README de módulo propio).
- **Fase 4** — interacción: moveresizekbd, directionalfocus, directionalmove,
  betterresize, zoom, warp, autostart.
