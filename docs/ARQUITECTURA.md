# Arquitectura de vxwm

Este documento describe la arquitectura interna del gestor de ventanas de
`aetherwm` (`vxwm/`). Para el contrato de módulos en detalle ver
`vxwm/modules/MODULES.md`; para el análisis del estado original que motivó la
reescritura ver `ANALISIS.md`.

## Visión general

`vxwm` es un fork de dwm 6.7 reescrito con el núcleo limpio en `vxwm.c` y las
funcionalidades del *vxwm* original (tags infinitos, gaps, fullscreen, xrdb,
ETF, focus/move direccional, ...) migradas a **módulos autónomos** bajo
`modules/`. Cada módulo es una unidad de compilación (TU) separada que enlaza
contra la API mínima que expone `vxwm.h`.

```
                    +---------------------+
                    |        X server     |
                    +---------------------+
                              ^
        Xlib (dpy, root)      |
                              |
   +--------------------------------------------------------+
   |                        vxwm.c                          |
   |   setup()  scan()  run()  manage()  arrange() ...      |
   |   handler[LASTEvent]  (dispatch de eventos)            |
   |   expone API en vxwm.h (dpy, root, selmon, scheme, ...)|
   +--------------------------------------------------------+
                     |              |               |
        modules (TU separadas, compiladas por separado)
   +---------+  +----------+  +----------+  +--------------+
   | gaps    |  | fullscreen|  | xrdb     |  | infinitetags |
   | etf     |  | windowmap |  | ewmh     |  | moveresizekbd|
   | warp    |  | autostart |  | zoom     |  | directionalf*|
   +---------+  +----------+  +----------+  +--------------+
```

## Ciclo de arranque

```
main(argc, argv)
 ├─ parse args: -v | [-ignoreautostart]
 ├─ setlocale + XSupportsLocale
 ├─ XOpenDisplay(NULL)          → dpy
 ├─ checkotherwm()              → aborta si ya hay otro WM (X error handler)
 ├─ setup()                     → init de todo (ver abajo)
 ├─ scan()                      → adopta ventanas ya existentes en root
 ├─ runautostart()  [AUTOSTART] → ejecuta la lista `autostart[]` de
 │   modules/autostart/autostart.c (vacía por defecto; `-ignoreautostart` la salta)
 └─ run()                       → bucle de eventos
```

`setup()` hace, en orden:

1. `XSetErrorHandler` con `xerrorstart` (para detectar otro WM durante setup).
2. Configura `screen`, `root`, `sw`/`sh`, cursor de espera.
3. Selecciona máscaras en root: `SubstructureRedirectMask |
   SubstructureNotifyMask | ButtonPressMask | StructureNotifyMask`.
4. `loadxrdb()` ([XRDB]) si aplica → crea `scheme[]` (esquemas de color) y
   `drw` (fuentes).
5. Inicializa `drw` con las fuentes de `config.def.h`.
6. `createmon()` para cada monitor (Xinerama); inicializa `gappx`, `mfact`,
   `nmaster`, `showbar`, `lt`, `tagset`, `canvas`.
7. `updategeom()` → posiciones de trabajo (`m->wx, wy, ww, wh`).
8. `updatebars()` → crea las ventanas de barra por monitor.
9. Registra los grabs de teclado y ratón (keybindings de `config.def.h`).
10. `XSelectInput` en `root` + define las propiedades EWMH
    ([EWMH_TAGS]) y `_NET_SUPPORTED` ([EWMH], núcleo).

## Bucle de eventos

```
run()
  while (running && XNextEvent(dpy, &ev))
      handler[ev.type](&ev)
```

`handler[]` es una tabla de punteros indexada por tipo de evento X:

```
ButtonPress   → buttonpress
ClientMessage → clientmessage        (EWMH: _NET_WM_*, _NET_ACTIVE_...)
ConfigureRequest → configurerequest
ConfigureNotify  → configurenotify
DestroyNotify    → destroynotify
EnterNotify      → enternotify       (focus al mover el ratón)
Expose           → expose            (redibuja la barra)
FocusIn          → focusin
KeyPress         → keypress
MappingNotify    → mappingnotify     (re-grab al cambiar teclado)
MapRequest       → maprequest        → manage()
MotionNotify     → motionnotify      (drag)
PropertyNotify   → propertynotify    (títulos, WM_HINTS, ...)
UnmapNotify      → unmapnotify       → unmanage()
```

El bucle es síncrono y de un solo hilo: cada evento se procesa hasta completar.
No hay colas internas; el rendimiento es el típico de dwm.

## Gestión de ventanas

`manage(Window w, XWindowAttributes *wa)` es el punto de entrada para toda
ventana nueva (`MapRequest`). Flujo resumido:

