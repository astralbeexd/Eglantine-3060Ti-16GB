# Distribución

El código del programa se ofrece conforme a GPL-3.0-or-later, con los avisos por archivo
y las licencias de terceros conservados. Los parches se aplican a una revisión concreta;
el script de compilación descarga esa base y las dependencias.

Antes de publicar un ZIP binario debe adjuntarse el código fuente correspondiente de la
build, sus scripts y el de las dependencias redistribuidas que lo requieran. Una lista de
nombres de bibliotecas o enlaces genéricos no sustituye esa entrega.
El candidato local contiene avisos y un inventario de paquetes/versiones; ese inventario
no certifica por sí solo el cumplimiento de todas las obligaciones de redistribución.

El archivo `runtime-packages.json` vincula cada DLL del candidato con el paquete MSYS2
instalado y su hash. Deben conservarse sus licencias y recopilar las fuentes exactas y
recetas de construcción antes de marcar la release binaria como lista.
FFmpeg y otros componentes pueden tener condiciones diferentes según su configuración.

No subas `user/` de una instalación personal. El perfil público se escribe desde cero:
no incluye claves, firmware, NAND, tarjetas SD, partidas, cachés, identificadores de mandos,
rutas de juegos, credenciales, registros ni configuraciones de red personales.
