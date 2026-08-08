# Contrato de módulos

Este documento define cómo se estructuran los módulos de `vxwm`. **Estado actual:
Fases 1 a 4 completas** — todos los módulos del núcleo, infinitetags y módulos de
interacción están completamente implementados, modularizados en su propio `.c`/`.h`,
cableados en `vxwm.h`/`vxwm.c` y verificados con `make check`.

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

## Catálogo completo de módulos (Fases 2, 3 y 4)

| Módulo | Macro | Descripción / Hooks |
|---|---|---|
| `windowmap` | `WINDOWMAP` | Mapea/desmapea ventanas en vez de moverlas fuera de pantalla |
| `gaps` | `GAPS` | Layout `gaps_tile`, insets y gaps dinámicos con `setgaps` |
| `fullscreen` | `FULLSCREEN` | Modo pantalla completa con `togglefullscreen` |
| `xrdb` | `XRDB` | Recarga de colores en caliente desde Xresources con `loadxrdb` |
| `etf` | `ENHANCED_TOGGLE_FLOATING` | Redimensionado/restauración inteligente de ventanas flotantes |
| `ewmh_tags` | `EWMH_TAGS` | Publica propiedades EWMH de escritorios para barras externas |
| `externalbars` | `EXTERNAL_BARS` | Respeta struts (_NET_WM_STRUT) de paneles externos |
| `infinitetags` | `INFINITE_TAGS` | Canvas infinito deslizable por tag, coordenadas en barra, pinning |
| `moveresizekbd` | `MOVE_RESIZE_WITH_KEYBOARD` | Mover y redimensionar ventanas flotantes por teclado |
| `directionalfocus` | `DIRECTIONAL_FOCUS` | Enfoque direccional de ventanas con `focusdir` |
| `directionalmove` | `DIRECTIONAL_MOVE` | Movimiento direccional de ventanas en tiled con `movedir` |
| `betterresize` | `BR_CHANGE_CURSOR` | Redimensionado de ventanas desde 8 bordes/esquinas con cambio de cursor |
| `zoom` | `ZOOM` | Integración con vcompmgr para zoom |
| `warptoclient` | `WARP_TO_CLIENT` | Warp del puntero al foco/evento de ventana |
| `autostart` | `AUTOSTART` | Ejecución de comandos de arranque al iniciar la sesión |

`modules/modules.h` gestiona además las reglas de dependencias (por ejemplo, `INFINITE_TAGS` fuerza `WINDOWMAP`, `ENHANCED_TOGGLE_FLOATING` fuerza `FLOATING_LAYOUT_FLOATS_WINDOWS`).

