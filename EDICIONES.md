# Elige una edicion para tu procesador

Las dos ediciones pertenecen al mismo repositorio. La tarjeta grafica y la cantidad de RAM no determinan las instrucciones del CPU.

| Edicion | Procesadores | Descarga y estado |
| --- | --- | --- |
| **Zen 3** | AMD Zen 3; referencia probada: Ryzen 7 5800X. Ejemplos de esa arquitectura: 5600X, 5900X y 5950X, sin pruebas propias en esos modelos. | [Release Zen 3](https://github.com/astralbeexd/Eglantine-3060Ti-16GB/releases/tag/v1.0.0-3060ti16gb-romfs1). El ZIP antiguo conserva su nombre 3060Ti-16GB. |
| **Compatible x86-64-v2** | Intel y AMD x64 que cumplan x86-64-v2. Usa esta opcion si no sabes si tu CPU es Zen 3. | [Descargar ZIP compatible](https://github.com/astralbeexd/Eglantine-3060Ti-16GB/releases/download/v1.0.0-compatible-x64-r2/Eglantine-Compatible-x64-r2.zip) · [Release y pruebas](https://github.com/astralbeexd/Eglantine-3060Ti-16GB/releases/tag/v1.0.0-compatible-x64-r2). Arranque probado en el 5800X; otros CPU pendientes de validación. |

El Xeon E5-2697 v3 del reporte sigue pendiente de una prueba real con esta edicion. No se declara resuelto su fallo sin esa comprobacion.

Ambas apuntan a Windows x64. La compatible tiene como objetivo Windows 10 1809+ y Windows 11; solo Windows 11 se ha probado localmente. Linux y macOS pasan comprobaciones pequenas de fuentes, pero no tienen paquetes propios completos validados.

## Instalacion

1. Elige la edicion correspondiente a tu **CPU**.
2. Extrae su ZIP completo en una carpeta nueva y de ruta corta. Conserva tu instalacion anterior.
3. Abre `Iniciar.cmd`; en la compatible puedes abrir `Iniciar-compatible.exe`.
4. Configura el mando y aporta tus propios archivos de consola y juegos que tengas derecho a utilizar.

No mezcles DLL ni ejecutables de las dos ediciones. No hay garantia de 60 FPS. Una carrera completa con los ultimos binarios compatibles sigue pendiente.

Consulta [compatibilidad y pruebas](COMPATIBILIDAD.md), [creditos](CREDITS.md), [licencias](LICENSE.txt) y [uso responsable](USO-RESPONSABLE.md).
