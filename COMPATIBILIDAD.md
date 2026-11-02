# Compatibilidad: objetivos y pruebas

## Paquete actualmente publicado

La release `v1.0.0-3060ti16gb-romfs1` contiene ejecutables **Zen 3**.
No son universales y no se vuelven genÃƒÂ©ricos al actualizar el README o el script.
La GPU y la cantidad de RAM no determinan quÃƒÂ© instrucciones admite el procesador.

## EdiciÃƒÂ³n compatible en preparaciÃƒÂ³n

- CÃƒÂ³digo C/C++ compilado con `-march=x86-64-v2 -mtune=generic`.
- CompilaciÃƒÂ³n completa separada de la ediciÃƒÂ³n Zen 3, incluidas las bibliotecas estÃƒÂ¡ticas.
- Se deben comprobar tambiÃƒÂ©n las DLL, el inicio, el uso de instrucciones detectadas en ejecuciÃƒÂ³n y el antivirus.
- Objetivo Windows: **10 de 64 bits (1809 o posterior) y 11**. La prueba local disponible es Windows 11;
  Windows 10 y el Xeon E5-2697 v3 necesitan comprobaciÃƒÂ³n en mÃƒÂ¡quinas reales antes de declararlos validados.
- Lanzador preparado para informar si el CPU o Windows no cumplen el requisito, o si el proceso termina con error.
- Tener un CPU x86-64-v2 no garantiza que cualquier GPU, controlador o juego funcione.

## Sistemas operativos

| Sistema | Estado de esta distribuciÃƒÂ³n |
| --- | --- |
| Windows 11 x64 / Zen 3 | Paquete publicado; pruebas locales y lÃƒÂ­mites descritos en la release. |
| Windows 10 1809+ x64 | Objetivo de la ediciÃƒÂ³n genÃƒÂ©rica; validaciÃƒÂ³n del paquete pendiente. |
| Linux x64 | Comprobaciones de fuentes y pequeÃƒÂ±as pruebas multiplataforma; aÃƒÂºn sin paquete propio validado. |
| macOS | Comprobaciones de fuentes y pequeÃƒÂ±as pruebas; aÃƒÂºn sin paquete propio validado. |
| Windows 7, 8, 8.1 o 32 bits | Fuera del objetivo de este runtime. |
| Windows ARM64 | Sin paquete nativo propio validado. |

Las comprobaciones de CI no compilan ni prueban el emulador completo en Linux/macOS.
Un resultado verde no acredita compatibilidad de juegos ni rendimiento en esos sistemas.

## Compilar desde fuentes

En MSYS2 UCRT64, el script usa el perfil genÃƒÂ©rico por defecto:

```sh
bash build-ucrt64.sh
```

Para una compilaciÃƒÂ³n especÃƒÂ­fica de Zen 3:

```sh
EGLANTINE_CPU_PROFILE=zen3 bash build-ucrt64.sh
```

Los resultados se guardan en carpetas distintas. Evita rutas largas al compilar en Windows.
Los binarios nuevos sÃƒÂ³lo se anunciarÃƒÂ¡n como descargables despuÃƒÂ©s de empaquetarlos y verificarlos.

Referencias: [Qt para Windows](https://doc.qt.io/qt-6/windows.html),
[polÃƒÂ­tica de plataformas MSYS2](https://www.msys2.org/docs/windows_support/).
El soporte de una dependencia no sustituye las pruebas de esta aplicaciÃƒÂ³n.

## Evidencia de esta compilaciÃ³n

[Resultados de compatibilidad](evidence/compatibility-validation.json). Las comprobaciones pequeÃ±as de fuentes y pruebas aisladas pasaron en los tres sistemas de CI; no se trata de tres compilaciones completas del emulador.
