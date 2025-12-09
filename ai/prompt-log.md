### Prompt 1
- **Prompt:** "Tengo una tarea de Embedded Linux, necesito un servicio systemd que lea un sensor falso y loguee timestamp + valor. ¿Me ayudas a plantear el proyecto?"
- **Respuesta (resumen):** La IA propuso usar `/dev/urandom` como sensor simulado, log por defecto en `/tmp` con fallback a `/var/tmp`, estructura del repositorio y un esqueleto de programa en C, Makefile, unidad systemd y README.

- **Commits / archivos relacionados:**
  - `src/assignment-sensor.c` (versión inicial)
  - `systemd/assignment-sensor.service`
  - `README.md` (borrador inicial)

### Prompt 2
- **Prompt:** "Explícame cómo manejar SIGTERM y evitar bucles ocupados en el lazo de muestreo."
- **Respuesta (resumen):** Sugirió usar una variable global `sig_atomic_t` modificada en el handler de señal y hacer el `sleep` en tramos de 1 segundo verificando la bandera, además de hacer `fflush` del log.

- **Commits / archivos relacionados:**
  - Cambios en `src/assignment-sensor.c` para manejo de señales y bucle de espera.