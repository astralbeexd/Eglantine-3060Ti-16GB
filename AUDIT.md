# Verificaciones del candidato portátil

Pruebas locales del 1 de noviembre de 2026. Esto es evidencia limitada, no certificación de seguridad.

| Comprobación | Resultado |
|---|---|
| Perfil público escrito desde cero | Sin claves, NAND, partidas, cachés, logs ni rutas personales |
| Plugins Qt | Una ubicación explícita mediante qt.conf; eliminadas doce DLL duplicadas |
| Runtime | 115 DLL, todas coinciden por SHA-256 con archivos de paquetes instalados |
| Dependencias declaradas | 74 paquetes; inventario y SBOM disponibles |
| Importaciones PE estáticas | Sin importaciones faltantes frente a paquete/Windows/API sets |
| ASLR y DEP | Marcas presentes en los 117 archivos PE revisados |
| Arranque aislado | 30 segundos activo, PATH limitado a Windows, sin módulos de MSYS2 cargados |
| Prueba del juego con perfil limpio | 6.000 presentaciones, carrera visible a 1080p, terminación controlada |
| Firma Authenticode | Ausente; no hay un editor verificado |
| Antivirus | Microsoft Defender devolvió 0x80508023; no se obtuvo un resultado válido |
| Vulnerabilidades de todas las dependencias | Revisión todavía no completada |
| Fuentes de dependencias para redistribuir binarios | Todavía incompletas |
| Auditoría profesional independiente | No realizada |

El arranque no demuestra aislamiento total de Windows ni ausencia de accesos a red o archivos.
La prueba del juego no mostró DEVICE_LOST, OUT_OF_DEVICE/HOST_MEMORY, PANIC ni fallo fastmem;
se usaron copias privadas de los archivos necesarios únicamente en una carpeta de prueba.
Esos datos no forman parte del paquete público. No es una prueba completa de todas las pistas.
El perfil público usa precisión de CPU Auto; las cifras históricas de rendimiento se midieron
con Unsafe en la instalación personal. Auto también activa algunas optimizaciones relajadas
de Dynarmic en esta base. La precisión de emulación no equivale a seguridad del proceso.
Se cargó un módulo externo llamado tiptsf.dll, además de los del paquete y del directorio Windows;
no se emitió un veredicto de confianza sobre ese módulo a partir de su nombre.
La comparación con paquetes instalados no sustituye una reverificación independiente de las
firmas oficiales de dichos paquetes. El SBOM no cubre completamente componentes estáticos.
Las marcas ASLR/DEP no prueban que todas las rutas de ejecución estén protegidas.

No se modificaron el antivirus, sus exclusiones, SmartScreen, firewall, BIOS ni driver.
El modo avión configurado es del emulador y no cambia la configuración de red de Windows.
No se subió el binario a servicios antivirus de terceros ni se publicaron datos personales.

## Comprobación del código público

SOURCE-MANIFEST.json fija los hashes de los parches, los veinte archivos modificados,
el perfil y el script de compilación. tools/verify_source.py aplica los parches a una
base exacta y compara los resultados, además de buscar tipos de archivo privados y secretos conocidos.
La búsqueda automática tiene alcance limitado y puede omitir secretos con otros formatos.

El workflow tiene permisos de lectura, checkout fijado por SHA y sin credenciales persistentes.
No utiliza secretos ni publica releases automáticamente. Su estado debe comprobarse en Actions;
añadir el archivo de workflow no significa por sí solo que la ejecución haya pasado.

La build mantiene desactivados el actualizador y la presencia de Discord mediante sus opciones de compilación.
No se garantiza una compilación reproducible bit a bit ni seguridad de todas las entradas de juego/mods.

La comprobación remota del commit 837f8e2 terminó correctamente: [ejecución de GitHub Actions](https://github.com/astralbeexd/Eglantine-3060Ti-16GB/actions/runs/41379971968).
Verifica los parches y la contabilidad; no certifica seguridad del ejecutable.
