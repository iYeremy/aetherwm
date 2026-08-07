# Contrato de módulos

Este documento define cómo se añadirán módulos a `vxwm` en las fases 2–4 de la
hoja de ruta. **Estado actual: Fase 1 (esqueleto). Aún no hay módulos; este
archivo fija las reglas para no improvisar.**

## Principio

A diferencia del `vxwm` original (que inyectaba los módulos con `#include` en un
único translation unit), aquí cada módulo es un **`.c`/`.h` compilado por
separado** y enlazado con el núcleo. El núcleo (`vxwm.c`) expone una API mínima
y los módulos se registran contra ella.

## Estructura de un módulo

```
modules/<nombre>/
├── <nombre>.c      # implementación (TU separado, sin #include del núcleo)
├── <nombre>.h      # API pública del módulo + documentación
└── README.md       # opcional: comportamiento, keybinds, dependencias
```

## Contrato que debe cumplir cada módulo

1. **No `#include` del núcleo.** Declara en su `.h` las funciones y variables
   externas que necesita (por ejemplo `dpy`, `selmon`, `Client`), y el núcleo
   las publica de forma explícita (prototipos no-`static` + `extern`).
2. **Declara sus dependencias.** Al inicio del `.h`:
   ```c
   /* Depends on: MONITOR_GEOMETRY, XRDB
    * Adds to Client: int extra_field;
    * Hooks used: on_arrange(Monitor *m) */
   ```
3. **Macro de activación.** Cada módulo define su propia macro `0/1` en
   `modules/modules.h` (generado por el Makefile a partir de las macros de los
   módulos presentes; no se edita a mano).
4. **Función de registro.** Si el módulo necesita hooks, expone
   `void <nombre>_init(void)` que el núcleo llama desde `setup()`.
5. **Sin lógica en el núcleo.** Todo el comportamiento vive en el módulo; el
   núcleo solo llama a hooks opcionales declarados en `vxwm.h`.

## Checklist para añadir un módulo nuevo

1. Crear `modules/<nombre>/<nombre>.{c,h}` cumpliendo el contrato.
2. Añadir la macro de activación a `modules/modules.h` (o a la fuente única
   de la que se genera).
3. Añadir `<nombre>.c` al `SRC` del Makefile (o a un `modules.mk`).
4. Declarar en `vxwm.h` los hooks que el módulo consume y llamarlos desde el
   núcleo.
5. Documentar keybinds/reglas en `config.h` si aplica.
6. Ejecutar `make && make check` (obligatorio, sin warnings).

## Hooks núcleo previstos (a implementar en Fase 2)

| Hook | Momento | Firma |
|---|---|---|
| `on_map` | tras adoptar un cliente | `void (*)(Client *)` |
| `on_unmap` | al des-adoptar | `void (*)(Client *)` |
| `on_arrange` | antes de reordenar un monitor | `void (*)(Monitor *)` |
| `on_view_change` | al cambiar de tag/vista | `void (*)(Monitor *)` |
| `on_keypress` | tras despachar una tecla | `void (*)(XKeyEvent *)` |

La lista exacta se fijará en Fase 2 al portar el primer módulo, no antes.

## Fases de migración (desde docs/ANALISIS.md)

- **Fase 2** — núcleo: windowmap, gaps, fullscreen, xrdb, etf, ewmh_tags,
  externalbars.
- **Fase 3** — infinitetags (el más complejo; README de módulo propio).
- **Fase 4** — interacción: moveresizekbd, directionalfocus, directionalmove,
  betterresize, zoom, warp, autostart.
