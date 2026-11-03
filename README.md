# Eglantine · Zen 3 y Compatible x86-64-v2

**[Elige la edición según tu procesador](EDICIONES.md)**

| Edición | CPU | Estado |
| --- | --- | --- |
| [Zen 3](ediciones/zen3/README.md) | AMD Zen 3; referencia: Ryzen 7 5800X | ZIP público disponible. |
| [Compatible](ediciones/compatible/README.md) | Más Intel y AMD x64 con x86-64-v2 | ZIP preparado; subida pública pendiente. |

Emulador comunitario **experimental para Windows**, basado en Eglantine. Pensado para probarlo sin compilar ni instalar MSYS2.

## Empieza aquí

### 1. Descarga el paquete completo — edición Zen 3

**[⬇ Descargar el emulador para Windows (ZIP, unos 69 MB)](https://github.com/astralbeexd/Eglantine-3060Ti-16GB/releases/download/v1.0.0-3060ti16gb-romfs1/Eglantine-3060Ti-16GB-carga-corregida-portable.zip)**

También puedes abrir la [página de descarga](https://github.com/astralbeexd/Eglantine-3060Ti-16GB/releases/tag/v1.0.0-3060ti16gb-romfs1) y buscar **Assets**.

Elige **Eglantine-3060Ti-16GB-carga-corregida-portable.zip**. Los archivos “Source code” son para desarrolladores.

### 2. Extrae la carpeta

1. Abre **Descargas** en el Explorador de archivos.
2. Haz clic derecho sobre el ZIP y selecciona **Extraer todo…**.
3. Pulsa **Extraer** y abre la carpeta extraída.
4. Entra en **Eglantine-3060Ti-16GB**, donde están el programa y las DLL.

### 3. Abre el emulador

Haz doble clic en **Iniciar.cmd** (puede aparecer simplemente como **Iniciar**).
También puedes abrir **eglantine.exe**. Mantén juntos los archivos de la carpeta.

No necesitas ejecutar como administrador, instalar MSYS2 ni abrir una terminal.
Si Windows muestra una advertencia, comprueba la procedencia y los hashes de la release antes de decidir si ejecutarlo.

### 4. Configura el mando y tus archivos

1. Conecta tu mando.
2. Abre **Emulation → Configure → Controls** y selecciona tu dispositivo.
3. Aporta los archivos necesarios de tu propia consola mediante las opciones de instalación del menú **Tools**.
4. Abre **File → Load File…** para seleccionar un juego obtenido legítimamente.
5. Para ver tu colección en la pantalla principal, usa **Add New Game Directory** y selecciona su carpeta.

Los nombres de los menús pueden variar con el idioma. Los juegos, claves y firmware se aportan por separado.

## ¿Ya tienes nuestra versión anterior?

Descarga [la actualización de ejecutables](https://github.com/astralbeexd/Eglantine-3060Ti-16GB/releases/tag/v1.0.0-3060ti16gb-romfs1).
Cierra el emulador, extrae el ZIP de actualización y copia **eglantine.exe** y **eden-cli.exe**
a tu carpeta existente, aceptando reemplazar esos dos archivos. Conserva **user**:
contiene tu configuración y puede contener partidas y archivos de consola.

## Compatibilidad con otros equipos

El ZIP publicado actualmente sigue siendo la edición **Zen 3**. Ya se compiló un candidato
genérico x86-64-v2 y pasó la prueba de arranque en Windows 11. Su validación en otros CPU,
en Windows 10 y en una carrera completa sigue pendiente; aún no tiene una release binaria pública.
Consulta [qué CPU y sistemas operativos están probados y cuáles siguen pendientes](COMPATIBILIDAD.md).

## Antes de probar

- Hardware de referencia: **Ryzen 7 5800X, RTX 3060 Ti de 8 GB y 16 GB de RAM**.
- Esta compilación se dirige a **Zen 3**; no está validada en otros procesadores. Tener una 3060 Ti por sí solo no garantiza compatibilidad.
- Perfil incluido: escala **1x**, CPU Normal y límite de velocidad **100 %**. El juego puede conservar su resolución dinámica.
- Usa una sola instancia y deja disponible la paginación de Windows.
- La primera sesión puede tener tirones por compilación de shaders.
- **No se garantizan 60 FPS, ausencia de fallos ni estabilidad en todos los juegos.**

## Si algo falla

| Lo que ves | Qué hacer |
| --- | --- |
| Faltan DLL | Extrae nuevamente el ZIP completo y conserva juntos todos sus archivos. |
| Faltan claves o firmware | Comprueba que tus archivos propios correspondan a tu consola y a la versión necesaria. |
| La lista de juegos está vacía | Usa Add New Game Directory o abre un archivo desde File. |
| El mando no responde | Revisa Controls y el dispositivo seleccionado. |
| Se congela o se cierra | Cierra esa instancia antes de volver a probar. Anota el juego, versión y momento del fallo. |
| Necesitas reportar un problema | Consulta [cómo informar de un fallo](GUIA-RAPIDA.md#informar-de-un-fallo). Revisa el log antes de compartirlo. |

## Pruebas humanas y uso de IA

**astralbeexd, responsable del repositorio, probó personalmente el emulador y comunicó sus observaciones de juego.**
Eso no equivale a una auditoría humana completa ni a validar todas las pistas o juegos.

Las modificaciones de esta edición y su documentación fueron realizadas con asistencia de inteligencia artificial.
**No es una reescritura total desde cero:** conserva código de Eglantine, Eden, yuzu y de sus dependencias.
Los créditos y licencias originales siguen correspondiendo a sus autores.

La corrección de carga pasó pruebas aisladas: índice RomFS de 62,948 s a 0,993 s con el mismo hash de cabecera y tablas;
la prueba de retención de metadatos también pasó. ClamAV no encontró detecciones en los paquetes identificados por sus hashes.
**Una carrera completa con estos últimos binarios sigue pendiente de validación.**

## Uso responsable

Este proyecto no distribuye ni enlaza juegos comerciales, claves de consola, firmware ni partidas personales.
Usa únicamente archivos que tengas derecho a utilizar. Tener un juego no implica por sí solo permiso para descargar
copias de terceros o eludir protecciones. No compartas esos materiales en issues ni en el repositorio.
Consulta [las pautas de uso responsable](USO-RESPONSABLE.md).

## Licencias, fuentes y referencias

- [Licencia del programa](LICENSE.txt) y [avisos de licencias](LICENSES/).
- [Créditos y proyectos de origen](CREDITS.md).
- [Cambios técnicos](CHANGES.md) · [README técnico anterior](DOCUMENTACION-TECNICA.md).
- [Fuentes completas y recetas de dependencias](https://github.com/astralbeexd/Eglantine-3060Ti-16GB/releases/tag/security-sources-21a037c36009).
- [Seguridad y límites de la revisión](SECURITY-REVIEW.md) · [estado de validación](RELEASE-STATUS.json).
- [Hashes e informes de los paquetes](https://github.com/astralbeexd/Eglantine-3060Ti-16GB/releases/tag/v1.0.0-3060ti16gb-romfs1).

Proyecto comunitario no oficial, sin afiliación ni respaldo de Nintendo. Se conservan las licencias por archivo y dependencia.