```
manage()
 ├─ descarta override-redirect y ventanas no-mapa
 ├─ alloc Client c; rellena: win, x, y, w, h (con hint de tamaño)
 ├─ applysizehints(c)            → respeta hints del programa
 ├─ c->tags = (wa->x == w && wa->y == h) ? 0 : c->mon->tagset[...]
 ├─ setclientstate(c, NormalState)
 ├─ windowtype: transiente / dialog  → isfloating=1   (siempre flotantes)
 ├─ updatewmhints / updatesizehints / updatewindowtype / updatetitle
 ├─ XSelectInput(c->win, ...)    → eventos de la ventana
 ├─ wmatom[WMProtocols] + WM_DELETE_WINDOW
 ├─ reglas de config.h (per-clase: tags, isfloating, monitor)
 ├─ [INFINITE_TAGS] coloca la ventana en el canvas:
 │     - centrada si CENTER_NEW_FLOATING_WINDOWS (layout flotante)
 │     - o bajo el cursor si NEW_FLOATING_WINDOWS_APPEAR_UNDER_CURSOR
 │     - si no hay sel: enfoca y arrastra (focus + togglefloating)
 ├─ attach(c) / attachstack(c)
 ├─ [WINDOWMAP] respeta WM_STATE (iconified → no mapear)
 ├─ EWMH: _NET_CLIENT_LIST, _NET_CLIENT_LIST_STACKING
 ├─ [EXTERNAL_BARS] update de barras externas
 ├─ arrange(selmon) + restack + focus
```

## Estructuras de datos clave

- **`Monitor`** — un monitor (o virtual con Xinerama): `m->wx, wy, ww, wh`
  (área de trabajo tras gaps/barra), `m->clients` (lista), `m->stack`
  (orden de apilado), `m->tagset[2]` (tags visibles, con historia para
  `view`/`toggleview`), `m->lt[2]` + `sellt` (layout actual/anterior),
  `m->nmaster`, `m->mfact`, `m->showbar`, y por módulo: `m->gappx` [GAPS],
  `m->canvas[ntags]` [INFINITE_TAGS] (origen del canvas por tag).
- **`Client`** — una ventana gestionada: `c->win`, geometría `x/y/w/h`,
  `c->bw` (borde), `c->tags`, `c->isfloating`, `c->isfullscreen`,
  `c->is_pinned` [INFINITE_TAGS], `c->sfx/sfy/sfw/sfh` [ETF],
  `c->wasmanuallyedited` [ETF], lista enlazada `c->next`, orden `c->snext`.
- **`Layout`** — `{ symbol, arrange() }`. `tile()`, `monocle()`, y
  `gaps_tile()` [GAPS]; `NULL` significa *flotante*. El primer entry de
  `layouts[]` en `config.def.h` es el layout por defecto.
- **`handler[]`** — tabla de dispatch de eventos (ver arriba).
- **`keys[]` / `buttons[]`** — keybindings y botones de `config.def.h`.
  `keypress` recorre `keys[]` buscando la primera coincidencia de
  `modifier + keysym`; las duplicadas posteriores se ignoran.

## Contrato de módulos

Resumen del contrato documentado en `modules/MODULES.md`:

1. El `.c` del módulo incluye `vxwm.h` (y su `.h`) **antes** de su `#if MODULE`;
   si la guarda fuera anterior, la macro valdría 0 y el TU se descartaría.
2. El TU nunca queda vacío: tras `#endif` hay un `typedef int vxwm_<mod>_present;`.
3. Los módulos usan solo la API de `vxwm.h` (`dpy`, `root`, `selmon`, `scheme`,
   `tags`, `ntags`, `updatebarpos`, `drawbar`, ...). Nunca `#include "vxwm.c"`.
4. Cada módulo tiene su macro `0/1` en `modules/modules.h` (fuente única,
   incluida vía `vxwm.h` y `drw.h`).
5. El Makefile compila siempre todos los `*.c` de `modules/` (sin editarse al
   añadir módulos).

### Cómo agregar un módulo

```
1. mkdir modules/mi_modulo
2. modules/mi_modulo/mi_modulo.h      → API pública + documentación
3. modules/mi_modulo/mi_modulo.c      → implementación (TU separado)
     #include "vxwm.h"
     #include "mi_modulo.h"
     #if MI_MODULO
     ... implementación ...
     #endif
     typedef int vxwm_mi_modulo_present;
4. Añadir  #define MI_MODULO 1  en modules/modules.h
5. Añadir  #include "modules/mi_modulo/mi_modulo.h"  en vxwm.c (o drw.h)
   y cualquier hook en el núcleo (manage, arrange, keypress, ...).
6. make && make check
```

## Colores y fuentes (xrdb / pywal)

- `scheme[SchemeLast]` es un array global de `Clr *` (drw). El núcleo dibuja la
  barra con estos esquemas.
- Con [XRDB], `loadxrdb()` relee `~/.Xresources` (o el root) y reconstruye
  `scheme` y `drw`. La keybind `Super+F5` lo invoca en caliente; el script
  `wallpaper.sh` (pywal) hace `xrdb -merge` + `xdotool key Super+F5` para
  aplicarlo sin reiniciar.
- La barra admite [BAR_HEIGHT] (altura fija) y [BAR_PADDING] (insets laterales
  → barra "pastilla" centrada). `positionbarwin()` y `drawbar()` respetan estos
  valores en tiempo de ejecución.

## Flujo de la barra (bottom, pastilla)

```
updatebarpos(m)                  → calcula rect de la barra (abajo, con pads)
positionbarwin(m)                → XMoveResizeWindow de la ventana de barra
drawbar(m)                       → pixmap: tags | ltsymbol | título | status
updatebars()                     → crea las barras por monitor al arrancar
```

Con `BAR_ALWAYS_ON_TOP`/`EXTERNAL_BARS_ALWAYS_ON_TOP` la barra se mantiene por
encima de las ventanas (override-redirect + grab del área).
