# Compatibilidad: objetivos y pruebas

## Ediciones publicadas

| Edición | CPU | Descarga |
| --- | --- | --- |
| Zen 3 | AMD Zen 3; referencia probada: Ryzen 7 5800X | [Release Zen 3](https://github.com/astralbeexd/Eglantine-3060Ti-16GB/releases/tag/v1.0.0-3060ti16gb-romfs1) |
| Compatible x86-64-v2 r2 | Intel y AMD x64 que cumplan x86-64-v2; arranque probado en el 5800X | [Release compatible](https://github.com/astralbeexd/Eglantine-3060Ti-16GB/releases/tag/v1.0.0-compatible-x64-r2) |

Elige según el procesador; una RTX 3060 Ti o 16 GB de RAM no garantizan compatibilidad del CPU. El Xeon E5-2697 v3 del reporte sigue pendiente de una prueba real.

## Comprobaciones de la compatible

- Compilación separada con `-march=x86-64-v2 -mtune=generic`; detección de instrucciones de Dynarmic en ejecución.
- Lanzador compilado para x86-64 básico: comprueba CPU y Windows, limpia el entorno, conserva argumentos Unicode y comunica cierres con error.
- Arranque limpio de 20 segundos; seis directorios predeterminados guardados y existentes.
- 63 ejecutables/DLL revisados, 15.640 importaciones entre archivos empaquetados sin símbolos pendientes; marcas ASLR y DEP comprobadas.
- Pruebas aisladas de memoria, nombres de hilos, filesystem y contabilidad del heap aprobadas. [Resultados r2](evidence/compatible-r2.json).
- ClamAV 1.5.4, base diaria 28173: ZIP final sin detecciones ni errores. El resultado se vincula al SHA-256 del ZIP; no certifica ausencia de malware.
- Una carrera completa y el rendimiento de estos binarios siguen pendientes. No se garantizan 60 FPS.

## Sistemas operativos

| Sistema | Estado |
| --- | --- |
| Windows 11 x64 / Ryzen 7 5800X | Arranque local probado. |
| Windows 10 1809+ x64 | Objetivo del paquete; pendiente de validación real. |
| Otros Intel/AMD x64 con x86-64-v2 | Objetivo de la compatible; requiere pruebas en cada equipo. |
| Linux y macOS | Fuentes y pruebas pequeñas de CI aprobadas; sin paquetes propios completos validados. |
| Windows 7, 8, 8.1 y 32 bits | Fuera del objetivo de este runtime. |
| Windows ARM64 | Sin paquete nativo propio validado. |

Las pruebas de CI no compilan ni prueban el emulador completo en Linux o macOS.

## Compilar

En MSYS2 UCRT64, `bash build-ucrt64.sh` usa la edición compatible por defecto. Para Zen 3: `EGLANTINE_CPU_PROFILE=zen3 bash build-ucrt64.sh`. Los resultados se guardan en carpetas distintas. Evita rutas largas al compilar.

[Elegir e instalar una edición](EDICIONES.md) · [Qt para Windows](https://doc.qt.io/qt-6/windows.html) · [Plataformas MSYS2](https://www.msys2.org/docs/windows_support/).
