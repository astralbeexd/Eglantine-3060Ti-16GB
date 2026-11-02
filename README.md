# Eglantine-3060Ti-16GB

Compilación comunitaria experimental para Windows, probada en Ryzen 7 5800X,
RTX 3060 Ti de 8 GB y 16 GB de RAM. No es una versión oficial ni garantiza 60 FPS.

## Estado de las descargas

Este repositorio contiene los parches, los veinte archivos de código modificados,
el perfil portátil y las instrucciones para reconstruir el programa desde una base exacta.
**Todavía no hay un binario público verificado en Releases.** No descargues supuestas
versiones de este proyecto desde enlaces ajenos. El paquete local está preparado como
candidato; falta completar la entrega de fuentes de sus dependencias y adjuntarlo a una release.

## Cambios principales

- Reutilización del código nativo transformado de shaders, tomada de Wael3rd.
- Correcciones de liberación, inicialización parcial y movimientos de memoria en Windows.
- Reintentos acotados cuando Windows no puede satisfacer temporalmente la reserva fastmem.
- Aritmética protegida en contabilidad Vulkan y antigüedad de texturas.
- Reducción de mensajes repetidos de auto-stubs NX2, conservando sus respuestas.
- Compilación Release para Zen 3 y ThinLTO selectivo del renderizador; PGO desactivado.
- Instrumentación opcional de tiempos y memoria, desactivada para uso normal.

El límite experimental del descuento de memoria libre VMA queda desactivado por defecto.
No se repararon todos los servicios desconocidos ni se eliminó toda la compilación de shaders.

## Perfil y recomendaciones

Salida 1920×1080 en modo portátil y escala 1x. El juego conserva su resolución dinámica:
esto no equivale a demostrar que cada pase 3D sea nativo a 1080p.
CPU Normal; límites normal/turbo 100%; sin ticks personalizados ni generación de cuadros.
GPU Boost altera consultas de tiempo de GPU, sin multiplicar el reloj de CPU.
Shaders y presentación asíncronos activos; reactive flushing desactivado.

La build local usa `-march=znver3`: se dirige a Zen 3 y no se ha validado en otros procesadores.
No se anuncia como compatible con cualquier CPU que tenga una RTX 3060 Ti.
Para otro hardware, recompila con las opciones apropiadas. Necesitas un controlador Vulkan compatible.
Deja la paginación de Windows disponible; no limites artificialmente el proceso a 4 GB.
No ejecutes varias copias del juego a la vez y permite que se caliente la caché de shaders.
No compartas cachés entre drivers o builds sin comprobar compatibilidad.

En el tramo automático comparado se midieron 56,9 FPS; todavía hubo tirones.
Dos pruebas llegaron a 18.000 presentaciones. El kart acelera sin dirección y puede
chocar contra una barrera: estas cifras no equivalen a una carrera completa ni a todas las pistas.
No hay demostración de funcionamiento con sólo 4 GB de RAM total: memoria privada,
working set compartido y memoria comprometida son métricas distintas.

## Uso del futuro paquete portátil

Extrae el ZIP completo en una carpeta con permisos de escritura; no ejecutes desde el ZIP.
Abre `Iniciar.cmd`; las DLL y el perfil vendrán incluidos y no necesitarás MSYS2 para jugar.
`eglantine.exe` también abre directamente el perfil portátil incluido.
Configura tu mando y aporta tu juego y archivos de consola necesarios mediante la interfaz.
No se incluyen juegos, claves, firmware, NAND, partidas, perfiles personales ni cachés de shaders.

## Código y compilación

La base es [Djihads80/Eglantine](https://github.com/Djihads80/Eglantine),
commit `6bde8c20d1f7c020a920e59a989c03822a08445d`.
Este repositorio es una distribución de cambios sobre esa base, no un historial completo del fork.
Los dos parches reproducen los veinte archivos de `modified/`.

En MSYS2 UCRT64, desde la raíz del repositorio:

```bash
bash build-ucrt64.sh
```

El script instala dependencias, descarga la base fijada, aplica los parches,
ejecuta pruebas y compila con dos procesos. Puede ser necesario reiniciar UCRT64 al
actualizar MSYS2. La compilación local se verificó; no se ha validado una instalación
nueva de MSYS2 de principio a fin. Los paquetes del script no están fijados a versiones
históricas: consulta `BUILD-INFO.json` para las versiones usadas localmente.

## Licencia, créditos y transparencia

GPL-3.0-or-later para el programa, respetando los avisos y licencias por archivo y dependencia.
Consulta [CREDITS.md](CREDITS.md), [CHANGES.md](CHANGES.md) y [DISTRIBUTION.md](DISTRIBUTION.md).
Cambios y documentación preparados con asistencia de IA. No se afirma una revisión humana
completa del código ni certificación de seguridad o estabilidad. No se ha enviado un PR al proyecto original.
