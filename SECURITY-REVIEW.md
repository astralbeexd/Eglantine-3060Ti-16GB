# Revisión de seguridad en curso

El candidato inicial fue analizado localmente con ClamAV 1.5.4 y bases actualizadas:
287 archivos examinados, cero detecciones, salida 0, sin errores ni alertas de límite.
Es un resultado para los hashes registrados en `evidence/ANTIVIRUS-INITIAL.json`.
No certifica ausencia de malware ni cubre automáticamente los binarios recompilados después.

## Fuentes y procedencia

Se reunieron 82 archivos fuente de paquetes MSYS2 (dependencias dinámicas y de compilación),
las fuentes de 15 paquetes obtenidos mediante CPM y el código del generador de zonas horarias.
Se verificaron matemáticamente 81 firmas localmente. El llavero usado coincide por SHA-256
con el llavero oficial actual de MSYS2. La entrega automática comprobará con GnuPG las
82 firmas y los hashes de los demás archivos antes de publicar una prerelease sólo de fuentes.
Incluye el submódulo exacto del generador de datos horarios y las recetas/parches necesarios.
Los archivos públicos no incluyen claves de consola, juegos, firmware, partidas ni datos de pruebas privados.

## Correcciones preparadas

- **zlib:** compilación del commit oficial `6be4bcda3da1d28b18a081c884b141a88a7cb84d`,
  que contiene la corrección `df84af25dc1942490e1d1c899a07619152a46148` para
  CVE-2026-76844 y CVE-2026-85091. Pasaron 14/14 pruebas CTest y una prueba de
  compatibilidad enlazada con los headers y la biblioteca de importación anteriores.
  Es una revisión de desarrollo de upstream, no una nueva publicación estable de zlib.
- **FFmpeg:** versión 9.0.2 con el parche oficial `2c2f6e96e31795ae95a8f8323a493ffbc493111f`
  para CVE-2026-107676. Compilación reducida a decodificadores de vídeo relevantes,
  sin red, libavformat, filtros ni codificadores; conserva CUDA/NVDEC, D3D11VA y DXVA2.
  La compilación remota terminó correctamente y se verificó el digest del artefacto descargado.
- **cpp-httplib:** actualización de 0.46.0 a 0.51.0, versión que corrige los cuatro avisos
  encontrados para la versión anterior. Se retiró el viejo parche MinGW y se usa el
  resolvedor DNS normal del sistema: puede esperar al timeout de Windows al resolver nombres.
  Las pruebas locales verificaron la eliminación de cookies al redirigir a otro origen y
  la protección de la ruta de streaming frente a inyección de cabeceras.
- **Markdown:** el diálogo de novedades usa texto plano en lugar de `setMarkdown`.
  El actualizador continúa desactivado. No queda una llamada explícita a las APIs Markdown
  en el código de la aplicación; esto limita una entrada al aviso OSV-2022-126, pero no
  convierte la biblioteca md4c incluida en Qt en una biblioteca corregida globalmente.

La reducción final de DLL debe comprobarse mediante el grafo real de importaciones y pruebas
del paquete antes de clasificar bibliotecas como ausentes. No se han descartado avisos sólo
porque ClamAV no haya detectado malware. Los avisos de paquetes retirados se mantendrán
en el registro histórico y la evaluación final indicará su aplicabilidad al paquete concreto.

## Estado actual

Las fuentes completas de dependencias se publicaron en la prerelease sólo de fuentes
`security-sources-98749eca2392`; la tarea verificó las 82 firmas MSYS2 con GnuPG.
La compilación del programa terminó. El candidato de seguridad anterior pasó ClamAV:
226 archivos, cero detecciones, salida 0. Ese resultado no cubre los ejecutables nuevos
que corrigen la carga de RomFS hasta repetir el análisis.

La corrección de carga pasó las pruebas documentadas en
`evidence/ROMFS-LOADING-REGRESSION.json`. La carrera completa sigue pendiente.
Se añadió una protección al banco de pruebas privado para no abrir el emulador con
margen insuficiente de memoria comprometida. No se modificó la configuración del
archivo de paginación ni del antivirus de Windows. No hay garantía de seguridad ni 60 FPS.
