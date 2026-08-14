# Plan de mejoras y funcionalidades para vxwm

## Objetivo general

Mejorar vxwm manteniendo su filosofía principal de **canvas infinito**, pero haciendo que el sistema sea más estable, intuitivo, modular, eficiente y cómodo de utilizar tanto con teclado como con mouse y touchpad.

Las modificaciones deben procurar mantener la arquitectura modular existente. Evitar implementar soluciones rígidas que dificulten agregar nuevos modos, funcionalidades o configuraciones posteriormente.

---

# 1. Barra del sistema

## Objetivo

Mejorar completamente la barra para que sea más estética, informativa y coherente con el resto del sistema.

### Funcionalidades necesarias

La barra debe mostrar, como mínimo:

- Hora.
    
- Estado y porcentaje de batería.
    
- Conexión a Internet.
    
- Estado de Bluetooth.
    
- Indicadores de aplicaciones ejecutándose en segundo plano.
    
    - Ejemplo: Steam.
        
    - Debe ser posible ampliar posteriormente esta lista.
        
- Workspace actual.
    
- Modo actual de vxwm.
    

### Integración visual

La barra debe utilizar el mismo sistema de colores dinámicos que ya utiliza vxwm para adaptarse al fondo de pantalla.

El objetivo es que:

- Los colores de la barra se adapten automáticamente al wallpaper.
    
- Los diferentes elementos mantengan una apariencia coherente.
    
- No existan colores definidos de forma rígida cuando puedan obtenerse del sistema de temas existente.
    

### Workspaces

Limitar el sistema a **4 workspaces**.

Los workspaces deben estar claramente identificados en la barra y su estado actual debe ser fácil de reconocer.

### Indicador de modo

Actualmente puede ser difícil saber en qué modo de pantalla se encuentra vxwm.

La barra debe proporcionar una indicación clara del modo actual, por ejemplo:

- Libre.
    
- Mosaico.
    
- Presentación.
    
- Canvas.
    
- Cualquier otro modo existente o que se agregue posteriormente.
    

Este sistema debe ser **modular**, de manera que agregar un nuevo modo no requiera rehacer toda la barra.

También debe revisarse el manual para explicar claramente qué hace cada modo.

### Prioridad de la barra

Actualmente la barra tiene una prioridad demasiado baja y una ventana puede quedar por encima de ella.

La barra debe tener una prioridad suficientemente alta para permanecer visible por encima de las ventanas cuando corresponda.

### Comportamiento especial en Canvas

El canvas infinito es una de las características principales de vxwm.

Cuando el usuario está en modo Canvas y se desplaza/navega por el escritorio, la barra debe poder ocultarse inteligentemente cuando interfiera con la navegación.

El comportamiento debe ser dinámico:

- La barra aparece cuando sea necesaria.
    
- Puede ocultarse durante la navegación por el canvas.
    
- No debe interferir con las ventanas ni con el movimiento por el espacio infinito.
    
- Debe existir una forma intuitiva de volver a mostrarla.
    

---

# 2. Movimiento de ventanas — Super + F

## Objetivo

Revisar y corregir el comportamiento de `Super + F`.

La intención es que se comporte de manera similar a cómo funciona el movimiento de ventanas en Hyprland: mover una ventana dentro del sistema de mosaico **sin convertirla innecesariamente en una ventana flotante** ni romper la estructura actual.

### Problema actual

Cuando existen dos ventanas divididas y se abre una tercera, vxwm puede terminar colocando la nueva ventana en un lado completo de la pantalla y desplazando las anteriores hacia el otro lado.

Ejemplo:

```text
┌───────────┬───────────┐
│           │ Ventana 1 │
│ Ventana 3 │───────────│
│           │ Ventana 2 │
└───────────┴───────────┘
```

Si se continúan abriendo ventanas, el problema se vuelve progresivo:

```text
┌───────────┬───────────┐
│           │ Ventana 1 │
│           ├───────────┤
│           │ Ventana 2 │
│ Ventana 10├───────────┤
│           │ Ventana 3 │
│           ├───────────┤
│           │ ...       │
└───────────┴───────────┘
```

Esto termina provocando un colapso de la distribución.

## Comportamiento deseado

La distribución debe considerar principalmente la posición actual del mouse.

Si existen dos ventanas:

```text
┌───────────┬───────────┐
│ Ventana 1 │ Ventana 2 │
│           │     🖱️    │
└───────────┴───────────┘
```

Si el mouse está sobre `Ventana 2` y se abre una nueva terminal, la nueva ventana debe repartirse con esa ventana:

