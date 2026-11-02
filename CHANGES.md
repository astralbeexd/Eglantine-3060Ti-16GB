# Cambios locales frente a la base fijada

Base: 6bde8c20d1f7c020a920e59a989c03822a08445d.

1. `host_memory`: VirtualFree con tamaño cero para MEM_RELEASE; limpieza parcial segura,
   transferencia de propiedad al mover buffers y reintentos sólo ante ERROR_COMMITMENT_LIMIT.
2. `heap_accounting` / Vulkan: impedir restas sin signo inconsistentes. Perfil opcional que
   limita el descuento de espacio libre VMA a 256 MiB; desactivado en la configuración elegida.
3. `texture_cache`: protección de edad y margen configurable para experimentos, sin imponer
   los umbrales agresivos que produjeron tirones durante las pruebas.
4. Shaders Vulkan/OpenGL y `shader_environment`: reutilizar el resultado de transformación
   nativa de Wael3rd para preparar variantes. Opt-out diagnóstico y contadores opcionales.
5. `self_controller`: manejador explícito 2400 con la misma respuesta de éxito del auto-stub;
   no se implementa una función desconocida por el mero hecho de quitar el aviso.
6. Servicios: limitar mensajes de auto-stubs repetidos; respuestas sin cambios, otros errores conservados.
7. Nombres de hilos Windows en MinGW; estadísticas frontend, tiempos de presentación,
   capturas 1080p e inicio TAS opcionales. Desactivados para jugar.
8. CMake: ThinLTO sólo en video_core y opción de PGO experimental, desactivada en la build elegida.
9. Perfil: CPU Normal, GPU Boost sólo para consultas de tiempo, modo portátil 1x,
   límites 100%, presentación/shaders asíncronos y reactive flushing apagado.

## Verificación local y límites

GUI y CLI compilaron con Clang 23.1.4; jugabilidad probada sólo en GUI.
Pruebas de contabilidad: 2.097.152 combinaciones más casos límite; pruebas Windows
de fastmem/fallback, alias, movimientos, limpieza y handles; reintentos y nombres de hilos.
Los parches se aplicaron a la base exacta y reprodujeron los veinte archivos modificados.
Sin DEVICE_LOST, OUT_OF_DEVICE/HOST_MEMORY, PANIC o fallo fastmem en tres registros seleccionados.
Esto no prueba ausencia de errores en otras escenas, juegos o equipos.

Tramo de 3000 presentaciones: reutilización apagada en v8, 53,43 FPS;
reutilización activa con perfil VMA experimental, 57,33 FPS;
reutilización activa con contabilidad VMA original, 56,90 FPS.
Entradas iniciales equivalentes pero trayectoria y carga variables; sin repeticiones suficientes
para atribuir una mejora porcentual causal. El último perfil redujo intervalos >25 ms
de 1,8% a 0,97% en ese tramo; también hubo P99 posteriores de 61 ms.
No se consiguieron 60 FPS estables. Las mediciones antiguas con CPU Overclock y escala
0,5x no se usan como prueba de velocidad normal a 1080p.
