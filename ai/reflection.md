 Reflexión sobre el uso de IA (`reflection.md`)

Utilicé un asistente de IA principalmente para:

- Definir la arquitectura general del proyecto (elección del sensor simulado,
  estructura de carpetas, uso de `systemd` y `Makefile`).
- Obtener un primer borrador del código en C para el bucle de muestreo,
  manejo de señales y formato de timestamps.
- Generar plantillas para la documentación (README, plan de pruebas y
  archivos de la carpeta `ai/`).

  - Revisé el código C, ajusté comentarios, nombres de variables y mensajes de
  error para que fueran más claros para mí.
- Comprobé que las llamadas del sistema (por ejemplo `sleep`, `sigaction`,
  `clock_gettime`) y las opciones de `systemd` (`Restart=on-failure`,
  `WantedBy=multi-user.target`) tuvieran sentido revisando páginas de manual
  y ejemplos.
- Probé el binario localmente: lo ejecuté con distintos intervalos,
  rutas de log y dispositivos, verificando el comportamiento esperado en
  cada caso (camino feliz, errores y stop con SIGTERM).

  a IA aceleró mucho la parte de “plantilla”, pero la
responsabilidad final del código es mía. Yo tuve que:

- Asegurarme de que el programa realmente terminara con códigos de salida
  correctos en caso de error.
- Verificar que el formato de log cumpliera los requisitos (timestamp ISO-8601 y
  líneas completas sin cortes).
- Ajustar la documentación para que coincidiera con los pasos que seguí en mi entorno.