```text
┌───────────┬───────────┐
│           │ Ventana 2 │
│ Ventana 1 ├───────────┤
│           │ Terminal  │
└───────────┴───────────┘
```

El usuario no debería tener que hacer click previamente. **La posición del mouse debe ser suficiente para determinar dónde debe incorporarse una nueva ventana.**

### División en cuatro

También debe ser posible distribuir ventanas en cuatro segmentos:

```text
┌───────────┬───────────┐
│ Ventana 1 │ Ventana 2 │
├───────────┼───────────┤
│ Ventana 3 │ Ventana 4 │
└───────────┴───────────┘
```

La lógica de mosaico debe permitir subdivisiones progresivas sin generar una estructura desequilibrada.

### Requisitos

- No convertir automáticamente ventanas en floating al moverlas.
    
- Mantener la estructura de mosaico.
    
- Utilizar la posición del mouse como criterio importante.
    
- Permitir dividir horizontal y verticalmente.
    
- Permitir llegar a cuatro cuadrantes.
    
- Evitar que las ventanas nuevas se acumulen sistemáticamente en un único lado.
    
- Mantener la lógica modular para poder agregar otros algoritmos de layout posteriormente.
    

---

# 3. Touchpad / MousePad

## Objetivo

Mejorar completamente la compatibilidad con el touchpad de portátiles.

Actualmente el touchpad permite principalmente mover el cursor, pero no proporciona una experiencia adecuada para hacer click izquierdo y derecho.

### Funcionalidades necesarias

Debe ser posible:

- Click izquierdo.
    
- Click derecho.
    
- Movimiento normal del cursor.
    
- Scroll.
    
- Gestos multitáctiles cuando sea posible.
    

### Integración con Canvas

El gesto más importante es la navegación por el canvas infinito.

Actualmente el comportamiento deseado mediante mouse es:

1. Colocar el cursor sobre el fondo.
    
2. Mantener presionado el click izquierdo.
    
3. Mover el mouse.
    
4. Navegar por el canvas.
    

Debe implementarse un comportamiento equivalente mediante touchpad.

Por ejemplo:

**Dos dedos + desplazamiento sobre el fondo → navegación por el canvas infinito.**

El gesto debe integrarse correctamente con el resto de los gestos del touchpad y no interferir con el uso normal de las ventanas.

---

# 4. Rendimiento y perfiles de energía

## Objetivo

Mejorar el rendimiento general de vxwm y, especialmente, reducir el impacto del entorno cuando se ejecutan aplicaciones exigentes como videojuegos.

Se deben crear perfiles de funcionamiento.

### Perfil ahorro de batería

Debe priorizar:

- Menor consumo energético.
    
- Menor cantidad de animaciones.
    
- Menor uso de efectos.
    
- Menor uso de blur.
    
- Menor movimiento innecesario.
    
- Reducción de procesos o actualizaciones no esenciales.
    

### Perfil rendimiento

Debe priorizar:

- FPS.
    
- Latencia.
    
- Fluidez.
    
- Rendimiento de aplicaciones.
    
- Juegos.
    
- Programas exigentes.
    

Debe reducir o desactivar efectos que puedan afectar innecesariamente el rendimiento.

### Selector de perfil

La barra debe disponer de una ventana emergente para cambiar entre perfiles.

La ventana debe:

- Tener una animación agradable.
    
- Utilizar el sistema visual actual.
    
- Adaptarse a los colores dinámicos.
    
- Ser sencilla de utilizar.
    
- Permitir agregar nuevos perfiles posteriormente.
    

Ejemplo:

```text
┌─────────────────────────────┐
│      Perfil del sistema     │
│                             │
│  ⚡ Rendimiento              │
│  🔋 Ahorro de batería        │
│  ⚖ Equilibrado              │
└─────────────────────────────┘
```

La implementación debe analizar primero qué mecanismos disponibles en Linux/Arch pueden utilizarse para realizar estos cambios correctamente, en lugar de simular únicamente un cambio visual.

---

# 5. Shell

## Objetivo

Evaluar si realmente es necesario desarrollar un shell propio.

La idea está inspirada parcialmente en Caelestia.

No debe considerarse una prioridad inicial.

### Decisión requerida

Primero analizar:

- Qué funcionalidades ya ofrece vxwm.
    
- Qué puede realizarse mediante herramientas externas.
    
- Qué funcionalidades justificarían realmente desarrollar un shell.
    
- Qué costo tendría mantenerlo.
    

Si no aporta un beneficio significativo en esta etapa, dejarlo como una funcionalidad futura.

