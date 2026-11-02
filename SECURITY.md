# Seguridad y alcance

Proyecto experimental con asistencia de IA. No existe una certificación de seguridad,
una auditoría profesional independiente ni una garantía de ausencia de malware o fallos.
El código público permite revisar los cambios antes de compilar.

## Estado de los binarios

No hay todavía una release binaria pública aprobada. El candidato local no está firmado
con Authenticode; no se debe presentar como de un editor verificado.
Un SHA-256 comprueba identidad respecto al archivo publicado; no demuestra que sea seguro.
Las comprobaciones estructurales PE, los hashes y una prueba de arranque tampoco sustituyen
el análisis antivirus ni la revisión de vulnerabilidades.

El intento de análisis local con Microsoft Defender falló con `0x80508023`.
Después se analizó el candidato inicial con ClamAV 1.5.4: 287 archivos y cero detecciones.
Los cambios posteriores requieren otro análisis del paquete final. No se afirma ausencia de vulnerabilidades.
No se han desactivado protecciones de Windows ni añadido exclusiones para ejecutar la build.

## Uso

- Usa una cuenta normal de Windows; el emulador portátil no necesita ejecutarse como administrador.
- No desactives el antivirus, SmartScreen o el firewall para hacerlo funcionar.
- No obtengas claves, firmware ni juegos de paquetes de terceros que los incluyan con el emulador.
- Usa copias de seguridad de tus partidas. Los fallos del emulador también pueden afectar datos.
- Las imágenes de juego, mods y shaders son entradas complejas: no se garantiza aislamiento de archivos maliciosos.
- El perfil público conserva modo avión y censura del nombre de usuario en los registros.
- Revisa cualquier registro antes de compartirlo: aun con censura puede contener rutas o identificadores.

## Procedencia y automatización

La base y los cambios incorporados están identificados por commit en BUILD-INFO.json y CREDITS.md.
SOURCE-MANIFEST.json y la comprobación automática verifican que los parches reproducen
los veinte archivos publicados. La acción externa está fijada por SHA; el trabajo tiene
permisos de sólo lectura, no usa secretos y no publica ejecutables automáticamente.
El runner del sistema y los paquetes instalados no están fijados completamente: no se
afirma una compilación reproducible bit a bit.

El SBOM describe bibliotecas dinámicas del candidato; no es un inventario completo de
componentes estáticos ni un resultado de búsqueda de vulnerabilidades.
Las bibliotecas deben distribuirse con sus avisos y fuentes correspondientes cuando sea exigible.

## Informar de un problema

No publiques claves, tokens, partidas, archivos personales ni pruebas de explotación con
datos privados en un issue. Describe versión y síntoma con una reproducción mínima
y registros previamente revisados. Si identificas una vulnerabilidad de una dependencia,
incluye el identificador del aviso y la versión afectada para poder comprobarlo.
