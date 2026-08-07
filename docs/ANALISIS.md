# Análisis completo de `vxwmdotfiles` y hoja de ruta para `aetherwm`

> Documento de control de lectura. Analiza en profundidad el proyecto original
> (`~/vxwmdotfiles`), documenta su arquitectura, detecta bugs y deuda técnica, y
> define la estrategia de reescritura con buenas prácticas en `aetherwm`.
>
> **Fecha:** 2026-08-07 · **Alcance:** 100% del repositorio (WM + configs + scripts).

---

## Índice

1. [Resumen ejecutivo](#1-resumen-ejecutivo)
2. [Estructura del proyecto actual](#2-estructura-del-proyecto-actual)
3. [El Window Manager (vxwm) — análisis a fondo](#3-el-window-manager-vxwm--análisis-a-fondo)
   - 3.1 [Origen y filosofía](#31-origen-y-filosofía)
   - 3.2 [Modelo de construcción](#32-modelo-de-construcción)
   - 3.3 [Flujo de arranque y ciclo de vida](#33-flujo-de-arranque-y-ciclo-de-vida)
   - 3.4 [Estructuras de datos centrales](#34-estructuras-de-datos-centrales)
   - 3.5 [Mapa de funciones del núcleo](#35-mapa-de-funciones-del-núcleo)
   - 3.6 [Sistema de módulos (compile-time)](#36-sistema-de-módulos-compile-time)
   - 3.7 [Catálogo de módulos](#37-catálogo-de-módulos)
   - 3.8 [Tags infinitos (INFINITE_TAGS)](#38-tags-infinitos-infinite_tags)
   - 3.9 [La barra de estado](#39-la-barra-de-estado)
   - 3.10 [Temas y color (XRDB / pywal)](#310-temas-y-color-xrdb--pywal)
4. [Configuraciones del entorno](#4-configuraciones-del-entorno)
5. [Scripts: instalador y auxiliares](#5-scripts-instalador-y-auxiliares)
6. [Bugs concretos detectados](#6-bugs-concretos-detectados)
7. [Deuda técnica y problemas de arquitectura](#7-deuda-técnica-y-problemas-de-arquitectura)
8. [Propuesta de reescritura y mapeo a `aetherwm`](#8-propuesta-de-reescritura-y-mapeo-a-aetherwm)
9. [Hoja de ruta sugerida](#9-hoja-de-ruta-sugerida)

---

## 1. Resumen ejecutivo

`vxwmdotfiles` (alias **NyxDot**) es un conjunto de dotfiles para un entorno de
escritorio X11 **minimalista**, con sincronización de color vía `pywal` y
centrado en un window manager propio llamado **vxwm** — un fork de
`dwm` (suckless) que añade decenas de features mediante un sistema de módulos
activables en tiempo de compilación (estilo *dwm-flexipatch*).

La pieza central es el WM, y su **feature insignia** es el sistema de
**tags infinitos** (`INFINITE_TAGS`): en lugar de "páginas" discretas, las
ventanas viven en un *canvas* virtual infinito y el usuario desliza su
viewport por él. El resto del repo son configuraciones de `kitty`, `picom`,
`rofi`, `dunst`, `fish` y un set de scripts de utilidad (volumen, brillo,
capturas, selector de fondos) más un instalador automatizado para Arch Linux.

**Veredicto general:** el proyecto es funcional, ingenioso y ligero, pero
adolece de:
- Un WM monolítico (2820 líneas) donde los módulos se inyectan con `#include`
  en medio de un único `translation unit`.
- Bugs concretos y verificables (sección 6).
- Configuraciones "solo para el autor" (rutas hardcodeadas, scripts con
  comentarios en idioma mezclado, dependencias instaladas vs. usadas).
- Documentación casi inexistente (README corto, man page desactualizado).
- Artefactos de compilación commiteados al git (`.o`, binario `vxwm`).

**Objetivo de aetherwm:** reproducir la misma funcionalidad con arquitectura
legible, módulos con contrato claro, documentación completa, scripts
portables/robustos y un instalador idempotente y seguro.

---

## 2. Estructura del proyecto actual

```
vxwmdotfiles/
├── README.md                  # Descripción breve + instalación (Arch only)
├── install.sh                 # Instalador automatizado (pacman + AUR + deploy)
├── .xinitrc                   # Inicialización de sesión X
├── myvxwm/                    # ── EL WINDOW MANAGER (código C) ──
│   ├── vxwm.c                 # Núcleo monolítico (2820 líneas)
│   ├── config.h               # Config activa (generada desde config.def.h)
│   ├── config.def.h           # Plantilla de configuración
│   ├── modules.h              # Switches de módulos activos
│   ├── modules.def.h          # Plantilla de switches
│   ├── config.mk              # VERSION, flags, rutas X11, compilador
│   ├── Makefile               # Build/install/dist/clean
│   ├── drw.c / drw.h          # Librería de dibujo (dwm)
│   ├── util.c / util.h        # Utilidades (die, ecalloc, macros MIN/MAX…)
│   ├── vxwm.1                 # Página man (desactualizada)
│   ├── rvx                    # Utilidad de reinicio del WM
│   ├── LICENSE                # Licencia del fork (MIT-style)
│   ├── LICENSE.dwm            # Licencia original de dwm
│   ├── vxwm / *.o             # ⚠️ Binario y objetos commiteados
│   └── modules/               # ── 16 módulos opcionales ──
│       ├── vxwm_includes.h    # #include condicional de los .h
│       ├── vxwm_includes.c    # #include condicional de los .c
│       ├── autostart/  betterresize/  directionalfocus/
│       ├── directionalmove/  etf/  ewmh_tags/  externalbars/
│       ├── fullscreen/  gaps/  infinitetags/  moveresizekbd/
│       ├── warptoclient/  windowmap/  xrdb/  zoom/
├── fish/                      # config.fish + fish_variables
├── kitty/kitty.conf           # Terminal (transparencia, nerd font, wal)
├── picom/picom.conf           # Compositor + animaciones (animations/)
├── rofi/                      # config.rasi, wall-changer.rasi, wallpaper.rasi
├── dunstrc                    # Notificaciones (base; overwrite por wal-dunst.sh)
├── wal/                       # Cache pywal generado (colores, templates, schemes)
├── wal-dunst.sh               # Regenera dunstrc con colores de wal y reinicia dunst
├── rofi-wallpaper.sh          # Selector de fondos (rofi + wal + xrdb reload)
├── brightness.sh              # Control de brillo + notificación
├── volume.sh                  # Control de volumen (wpctl) + notificación
├── ss.sh                      # Captura de pantalla (scrot) + notificación
├── wallhaven-*.jpg            # Fondos de pantalla incluidos
└── 2026-06-25_*.png           # Capturas de preview del README
```

### Estructura base ya creada en `aetherwm`

```
aetherwm/
├── README.md                  # Vacío (por escribir)
├── install.sh                 # Vacío (por escribir)
├── uninstall.sh               # Vacío (por escribir)
├── .gitignore                 # Ignora *.o
├── config/                    # Configs de apps (destino ~/.config)
│   ├── dunst/  kitty/  picom/  rofi/  wal/
├── scripts/                   # Scripts auxiliares (~/… )
├── vxwm/                      # Código del WM (reenescrito)
└── wallpapers/                # Fondos de pantalla
```

La reescritura reorganiza la estructura actual: las configs que hoy están
sueltas en la raíz pasan a `config/`, los scripts a `scripts/`, los fondos a
`wallpapers/`, y el WM a `vxwm/`.

---

## 3. El Window Manager (vxwm) — análisis a fondo

### 3.1 Origen y filosofía

- Fork de `dwm 6.7`, manteniéndose al día con los cambios de dwm (comentario
  inicial de `vxwm.c`).
- Autor: **wh1tepearl**. Versión del repo: `2.3` (`config.mk`), el header del
  fuente dice `2.2`.
- Inspirado en `5element` y en el compositor Wayland `hevel` para el concepto
  de tags infinitos.
- Filosofía suckless: configurar es **recompilar**, no leer archivos en runtime.
  Configuración en `config.h` (aspecto, teclas, reglas, layouts) y `modules.h`
  (features on/off con `0`/`1`).

### 3.2 Modelo de construcción

El WM se compila como **un único archivo** gracias a la amalgamación:

```c
// vxwm.c
#include "config.h"               // configuración activa
#include "modules/vxwm_includes.c" // #includes condicionales de los módulos
```

El Makefile sigue el patrón dwm:

```make
config.h:  cp config.def.h $@     # solo si no existe
modules.h: cp modules.def.h $@    # solo si no existe
```

- `config.def.h` y `modules.def.h` son las plantillas "limpias" (para
  distribuir). El usuario edita las copias `.h`.
- `SRC = drw.c vxwm.c util.c` → solo se compilan 3 TU; el resto entra por
  `#include`.
- Flags: `-std=c99 -pedantic -Wall`, Xinerama, Xft/fontconfig, X11.
- `make clean install` instala el binario en `${PREFIX}/bin/vxwm` y la man page
  `vxwm.1` (con `sed` de versión). Nota: **instala `rvx` pero el `uninstall`
  sí lo borra; el `install` nunca copia `rvx`** (bug de instalación).

**Cómo se decide el orden de compilación:** `modules/vxwm_includes.c` se
incluye en la **mitad** de `vxwm.c` (línea 354), es decir, *después* de las
declaraciones de todas las funciones `static` del núcleo (líneas 216–306) y de
las variables globales (311–350). Así los módulos pueden llamar a cualquier
función del núcleo y acceder a `dpy`, `selmon`, `mons`, `tags`, etc. sin
re-declararlas. Los prototipos de cada módulo están en su `.h`, también
`#include`-ados por `vxwm_includes.h` en la línea 308.

> ⚠️ **Consecuencia arquitectónica:** no existe separación real de
> compilación. Un cambio en `vxwm.c` o en cualquier módulo fuerza recompilar
> todo el TU. El "aislamiento" es solo lógico (condicionales `#if`).

### 3.3 Flujo de arranque y ciclo de vida

```
main()  (vxwm.c:2783)
│
├─ setlocale / XSupportsLocale
├─ XOpenDisplay
├─ checkotherwm()          → detecta si otro WM corre (xerrorstart + SubstructureRedirectMask)
├─ [XRDB] XrmInitialize + loadxrdb()
├─ setup()                 → inicializa todo (ver abajo)
├─ [OpenBSD] pledge
├─ scan()                  → adopta ventanas ya existentes (2 pasadas: normales + transients)
├─ [AUTOSTART] runautostart()  (a menos de -ignoreautostart)
├─ run()                   → bucle principal de eventos
└─ cleanup()               → libera todo y sale
```

**`setup()` (vxwm.c:1963)** hace, en orden:
1. `sigaction(SIGCHLD, SIG_IGN)` → evita zombies; limpia zombies heredados.
2. Captura `screen`, `sw`, `sh`, `root`.
3. Crea `drw` (pixmap + GC) y carga fuentes (`fonts[]`). Si no carga, `die`.
4. Calcula `lrpad` (altura de fuente) y `bh` (altura de barra; si `BAR_HEIGHT`,
   usa `user_bh`).
5. `updategeom()` → crea/ajusta monitores (Xinerama o fallback).
6. Inicializa atoms ICCCM/EWMH (`wmatom[]`, `netatom[]`).
7. Crea cursores (`CurNormal/Resize/Move` + 8 de esquinas si `BETTER_RESIZE &&
   BR_CHANGE_CURSOR`).
8. Crea esquemas de color (`scheme[i]`).
9. `updatebars()` + `updatestatus()`.
10. Crea `wmcheckwin` (ventana de soporte `_NET_SUPPORTING_WM_CHECK`), publica
    `_NET_SUPPORTED`, `[EWMH_TAGS]` desktops/names/viewport.
11. Selecciona eventos en root, `grabkeys()`, `focus(NULL)`.

**`run()` (vxwm.c:1767)** es el bucle clásico de Xlib:

```c
while (running && !XNextEvent(dpy, &ev))
    if (handler[ev.type]) handler[ev.type](&ev);
```

`handler[]` (vxwm.c:327) es una tabla dispatch indexada por tipo de evento que
mapea 13 tipos: ButtonPress, ClientMessage, ConfigureRequest, ConfigureNotify,
DestroyNotify, EnterNotify, Expose, FocusIn, KeyPress, MappingNotify,
MapRequest, MotionNotify, PropertyNotify, UnmapNotify.

### 3.4 Estructuras de datos centrales

| Tipo | Ubicación | Propósito |
|---|---|---|
| `Arg` (union `{i, ui, f, v}`) | vxwm.c:110 | Argumento genérico para keybinds/buttons |
| `Client` | vxwm.c:127 | Una ventana gestionada. Campos geométricos (`x,y,w,h`, `old*`), hints de tamaño (`basew,minw,incw…`), `tags`, flags (`isfixed, isfloating, isurgent, neverfocus, isfullscreen`), enlaces de lista (`next`, `snext`), `mon`, `win`, `name[256]`. Campos condicionales: `INFINITE_TAGS`→`saved_cx/cy/cw/ch`, `was_on_canvas`, `is_pinned`; `WINDOWMAP`→`ismapped`; `ENHANCED_TOGGLE_FLOATING`→`sfx/sfy/sfw/sfh`, `wasmanuallyedited` |
| `Key` | vxwm.c:157 | `mod`, `keysym`, `func`, `arg` |
| `Button` | vxwm.c:117 | `click` (zona de la barra), `mask`, `button`, `func`, `arg` |
| `Layout` | vxwm.c:164 | `symbol` + `arrange(Monitor*)` (NULL = flotante) |
| `Monitor` | vxwm.c:176 | Un monitor. Geometría de barra (`by`), pantalla (`mx,my,mw,mh`), área de ventanas (`wx,wy,ww,wh`), `seltags`, `tagset[2]`, `lt[2]`, listas `clients/stack/sel`, `barwin`, y campos condicionales: `GAPS`→`gappx`; `INFINITE_TAGS`→`canvas` (array de `CanvasOffset` por tag); `EXTERNAL_BARS`→struts |
| `CanvasOffset` | vxwm.c:169 | `cx, cy` (y `saved_cx/saved_cy`) = desplazamiento del viewport por tag |
| `Rule` | vxwm.c:206 | Regla de emparejado (`class`, `instance`, `title`, `tags`, `isfloating`, `monitor`) |
| `ExternalBarStrut` | externalbars.h:1 | Estruts `_NET_WM_STRUT(_PARTIAL)` por barra externa |

**Listas:** cada `Monitor` mantiene 3 listas enlazadas de `Client`:
- `clients` (orden de creación/stacking tiled),
- `stack` (orden de enfoque; también usado para el restack y show/hide),
- `sel` (cliente enfocado).

**Tags:** bitmask `unsigned int` (9 tags → máximo 31, validado en tiempo de
compilación por `struct NumTags { char limitexceeded[LENGTH(tags) > 31 ? -1 : 1]; }`
en vxwm.c:357).

### 3.5 Mapa de funciones del núcleo

Cada entrada: **función — ubicación — qué hace**.

**Ciclo de vida de un cliente**
- `manage()` vxwm.c:1258 — adopta una ventana: crea `Client`, lee geometry,
  `updatetitle`, aplica `XGetTransientForHint` (hereda monitor/tags del padre)
  o `applyrules`, clampa a monitor, pone borde, `updatewindowtype/sizehints/
  wmhints`, centra si aplica, selecciona eventos, graba botones, calcula
  `isfloating`, `attach`+`attachstack`, publica `_NET_CLIENT_LIST`,
  mueve la ventana fuera de pantalla (`x + 2*sw`, vxwm.c:1334), `setclientstate
  (NormalState)`, `arrange`, `XMapWindow`, `focus`.
- `unmanage()` vxwm.c:2312 — des-adopta: desengancha listas, restaura borde
  (`oldbw`), `WithdrawnState`, libera memoria, `focus(NULL)`, `updateclientlist`,
  `arrange`, opcionalmente `warptoclient`.
- `unmapnotify()` vxwm.c:2342 — si `send_event` marca `WithdrawnState`, si no
  `unmanage(c, 0)`.
- `destroynotify()` vxwm.c:780 — `unmanage(c, 1)`; `[EXTERNAL_BARS]`
  `externalbars_unregister`.
- `maprequest()` vxwm.c:1358 — ignora override_redirect; si es barra externa la
  registra; si no, `manage`.

**Layout / geometría**
- `tile()` vxwm.c:2177 — layout maestro/stack clásico, con variantes `GAPS`.
- `monocle()` vxwm.c:1378 — maximiza visibles; símbolo `[n]`.
- `setlayout()` vxwm.c:1911 — cambia layout; con `INFINITE_TAGS` además guarda/
  restaura posiciones de canvas al entrar/salir de flotante.
- `setmfact()` vxwm.c:1949 — factor del área maestra (0.05–0.95).
- `incnmaster()` vxwm.c:1176 — nº de maestras; con warp opcional.
- `swapmaster()` vxwm.c:2765 — lleva el cliente enfocado al maestro.
- `resize()` vxwm.c:1610 → `applysizehints()` vxwm.c:397 → `resizeclient()` vxwm.c:1617.
- `applysizehints()` — aplica base/min/aspect/inc/max de hints ICCCM.
- `restack()` vxwm.c:1713 — ordena stacking (ventanas tiled bajo la barra;
  `BAR_ALWAYS_ON_TOP`, barras externas y pinneadas por encima).
- `arrange()` vxwm.c:467 — `showhide` + `arrangemon` + `restack`; con
  `WINDOWMAP` envuelve en `XGrabServer`.
- `arrangemon()` vxwm.c:488 — copia símbolo y llama `arrange` del layout.
- `showhide()` vxwm.c:2089 — muestra/oculta según visibilidad; usa
  `SHOWHIDEPROFILE` (XMoveWindow fuera o unmap según `WINDOWMAP`/animación).

**Enfoque / stacking**
- `focus()` vxwm.c:973 — enfoca cliente: busca primero visible en stack,
  actualiza `selmon->sel`, borde, `setfocus`, `_NET_ACTIVE_WINDOW`, `drawbars`.
- `unfocus()` vxwm.c:2299 — restaura borde y libera input focus.
- `focusstack()` vxwm.c:1022 — navega en stack; con `INFINITE_TAGS` además
  `centerwindow(NULL)`.
- `focusmon()` vxwm.c:1008 — cambia monitor seleccionado.
- `setfocus()` vxwm.c:1873 — `XSetInputFocus` + `WMTakeFocus` + propiedad.
- `focusin()` vxwm.c:999 — corrige clientes que roban el foco.

**Ratón**
- `buttonpress()` vxwm.c:511 — resuelve zona de la barra (tags, símbolo, status,
  título) o cliente; invoca `buttons[]`.
- `movemouse()` vxwm.c:1410 — arrastre (con snap, límites de monitor,
  `MOVE_IN_TILED` para mover entre tiled, zoom divisor, rate-limit).
- `resizemouse()` — la versión vanilla (vxwm.c:1633) se reemplaza por
  `betterresize` si `BETTER_RESIZE` (mejora de 8 lados).
- `motionnotify()` vxwm.c:1393 — cambia monitor al cruzar borde.

**Tags / vistas**
- `view()` vxwm.c:2618 / 2660 — dos variantes por `TAG_TO_TAG` (toggle a tag
  anterior si ya estás en ella). Con `INFINITE_TAGS` guarda/restaura canvas.
- `toggleview()` vxwm.c:2284 — añade/quita tag de la vista.
- `tag()` vxwm.c:2137 — asigna tag al cliente (guarda canvas del tag destino).
- `toggletag()` vxwm.c:2263 — alterna tag en cliente.
- `tagmon()` vxwm.c:2169 — envía cliente a otro monitor (`sendmon`).
- `sendmon()` vxwm.c:1818 — reasigna monitor + tags, `[EWMH_TAGS]` desktop.

**Diversos**
- `spawn()` vxwm.c:2115 — `fork`+`setsid`+`execvp`; ajusta `dmenumon`.
- `killclient()` vxwm.c:1242 — `WM_DELETE_WINDOW`, fallback `XKillClient`.
- `quit()` vxwm.c:1590 — `running = 0`.
- `setfullscreen()` vxwm.c:1883 — entra/sale de fullscreen (`_NET_WM_STATE_FULLSCREEN`,
  borde 0, geometría monitor, restaura `oldx/y/w/h`).
- `togglefloating()` vxwm.c:2249 — alterna flotante.
- `togglebar()` vxwm.c:2222 — muestra/oculta barra (recalcula `by`).
- `seturgent()` vxwm.c:2076 — flag + hint XUrgency.
- `updatewindowtype()` vxwm.c:2585 — dialogs flotan; `_NET_WM_STATE_FULLSCREEN`.
- `updatestatus()` vxwm.c:2568 — lee `XA_WM_NAME` de root (via xsetroot) a `stext`.
- `updatetitle()` vxwm.c:2576 — `_NET_WM_NAME` → `WM_NAME` → "broken".
- `updatewmhints()` vxwm.c:2597 — urgente/neverfocus.
- `updatesizehints()` vxwm.c:2524 — parsea `WM_NORMAL_HINTS`.
- `updatebars()/updatebarpos()/drawbar()/drawbars()/expose()` — barra.
- `updateclientlist()` vxwm.c:2416 — re-publica `_NET_CLIENT_LIST`.
- `updategeom()` vxwm.c:2430 — hotplug de monitores.
- `grabkeys()` vxwm.c:1146 — graba keybinds en root (incl. numlock/lock).
- `grabbuttons()` vxwm.c:1122 — graba botones en el cliente.
- `updatenumlockmask()` vxwm.c:2508 — detecta numlock.
- `checkotherwm()/xerror()/xerrordummy()/xerrorstart()` vxwm.c:571–2762 —
  manejo de errores X y detección de WM previo.
- `wintoclient()/wintomon()/recttomon()/dirtomon()` — lookups.

### 3.6 Sistema de módulos (compile-time)

**Mecánica (flexipatch):**
1. `modules.def.h` define ~40 macros `0/1` agrupadas por temática
   (KILLER FEATURES, Move/Resize, eye candy, Tagging, Bar, Warp to client, Misc,
   Floating).
2. `modules/vxwm_includes.h` hace `#include` condicional de cada `modulo.h`.
3. `modules/vxwm_includes.c` hace `#include` condicional de cada `modulo.c`.
4. El propio `vxwm.c` y `config.h` encierran su lógica con los mismos `#if`.

**Reglas de dependencia (ya forzadas en vxwm.c:40-48):**
```c
#if INFINITE_TAGS && !WINDOWMAP      → fuerza WINDOWMAP = 1
#if ENHANCED_TOGGLE_FLOATING && !FLOATING_LAYOUT_FLOATS_WINDOWS → fuerza FLFW = 1
```
Documentadas también al final de `modules.def.h`. Existen dependencias
**duras no forzadas**: `BETTER_RESIZE` reemplaza a `resizemouse`; `ZOOM` usa
`zoom_value()` en `movemouse`/`movecanvasmouse`/`moveresize`; `DIRECTIONAL_MOVE`
es usado por `moveresize`; `WARP_TO_CLIENT` es prerequisito de todas las opciones
`WARP_TO_*`.

**Para añadir un módulo nuevo hoy haría falta tocar 4 lugares:**
1. `modules.def.h` (y `modules.h`): nueva macro `#define NUEVO 1`.
2. Crear `modules/nuevo/nuevo.c` y `.h`.
3. Añadir `#if NUEVO #include "nuevo/nuevo.h"` en `vxwm_includes.h`.
4. Añadir `#if NUEVO #include "nuevo/nuevo.c"` en `vxwm_includes.c`.
5. (Opcional) prototipos de campos en `Client`/`Monitor`, hooks `#if` en `vxwm.c`,
   keybinds en `config.h`.
Además, los prototipos de módulos suelen declararse `static` en el `.h` pero la
definición en el `.c` **no repite** `static` — legal en C (herencia de linkage)
pero inconsistente y confuso.

### 3.7 Catálogo de módulos

| Módulo | Macro(s) | Archivos | Qué aporta | Hooks en núcleo |
|---|---|---|---|---|
| **Tags infinitos** | `INFINITE_TAGS`, `IT_SHOW_COORDINATES_IN_BAR` | infinitetags/ | Canvas infinito por tag: `movecanvas`, `movecanvasmouse`, `homecanvas`, `centerwindow`, `pinwindow`, `getcurrenttag`, save/restore | `Client` extra fields, `Monitor.canvas`, `view/tag/setlayout/focusstack/incnmaster/drawbar/restack/applysizehints`, keybinds |
| **Zoom** | `ZOOM` | zoom/ | Lee factor de zoom de `/tmp/vcompmgr_$DISPLAY.sock.zoom` (vcompmgr) | `movemouse`, `movecanvasmouse`, `moveresize` dividen por el factor; keybinds spawn `vcompmgr -Z` |
| **Better resize** | `BETTER_RESIZE`, `BR_CHANGE_CURSOR` | betterresize/ | Resize por 8 lados/esquinas con cursor dinámico | Reemplaza `resizemouse`; enum CurNW…CurW |
| **Rate-limit move/resize** | `LOCK_MOVE_RESIZE_REFRESH_RATE` | (núcleo) | Limita a `refreshrate` FPS los eventos de arrastre | `movemouse`, `resizemouse`, `movecanvasmouse` |
| **Move/resize por teclado** | `MOVE_RESIZE_WITH_KEYBOARD` | moveresizekbd/ | Mover/redimensionar con flechas; en tiled delega en `movedir`; en flotante con canvas infinito desplaza el canvas al empujar contra el borde | keybinds `moveresize` con pasos |
| **Gaps** | `GAPS` | gaps/ | Espaciado entre ventanas (`gappx`) | `Monitor.gappx`, `tile()`, keybinds `setgaps` |
| **XRDB** | `XRDB` | xrdb/ | Carga colores desde `Xresources` (`dwm.color0/6`); `MAYBE_CONST` para permitir sobrescritura; cambia firma de `drw_scm_create` | `main` (loadxrdb), keybind `Mod+F5`, recarga schemes |
| **Fullscreen** | `FULLSCREEN` | fullscreen/ | Bind para toggle fullscreen | keybind `togglefullscr` |
| **EWMH tags** | `EWMH_TAGS` | ewmh_tags/ | Publica `_NET_NUMBER_OF_DESKTOPS/CURRENT_DESKTOP/DESKTOP_NAMES/VIEWPORT/_NET_WM_DESKTOP` para bars externas | Atoms extra en `netatom[]`, `setup`, `manage`, `sendmon`, `tag/toggletag/toggleview/view` |
| **Barras externas** | `EXTERNAL_BARS`, `EXTERNAL_BARS_ALWAYS_ON_TOP` | externalbars/ | Detecta barras con `_NET_WM_STRUT(_PARTIAL)`, reserva área en cada monitor | `maprequest`, `scan`, `propertynotify`, `destroynotify`, `unmapnotify`, `updatebarpos`, `restack`, `cleanup` |
| **Warp to client** | `WARP_TO_CLIENT` + 6 `WARP_TO_*` | warptoclient/ | Mueve el cursor al centro de una ventana/contexto | `focusstack`, `incnmaster`, `manage`, `unmanage`, `swapmaster`, `enhancedtogglefloating`, `moveresize`, `focusdir` |
| **Enhanced toggle floating** | `ENHANCED_TOGGLE_FLOATING`, `RESTORE_SIZE_AND_POS_ETF` | etf/ | Toggle flotante "inteligente": recuerda tamaño/pos; al desflotar vuelve a tiled | `Client.sfx/sfy/sfw/sfh`, `wasmanuallyedited`; keybind `Mod+q` |
| **Window map/unmap** | `WINDOWMAP` | windowmap/ | Mapear/desmapear ventanas en vez de moverlas fuera (arregla foco perdido y anima con picom) | `SHOWHIDEPROFILE`, `Client.ismapped`, `arrange` con grab |
| **Foco direccional** | `DIRECTIONAL_FOCUS` | directionalfocus/ | Foco por dirección (Alt+flechas) con scoring geométrico | keybinds `focusdir` |
| **Movimiento direccional** | `DIRECTIONAL_MOVE` | directionalmove/ | Mover cliente tiled en dirección (reordena lista `clients`) | `moveresize`, keybinds `movedir` |
| **Autostart** | `AUTOSTART` | autostart/ | Ejecuta `autostart[]` de config.h al inicio (`-ignoreautostart` lo salta) | `main`, `config.h` |
| **Mover en tiled** | `MOVE_IN_TILED` | (núcleo) | Arrastrar con ratón reordena clientes tiled | `movemouse` |
| **Tag-to-tag** | `TAG_TO_TAG` | (núcleo) | `view` vuelve a la tag anterior si se repite la misma | `view` |

### 3.8 Tags infinitos (INFINITE_TAGS)

Es el corazón conceptual del WM. En lugar de cambiar "páginas", el usuario
desliza un viewport sobre un plano virtual infinito por tag:

- **`Monitor.canvas[tagidx]`** guarda `(cx, cy)`: el desplazamiento actual del
  viewport para cada tag.
- **`Client.saved_cx/cy/cw/ch` y `was_on_canvas`** guardan la posición en el
  canvas del cliente (usados al salir de tiled a flotante y al cambiar de tag).
- **`Client.is_pinned`** → ventanas ancladas que no se mueven con el canvas y
  quedan encima (`PINNED_WINDOWS_ALWAYS_ON_TOP`).

Operaciones:
- `movecanvas` (saltos discretos `MOVE_CANVAS_STEP`), `movecanvasmouse`
  (arrastre con multiplicador `.f` y acumulador de subpíxel), `homecanvas`
  (vuelve a 0,0), `centerwindow` (desliza el canvas hasta centrar el foco),
  `pinwindow` (anclar).
- Coordenadas visibles en la barra con `IT_SHOW_COORDINATES_IN_BAR`
  (`COORDINATES_STYLE`/`COORDINATES_DIVISOR`).
- Al cambiar entre flotante y tiled, `setlayout`/`view` guardan y restauran
  posiciones (`save_canvas_positions`/`restore_canvas_positions`).

**Nota de integración:** `applysizehints` con `INFINITE_TAGS` **desactiva** el
clampado interactivo a `sw/sh` (vxwm.c:406-415) porque las ventanas pueden
legítimamente vivir fuera del monitor. `moveresize` (variante infinite tags)
empuja el canvas cuando un cliente choca contra el borde del viewport
(vxwm.c:94-131) — comportamiento muy logrado pero con lógica duplicada respecto
a `movecanvasmouse`.

### 3.9 La barra de estado

- Ventana `override_redirect`, pixmap `ParentRelative`, clase `vxwm`.
- Dibujada con la librería **drw** (pixmap + GC + Xft, con fallback de fuentes
  por codepoint y caché de "no match").
- Contenido por zona (de izquierda a derecha): **tags** (ocupados resaltados
  con rectángulo, urgentes con `urg`), **símbolo de layout**, **título del
  cliente enfocado**, **status** (`stext` desde root name, solo en el monitor
  seleccionado), y opcionalmente **coordenadas del canvas**.
- Parámetros: `BAR_HEIGHT` (`user_bh`), `BAR_PADDING` (lateral `left_sidepad/
  right_sidepad` y vertical `top_vertpad/bottom_vertpad`), `BAR_ALWAYS_ON_TOP`,
  `ALT_CENTER_OF_BAR_COLOR`, `OCCUPIED_TAGS_DECORATION` (texto alternativo para
  tags ocupadas).
- En `config.h` activo hay padding lateral de **450px** a cada lado — diseño
  "barra centrada" intencional.

### 3.10 Temas y color (XRDB / pywal)

- `XRDB` permite recargar colores en caliente desde `~/.Xresources` con
  `Mod+F5`. El macro `XRDB_LOAD_COLOR` (xrdb.h) valida el formato hex `#rrggbb`
  antes de copiar. Para ello `config.h` marca los arrays de color con
  `MAYBE_CONST` (que se vuelve vacío con XRDB), y `drw.h` cambia la firma de
  `drw_scm_create` (const vs no-const).
- El pipeline de color del entorno es **pywal**: `rofi-wallpaper.sh` elige
  fondo → `wal -n` genera `~/.cache/wal/*` → `xrdb -merge colors.Xresources` →
  `xdotool key Super+F5` recarga vxwm → `wal-dunst.sh` regenera `dunstrc`.
- `wal/templates/dunst` es la plantilla de dunst de pywal; `wal/dunst` es la
  salida renderizada commiteada (colores estáticos "catppuccin-ish").

---

## 4. Configuraciones del entorno

| App | Archivo | Detalles |
|---|---|---|
| **kitty** | `kitty/kitty.conf` | Nerd Font JetBrains Mono, `font_size 10.5`, `background_opacity 0.65`, cursor trail, `include ~/.cache/wal/colors-kitty.conf` |
| **picom** | `picom/picom.conf` + `animations/` | backend egl, vsync, shadow 30px desplazada, corner-radius 6, blur dual_kawase strength 5; activa *overshootless* para open/close/geometry y *slidefade overshoot* para tag change; reglas para menús/fullscreen/dock sin radios/sombras |
| **rofi** | `config.rasi` (launcher drun limpio, importa `colors-rofi-dark.rasi`), `wall-changer.rasi` (grid 3×2 con previews), `wallpaper.rasi` (estilo alternativo hardcodeado, sin usar) | Temas centrados, radios, íconos Papirus |
| **fish** | `config.fish`, `fish_variables` | Mínimo: saluda desactivado, alias `fck=sudo` |
| **dunst** | `dunstrc` (raíz) + `wal/dunst` | Notificaciones top-right, stack-duplicates, marco de 3px; `wal-dunst.sh` lo sobrescribe con colores wal |
| **pywal** | `wal/` | Cache completa (Xresources, colors.sh, kitty, rofi, dwm, sway, vim, css, etc.) y `schemes/` JSON por fondo |

---

## 5. Scripts: instalador y auxiliares

### `install.sh` (Arch-only)
Flujo: banner → escalar sudo → `pacman -Sy` de 18 paquetes → instalar
`graphite-gtk-theme` desde AUR (`makepkg -si` como usuario) → `make clean
install` de `myvxwm` → crear directorios → escribir `.gtkrc-2.0`,
`~/.config/gtk-3.0/settings.ini` y `gtk-4.0/settings.ini` con Graphite-Dark →
copiar scripts a `~` con permisos `777` → copiar `.xinitrc` → copiar imágenes a
`~/Pictures/Wallpaper` → desplegar `fish/rofi/kitty/picom` a `~/.config` →
copiar `dunstrc` → copiar `wal/` a `~/.cache/wal`.

Problemas relevantes:
- **No idempotente**: `rm -rf ~/.config/<dir>` sin respaldo.
- `chmod 777` en scripts del HOME (inseguro e innecesario).
- Instala `pamixer` pero `volume.sh` usa `wpctl` (PipeWire).
- No instala `xdotool` (necesario por `rofi-wallpaper.sh`), ni `vcompmgr`
  (necesario por `ZOOM`), ni `xsetroot` para el status de la barra, ni
  `xwallpaper` está bien (sí está).
- Ruta `~/Pictures/Wallpaper` (singular) incoherente con `rofi-wallpaper.sh`.
- El instalador es Arch-only por diseño (documentado en README).
- No hay `uninstall.sh` en el proyecto original.

### Scripts auxiliares
- `brightness.sh` — `brightnessctl set $1` + `dunstify -r 9994` con barra.
- `volume.sh` — **wpctl** (PipeWire) con `dunstify -r 9993`. Contiene el
  bug 6.5 (dependencia no instalada).
- `ss.sh` — `scrot -s` a `~/Pictures/Screenshot/%d-%m-%Y_%H-%M-%S.png` +
  `notify-send`. Nota: en `config.h` el bind usa `~/ss-area.sh`
  (bug 6.1).
- `rofi-wallpaper.sh` — lista `~/Pictures/Wallpapers` (plural), grid rofi,
  xwallpaper stretch, `wal -n`, `xrdb -merge`, `xdotool key Super+F5`,
  `wal-dunst.sh`, `notify-send`. Problemas: ruta plural, dependencia xdotool,
  ignora webp.
- `wal-dunst.sh` — regenera `~/.config/dunst/dunstrc` desde
  `~/.cache/wal/colors.sh` y reinicia dunst. Duplica la plantilla de
  `wal/templates/dunst` (mejor usar la plantilla de pywal).
- `rvx` — reinicia vxwm (`killall vxwm; vxwm -ignoreautostart`, o `-d` con
  dbus-run-session). Sin argumentos imprime ayuda "fantasma"; `$arg` sin
  comillas; no lo instala el Makefile.
- `.xinitrc` — `dbus-launch`, xwallpaper (con typo), `wal`, `picom &`, `vxwm &`,
  `exec dbus-run-session sleep infinity`. Ver bug 6.3.

---

## 6. Bugs concretos detectados

Ordenados por severidad. Todos verificados leyendo el código.

1. **Tecla Imprimir rota — `config.h:190`.**
   `{ 0, XK_Print, spawn, SHCMD("~/ss-area.sh") }` apunta a un script que **no
   existe** en el repo (el real es `~/ss.sh`). Pulsar Imprimir falla en silencio.
   Además `ss.sh` es `scrot -s` (selección) — coherente con el nombre original,
   pero hay que unificar nombre.

2. **Selector de fondos sin wallpapers — `install.sh:121` vs `rofi-wallpaper.sh:4`.**
   El instalador crea/copia a `~/Pictures/Wallpaper` (singular); el script lista
   `~/Pictures/Wallpapers` (plural). El picker nunca muestra los fondos
   instalados. Además `wallpaper.rasi` (el tema bonito de 650px) no se usa:
   `rofi-wallpaper.sh` invoca `wall-changer.rasi`.

3. **`.xinitrc` roto/tosco.**
   - Línea 4: `xwallpaper --scretch` → typo (`--stretch`), falla silenciosamente.
   - Doble arranque de bus: `eval $(dbus-launch ...)` y luego
     `exec dbus-run-session sleep infinity` (dos sesiones D-Bus).
   - `vxwm &` corre **fuera** del `dbus-run-session` final; los programas
     lanzados desde vxwm tampoco heredan ese bus.
   - Fondo hardcodeado `wallhaven-6lk75x.jpg` (debe existir en
     `~/Pictures/Wallpaper`).
   - No hay bucle de reintento para picom/xwallpaper; si fallan, se pierden.

4. **`volume.sh` usa `wpctl` (PipeWire) pero el instalador instala `pamixer`
   (PulseAudio) — `install.sh:64`.**
   En un sistema sin PipeWire el volumen no funciona. O se cambia el script a
   `pamixer` o se añade `pipewire`/`wireplumber`/`pipewire-utils` a deps.

5. **Macro mal escrita en `modules/betterresize/betterresize.c:81`.**
   `#if !RESIZINIG_WINDOWS_IN_ALL_LAYOUTS_FLOATS_THEM` — la macro real es
   `RESIZING_WINDOWS_IN_ALL_LAYOUTS_FLOATS_THEM` (modules.h:33). Al estar
   indefinida, `!0 == 1`, así que la rama "solo tiled" se toma siempre, ignorando
   el valor configurado. Funcionalmente casi equivalente hoy, pero es un bug
   latente y una trampa de mantenimiento.

6. **ZOOM sin soporte real en la instalación — `modules.h:25`.**
   `ZOOM 1` requiere `vcompmgr` instalado y corriendo; no está en las
   dependencias del instalador. `zoom_value()` devuelve 1.0 si el sock no
   existe (seguro, pero el feature no funciona).

7. **`rvx` mal instalado y con shell frágil.**
   - `Makefile:install` nunca copia `rvx`, aunque `uninstall` lo borra
     (`config.mk` lo declara como "utility de restart" en el README).
   - `rvx` usa `$arg` sin comillas y sin `set -u`; con `./rvx` imprime
     "Run rvx -h for help" antes de reiniciar.

8. **`loadxrdb()` abre su propia conexión al display** (`xrdb.c:10`) mientras el
   resto usa la global `dpy`. No es un fallo, pero es una conexión extra
   innecesaria y un patrón a evitar.

9. **Artefactos binarios commiteados.** `myvxwm/vxwm`, `*.o` y los `.o` de
   `modules/` están dentro del repo git (propietario root). Deben ir a
   `.gitignore` (aetherwm ya lo hace).

10. **`vxwm.1` desactualizado** — documenta `st`/`dmenu` y no las binds reales
    (kitty/rofi, canvas infinito, etc.). El README del repo también es mínimo.

11. **`ss.sh` vs `install.sh`** — `install.sh` despliega `ss.sh`, pero `config.h`
    busca `ss-area.sh` (relacionado con el bug 1). Además `ss.sh` no comprueba
    `scrot` de forma robusta (menor).

12. **`config.h` guarda keybinds duplicados/en conflicto** — p. ej. `Mod+d` está
    en `incnmaster` (vxwm.c:148) y `Mod+Shift+d` en `centerwindow`; `Mod+q` en
    `quit` (vxwm.c:174) y `Mod+q` también en `enhancedtogglefloating`
    (config.h:199). El primero gana (orden en `keys[]`), así que `ETF` solo se
    puede usar con ratón/título. Es un conflicto silencioso.

13. **`moveresize` con `INFINITE_TAGS`** — en la rama de canvas (vxwm.c:94-131)
    reimplementa a mano lo que `movecanvas` ya hace; frágil ante cambios.

14. **Comentarios de scripts en indonesio** (`brightness.sh`, `volume.sh`,
    `wal-dunst.sh`) y algunas configs (rofi) — mezcla de idiomas que dificulta
    el mantenimiento.

---

## 7. Deuda técnica y problemas de arquitectura

**En el WM:**
- **Monolito con `#include`.** Todo vive en un TU; no hay fronteras reales de
  compilación, lo que impide testing unitario, análisis estático granular y
  comprensión incremental. Los módulos dependen de internals globales (`dpy`,
  `selmon`, macros `ISVISIBLE`, `TEXTW`, enums de eventos).
- **`vxwm.c` de 2820 líneas** con ~90 funciones estáticas. La sección de
  declaraciones y la de implementación están separadas pero el fichero es
  difícil de navegar.
- **Doble mantenimiento de plantillas** (`config.def.h`/`config.h`,
  `modules.def.h`/`modules.h`): fácil desincronización (ya pasó: versión 2.2 en
  header vs 2.3 en config.mk).
- **Código muerto / ramas `#if`** difíciles de limpiar: ej. `resizemouse` vanilla
  y `#if USE_RESIZECLIENT_FUNC`, `#if RESIZINIG…`.
- **Sin gestión de errores sistemática** en llamadas X; `xerror()` es el
  paraguas. No hay logs ni observabilidad.
- **Warnings con `-Wextra`** (~25): sign-compare (int vs unsigned), unused
  parameter en callbacks (`arg`), operador `?:` con signo. Con `-Wall` pasa
  limpio, pero con mejor hygiene se evita la clase de bug 6.5.
- **Nombres de macros inconsistentes** (`RESIZINIG…`, `Scretch` en script, uso
  mezclado de `_scretch`).
- **Sin contrato de módulos**: no hay documentación de qué globals/hooks usa
  cada módulo, ni checklist para añadir uno nuevo.

**En el entorno:**
- Configuraciones "personales" (paddings 450px, `topbar=0`, fuentes, temas)
  sin explicación.
- Caché pywal y fondos **commiteados** (bloat y datos de la máquina del autor:
  rutas `/home/nyxarch/...` en `wal/colors.sh`, `wal/wal`, y en los JSON de
  `schemes/`).
- `install.sh` monolítico, no idempotente, sin flags (no hay "install --wm-only",
  ni `uninstall`, ni dry-run).
- Duplicación de plantillas de dunst (`dunstrc`, `wal/dunst`, `wal/templates/dunst`,
  `wal-dunst.sh`).
- `wallpaper.rasi` muerto (tema alternativo no referenciado).

**Lo que SÍ está bien y debe conservarse:**
- Sistema de features compile-time (flexipatch) con dependencias forzadas.
- `drw` bien separada y reutilizable.
- `util.h` con macros `MAX/MIN/BETWEEN/LENGTH` y `ecalloc/die` con `errno`.
- Comentario de intención bueno en varias partes (p. ej. el subpíxel en
  `movecanvasmouse`, el "this comment got me giggling", ICCCM).
- Enfoque general ligero: sin daemons, sin config runtime pesada.
- Separación de roles por módulos (aunque mal implementada).

---

## 8. Propuesta de reescritura y mapeo a `aetherwm`

### 8.1 Estructura objetivo

```
aetherwm/
├── README.md                # Documento principal: instalación, teclas, arquitectura
├── install.sh               # Idempotente, con checks, respaldo y uninstall
├── uninstall.sh
├── .gitignore               # *.o, vxwm, cache, wallpapers generados
├── config/
│   ├── dunst/dunstrc        # desde wal/templates
│   ├── kitty/kitty.conf
│   ├── picom/picom.conf + animations/
│   ├── rofi/{config,wall-changer,wallpaper}.rasi
│   └── wal/templates/dunst  # única fuente de verdad para dunst
├── scripts/
│   ├── volume.sh  brightness.sh  screenshot.sh  wallpaper.sh  wal-dunst.sh
├── vxwm/                    # WM reescrito (ver 8.2)
└── wallpapers/              # Fondos (con README de licencias/atribución)
```

### 8.2 Mejoras al WM (`aetherwm/vxwm/`)

**Objetivo:** mantener features y filosofía, ganar claridad y robustez.

1. **Separar núcleo y módulos con archivos compilados individualmente** (no
   `#include`): un módulo por `(modulo.h, modulo.c)` que declare sus
   dependencias. Si se quiere conservar el "all-in-one" se puede generar con un
   script de amalgama, pero el código fuente debe estar separado.
2. **Introducir un "registro de módulos"**: cada módulo exporta `nombre.h` con
   sus funciones y declara explícitamente:
   - campos que añade a `Client`/`Monitor`,
   - hooks que usa (p. ej. `on_map`, `on_arrange`, `on_view_change`),
   - prerequisitos (macros que exige).
3. **Renombrar/agrupar macros** con prefijos de módulo y un único `modules.h`
   autogenerado desde `modules/*/mod.h` (evitar `config.def.h` + `config.h`
   duplicados: usar `#include "config.h"` con `config.def.h` como template y
   `cp` en el Makefile, que ya es lo que hay — pero documentar el flujo).
4. **Arreglar los bugs de la sección 6** (mejorresize typo, binds duplicados,
   rutas de scripts, `xinitrc`).
5. **Hygiene C**: añadir `-Werror -Wall -Wextra` (o `-Wsign-conversion`),
   tipar correctamente (`size_t`, `unsigned`), marcar callbacks `(void)`/usar
   `UNUSED`, `static` consistente en definiciones de módulos.
6. **Documentar API**: comentario de cabecera por función con propósito,
   params y "caller". README por módulo si es complejo (infinitetags merece
   uno).
7. **`config.mk`**: fijar versión en un solo sitio; generar `vxwm.1` desde
   fuente única (p. ej. `vxwm.1.md`).
8. **Instalar `rvx`** en el Makefile y pulir su shell (`set -eu`, `""`, case).
9. **Tests/chequeo**: mínimo un `make check` con `-Werror` y un
   `--dry-run`/valgrind smoke; idealmente tests unitarios de las funciones
   puras (scoring de focusdir/movedir, parseo de hints, clamps de
   applysizehints).
10. **Añadir observabilidad mínima**: `fprintf(stderr, ...)` controlado por
    `-DDEBUG` para eventos clave (manage/unmanage/error).

### 8.3 Mejoras a los scripts e instalador

- `install.sh`:
  - Flags: `--wm`, `--configs`, `--all`, `--dry-run`, `--uninstall`.
  - `set -Eeuo pipefail`, función `info/warn/err`, respaldo (`~/.config/.bak`).
  - Instalar las dependencias **reales**: alinear `pamixer`/`wpctl`, añadir
    `xdotool` (o eliminar su uso), documentar `vcompmgr` como opcional de ZOOM.
  - Unificar rutas `Wallpaper` (singular) en instalador y scripts.
  - Deploy con permisos `755` (no `777`).
  - Generar `dunstrc` desde la plantilla pywal (única fuente).
- `scripts/`:
  - `screenshot.sh` con `flameshot`/`scrot -s` + notificación y bind corregido
    en config.h.
  - `wallpaper.sh`: usar `xdg`/`setsid`, evitar `xdotool` (o detectarlo),
    soportar webp, y recargar vxwm vía `Mod+F5` simulado solo si existe.
  - Comentarios en inglés o español consistentes.
- `.xinitrc`: una sola sesión D-Bus, `xwallpaper --stretch`, arranque
  tolerante a fallos, y `vxwm` dentro de la sesión (o usar `startx` + script).

### 8.4 Documentación a incluir en aetherwm

- `README.md`: qué es, dependencias, instalación (Arch + otros), capturas,
  keybinds completos, cómo extender.
- `docs/ARQUITECTURA.md`: diagramas de flujo (startup, ciclo de evento,
  manage), estructuras de datos, contratos de módulos, "cómo agregar un módulo".
- `docs/ANALISIS.md`: este documento (decisión de mantenerlo como referencia
  del estado original).
- `vxwm/README.md` + `vxwm.1` regenerados.
- Licencias: conservar `LICENSE` (MIT-style) y `LICENSE.dwm`.

---

## 9. Hoja de ruta sugerida

| Fase | Entregable | Criterio de aceptación |
|---|---|---|
| **0. Línea base** | Completar `docs/ANALISIS.md` (este) | Consenso sobre bugs y objetivos |
| **1. Esqueleto** | Recrear estructura de `aetherwm/vxwm/` con núcleo limpio compilando con `-Werror` | `make && make check` sin warnings |
| **2. Módulos núcleo** | Portar en orden: windowmap, gaps, fullscreen, xrdb, etf, ewmh_tags, externalbars | Mismas features; tests unitarios de funciones puras |
| **3. Infinite tags** | Portar infinitetags con su propia doc (módulo más complejo) | Comportamiento idéntico + README de módulo |
| **4. Interacción** | moveresizekbd, directionalfocus/directionalmove, betterresize, zoom, warp, autostart | Keybinds verificados uno a uno |
| **5. Config/scripts** | Portar configs a `config/`, scripts a `scripts/`, corregir bugs 6.1–6.4 | Scripts funcionando en Arch limpio |
| **6. Instalador** | `install.sh`/`uninstall.sh` idempotentes con flags | Instalar→desinstalar→reinstalar en máquina limpia |
| **7. Documentación** | README + ARQUITECTURA + vxwm.1 regenerados | Cualquier persona puede añadir un módulo sin leer el código |
| **8. Verificación final** | Checklist de features vs original | Cada feature de `modules.h` verificada |

---

## Apéndice A — Glosario

- **TU (translation unit)** — unidad de compilación; aquí, un `.c` tras
  preprocesado.
- **Atom** — identificador internado en X server para propiedades/mensajes
  (`_NET_WM_*`, `WM_*`).
- **EWMH** — Extended Window Manager Hints, protocolo para interoperar con
  paneles/desktops.
- **ICCCM** — Inter-Client Communication Conventions Manual (títulos, hints de
  tamaño, focus, estados).
- **Override-redirect** — ventana que el WM no gestiona (barras, menús).
- **Strut** — área reservada en un borde de pantalla por una barra externa.
- **Viewport / canvas** — el plano infinito de los tags infinitos.
- **Drw** — librería de dibujo de suckless (pixmap + Xft) reutilizada aquí.

## Apéndice B — Inventario de macros en `modules.h`

```
INFINITE_TAGS, IT_SHOW_COORDINATES_IN_BAR, ZOOM,
BETTER_RESIZE, BR_CHANGE_CURSOR, LOCK_MOVE_RESIZE_REFRESH_RATE,
USE_RESIZECLIENT_FUNC, RESIZING_WINDOWS_IN_ALL_LAYOUTS_FLOATS_THEM,
MOVE_RESIZE_WITH_KEYBOARD,
GAPS, XRDB,
TAG_TO_TAG, SLOWER_TAGS_ANIMATION, WINDOWMAP, PDWM_LIKE_TAGS_ANIMATION,
ALT_CENTER_OF_BAR_COLOR, BAR_HEIGHT, BAR_PADDING, OCCUPIED_TAGS_DECORATION,
EXTERNAL_BARS, EWMH_TAGS,
WARP_TO_CLIENT + 6 × WARP_TO_*,
AUTOSTART, FULLSCREEN, MOVE_IN_TILED, DIRECTIONAL_FOCUS, DIRECTIONAL_MOVE,
FLOATING_LAYOUT_FLOATS_WINDOWS, ENHANCED_TOGGLE_FLOATING,
RESTORE_SIZE_AND_POS_ETF
```

Macros de configuración (config.h): `COORDINATES_STYLE, COORDINATES_DIVISOR,
MOVE_CANVAS_STEP, MOVE_WITH_KEYBOARD_STEP, RESIZE_WITH_KEYBOARD_STEP,
CENTER_NEW_FLOATING_WINDOWS, NEW_FLOATING_WINDOWS_APPEAR_UNDER_CURSOR,
BAR_ALWAYS_ON_TOP, EXTERNAL_BARS_ALWAYS_ON_TOP, PINNED_WINDOWS_ALWAYS_ON_TOP,
BAR_PADDING, LOCK_MOVE_RESIZE_REFRESH_RATE`.

---

*Fin del análisis. Este documento es la base del trabajo de reescritura en
`aetherwm` y debe mantenerse actualizado conforme se migre cada módulo.*