---

# 6. Atajos de teclado

## Objetivo

Rediseñar y organizar los atajos de teclado para que sean intuitivos y tengan relación lógica con la acción que realizan.

Los atajos deben:

- Ser fáciles de memorizar.
    
- Mantener una lógica consistente.
    
- Evitar combinaciones arbitrarias.
    
- Estar documentados claramente.
    
- Permitir ampliar posteriormente el sistema.
    

### Atajos que NO deben modificarse

Estos atajos son globales para el usuario y deben mantenerse exactamente como están:

- `Super + T`
    
- `Super + W`
    
- `Super + Q`
    

No reutilizar estas combinaciones para otras funcionalidades.

---

# 7. Clipboard para capturas de pantalla

## Objetivo

Modificar el sistema de capturas para que una captura pueda utilizarse inmediatamente después de realizarla.

### Problema actual

Actualmente:

1. Se realiza una captura.
    
2. La imagen se guarda en un directorio.
    
3. El usuario debe abrir el gestor de archivos.
    
4. Buscar la imagen.
    
5. Copiarla.
    
6. Finalmente pegarla donde la necesite.
    

Este flujo es innecesariamente largo.

### Comportamiento deseado

Al realizar una captura:

1. Guardar la imagen en el directorio correspondiente.
    
2. Copiar automáticamente la imagen al clipboard.
    
3. Permitir pegarla inmediatamente con `Ctrl + V`.
    

Debe funcionar en aplicaciones como:

- Navegadores.
    
- Editores.
    
- Chats.
    
- Gestores de archivos.
    
- Cualquier aplicación compatible con imágenes en el clipboard.
    

La imagen debe permanecer disponible para pegar aunque el archivo original esté almacenado en otro directorio.

---

# 8. Sistema de notificaciones

## Objetivo

Crear un sistema de notificaciones sencillo, modular y personalizable.

Actualmente existe principalmente la notificación asociada a las capturas de pantalla.

Debe ampliarse para soportar eventos como:

- Batería baja.
    
- Batería completamente cargada.
    
- Cambio de perfil de rendimiento.
    
- Captura de pantalla.
    
- Bluetooth conectado/desconectado.
    
- Cambios importantes del sistema.
    
- Notificaciones de aplicaciones.
    
- Notificaciones del navegador, cuando el usuario las permita.
    

### Diseño

Las notificaciones deben:

- Integrarse visualmente con vxwm.
    
- Utilizar los colores dinámicos.
    
- Tener animaciones suaves.
    
- Ser configurables.
    
- Poder habilitarse/deshabilitarse individualmente.
    
- Mantener una arquitectura modular para agregar nuevos tipos de notificaciones.
    

---

# 9. Revisión de los modos

Antes de implementar nuevas funcionalidades, realizar una revisión general de todos los modos existentes.

Para cada modo comprobar:

- Que pueda activarse correctamente.
    
- Que pueda desactivarse correctamente.
    
- Que no interfiera con otros modos.
    
- Que las ventanas mantengan su estado esperado.
    
- Que los atajos funcionen.
    
- Que el mouse funcione correctamente.
    
- Que el comportamiento sea consistente.
    
- Que no existan errores al cambiar rápidamente entre modos.
    

Especial atención a las interacciones entre:

- Canvas.
    
- Floating.
    
- Mosaico.
    
- Presentación.
    
- Workspaces.
    
- Movimiento de ventanas.
    

No asumir que el comportamiento actual es correcto: probar cada modo y documentar los problemas encontrados.

---

# 10. Workspaces

## Objetivo

Corregir el comportamiento incorrecto al volver a seleccionar el workspace actual.

### Problema

Actualmente puede ocurrir lo siguiente:

1. `Super + 2`
    
2. Abrir una ventana.
    
3. `Super + 1`
    
4. Abrir otra ventana.
    
5. `Super + 1` nuevamente.
    

En lugar de permanecer en el workspace 1, vxwm puede enviar al usuario a un workspace aleatorio, frecuentemente el workspace 1 pero generando un estado inconsistente.

### Comportamiento deseado

Si el usuario ya está en el workspace solicitado:

```text
Super + 1
```

no debe ocurrir ninguna acción.

Debe simplemente permanecer en el workspace actual.

Esto debe evitar:

- Cambios inesperados.
    
- Selección aleatoria.
    
- Problemas de gestión de ventanas.
    
- Estados inconsistentes.
    

Además, dado que el sistema debe limitarse a **4 workspaces**, todos los atajos relacionados deben respetar ese límite.

---

