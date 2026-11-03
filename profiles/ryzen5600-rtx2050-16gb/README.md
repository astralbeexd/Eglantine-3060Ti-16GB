# Perfil experimental: Ryzen 5 5600 / RTX 2050 / 16 GB

Objetivo: probar Mario Kart World con una RTX 2050 de **4 GB de VRAM**, Ryzen 5 5600 y 16 GB de RAM. La variante exacta de la tarjeta y sus limites de potencia pueden cambiar el resultado. No se ha probado en ese equipo.

## Instalacion del paquete preparado

1. Extrae el ZIP completo en una carpeta nueva y corta. Conserva tu instalacion anterior.
2. Abre **Iniciar-4GB.cmd**. No requiere MSYS2 ni administrador.
3. Configura tu mando y aporta tus propios archivos de consola y juegos que tengas derecho a utilizar.

La carpeta completa incluye el runtime; estos tres archivos del repositorio por si solos no son un emulador instalable.

## Que cambia

- Limite de **tres compiladores de shaders simultaneos** para reducir los picos de RAM y la competencia por CPU. La primera compilacion puede tardar mas; requiere comparar en el equipo objetivo.
- Activa la politica experimental existente para contabilizar con mas prudencia las reservas de memoria de Vulkan. Puede provocar mas limpieza de cache; no garantiza menos tirones.
- Configura explicitamente BC3 y uso conservador de VRAM. El codigo base ya seleccionaba BC3 para juegos Switch 2 en tarjetas pequenas; no es una compresion nueva. Puede cambiar la calidad de texturas.
- Mantiene **escala 1x y velocidad 100 %**, incluido turbo a 100 %. No fuerza una resolucion 3D interna: el juego puede usar resolucion dinamica.
- Usa los mismos ejecutables del emulador que Compatible r2. El lanzador nuevo reconoce `--perfil-4gb` y conserva los argumentos del juego.

No se ha medido una mejora de FPS ni validado una carrera con este perfil. Una prueba en la RTX 3060 Ti tampoco validaria el rendimiento de una RTX 2050. No se reduce artificialmente la memoria que necesita la consola emulada.

## Validacion y limites

Pasaron las pruebas del lanzador con Clang y GCC: argumentos Unicode, rutas con espacios, seleccion del perfil y variables heredadas. Las pruebas no ejecutan un juego. La prueba de carrera local queda pendiente mientras el margen de memoria comprometida sea insuficiente; no se cierran aplicaciones del usuario para forzarla.

Para comparar, usa la misma carrera y configuracion, registra FPS, velocidad de emulacion, tirones, RAM y VRAM, e indica si la cache de shaders estaba fria o caliente. No presentes el perfil como estable o probado en una RTX 2050 hasta hacer esa comprobacion.

Basado en Eglantine/Eden/yuzu; cambios de esta distribucion con asistencia de IA. Consulta las licencias y creditos del paquete. No se incluyen juegos, claves, firmware ni partidas.