# 11. Manual de usuario

## Objetivo

Crear un manual sencillo de entender incluso para alguien que acaba de instalar vxwm.

El manual debe explicar:

### Conceptos básicos

- Qué es vxwm.
    
- Qué significa el concepto de canvas infinito.
    
- Cómo funcionan los workspaces.
    
- Cómo funcionan los modos.
    

### Modos

Explicar cada modo con:

- Nombre.
    
- Propósito.
    
- Cómo activarlo.
    
- Cómo salir de él.
    
- Cómo interactúa con las ventanas.
    
- Cuándo conviene utilizarlo.
    

### Atajos

Crear una tabla organizada:

|Atajo|Acción|Contexto|
|---|---|---|
|`Super + T`|Abrir terminal|Global|
|`Super + W`|...|Global|
|`Super + Q`|...|Global|

Los atajos deben estar agrupados por categoría:

- Ventanas.
    
- Workspaces.
    
- Modos.
    
- Canvas.
    
- Aplicaciones.
    
- Sistema.
    
- Mouse/touchpad.
    

### Mouse

Explicar:

- Click izquierdo.
    
- Click derecho.
    
- Movimiento.
    
- Mover ventanas.
    
- Interacción con mosaicos.
    
- Navegación por Canvas.
    

### Touchpad

Explicar los gestos disponibles y su función.

### Barra

Explicar todos sus indicadores y qué significa cada uno.

### Configuración

Explicar dónde modificar:

- Atajos.
    
- Colores.
    
- Barra.
    
- Modos.
    
- Workspaces.
    
- Notificaciones.
    
- Perfiles de rendimiento.
    

El manual debe evitar explicaciones excesivamente técnicas cuando no sean necesarias.

---

# Prioridad de implementación

Para evitar intentar modificar todo simultáneamente, trabajar en el siguiente orden:

## Prioridad 1 — Estabilidad y comportamiento fundamental

1. Revisar los modos existentes.
    
2. Corregir workspaces.
    
3. Corregir `Super + F`.
    
4. Mejorar el algoritmo de mosaico.
    
5. Permitir división en cuatro cuadrantes.
    

## Prioridad 2 — Interacción

6. Mejorar mouse/touchpad.
    
7. Implementar navegación por Canvas mediante touchpad.
    
8. Revisar y reorganizar atajos.
    

## Prioridad 3 — Barra y experiencia visual

9. Rediseñar la barra.
    
10. Indicador modular de modos.
    
11. Workspaces en la barra.
    
12. Batería, red y Bluetooth.
    
13. Indicadores de aplicaciones.
    
14. Prioridad/overlay de la barra.
    
15. Ocultamiento inteligente en Canvas.
    

## Prioridad 4 — Sistema

16. Clipboard automático para capturas.
    
17. Sistema de notificaciones.
    
18. Perfiles de energía/rendimiento.
    

## Prioridad 5 — Funcionalidades futuras

19. Evaluar desarrollo de un shell propio.
    
20. Mejoras visuales adicionales.
    
21. Nuevos modos de layout.
    
22. Nuevos gestos y automatizaciones.
    

---

# Reglas para la implementación

Antes de modificar código:

1. Analizar la arquitectura actual de vxwm.
    
2. Identificar qué archivos y módulos controlan cada funcionalidad.
    
3. No modificar código sin entender primero su relación con los módulos existentes.
    
4. Mantener la modularidad.
    
5. Evitar duplicación de lógica.
    
6. Evitar soluciones específicas para un único caso cuando pueda diseñarse una solución general.
    
7. Mantener compatibilidad con las funcionalidades actuales siempre que sea posible.
    
8. Probar cada modificación antes de pasar a la siguiente.
    
9. Si una funcionalidad requiere cambiar la arquitectura, explicar primero qué parte debe cambiar y por qué.
    
10. No asumir que una solución de otro window manager puede copiarse directamente a vxwm: adaptarla a la arquitectura y filosofía de canvas infinito.
    

# Forma de trabajo recomendada

Para cada tarea:

1. Analizar el código relacionado.
    
2. Explicar brevemente cómo funciona actualmente.
    
3. Identificar el problema.
    
4. Proponer la solución.
    
5. Identificar los archivos que serán modificados.
    
6. Implementar el cambio.
    
7. Compilar.
    
8. Ejecutar pruebas.
    
9. Revisar posibles regresiones.
    
10. Documentar el resultado.
    

**Importante:** no intentar implementar todas las funcionalidades de una sola vez. Trabajar por módulos y mantener vxwm funcional después de cada cambio importante.