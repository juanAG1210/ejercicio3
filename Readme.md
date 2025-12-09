# assignment-sensor – Servicio systemd con sensor simulado

 un programa en C que se ejecuta
como servicio `systemd`, lee periódicamente un **sensor simulado** y registra
`timestamp + valor` en un archivo de log.

El objetivo es practicar:

- Cómo empacar un programa compilado como servicio `systemd`.
- Manejo básico de logs, argumentos de configuración y señales (SIGTERM).
- Builds reproducibles usando `make`.
- Documentación clara de instalación, pruebas y desinstalación.
- Evidencia de uso de asistentes de IA (carpeta `ai/`).

---

## 1. Descripción general

El binario principal se llama `assignment-sensor` y hace lo siguiente:

- Usa un “sensor falso” como fuente de datos (por defecto `/dev/urandom`).
- Cada **N segundos** (configurable) lee un valor de 32 bits del dispositivo.
- Por cada lectura escribe una línea en un log con este formato:

  ```text
  2025-09-19T15:03:21.123Z | 0xDEADBEEF

Es decir: timestamp ISO-8601 en UTC con milisegundos + valor en hexadecimal.

Si no se indica --logfile, intenta usar por defecto:

/tmp/assignment_sensor.log

Si /tmp no es escribible, hace fallback a /var/tmp/assignment_sensor.log.

Corre en bucle hasta recibir una señal SIGTERM (por ejemplo desde
systemctl stop), momento en el que:

Sale del bucle principal.

Escribe una línea de cierre en el log.

Cierra archivos y termina con código 0.

Si ocurre un error fatal de arranque (intervalo inválido, no puede abrir log,
no puede abrir el dispositivo), imprime un mensaje en stderr y termina con
código de salida distinto de 0, para que systemd pueda detectarlo.

Elección del “mock sensor”

Se usa /dev/urandom como sensor simulado porque:

Está disponible en prácticamente cualquier distribución Linux.

No se bloquea (a diferencia de /dev/random).

Permite obtener valores arbitrarios de 32 bits que se pueden formatear en
hexadecimal, lo que facilita ver que el sistema realmente está muestreando
datos distintos a lo largo del tiempo.

2. Estructura del repositorio
.
├─ src/
│  └─ assignment-sensor.c        # Programa en C
├─ systemd/
│  └─ assignment-sensor.service  # Unidad systemd
├─ tests/
│  └─ manual-tests.md            # Plan de pruebas manuales
├─ ai/
│  ├─ prompt-log.md              # Registro de prompts a la IA
│  ├─ reflection.md              # Reflexión corta sobre el uso de IA
│  └─ provenance.json            # Herramientas/modelos usados
├─ build/
│  └─ assignment-sensor          # Binario compilado (se genera con make)
├─ Makefile                      # Build con make
└─ README.md                     # Este archivo

3. Requisitos

Sistema Linux con systemd (por ejemplo Ubuntu, Debian, etc.).

Compilador C (gcc o clang).

Herramienta make.

Permisos para instalar:

Binarios en /usr/local/bin

Unidades systemd en /etc/systemd/system
(normalmente mediante sudo).

Funciona perfectamente dentro de WSL siempre que se tenga systemd habilitado.

4. Clonado y compilación

Clonar el repositorio:

# assignment-sensor — Servicio systemd con sensor simulado

Proyecto para la asignatura de Embedded Linux: un programa en C que se
ejecuta como servicio `systemd`, lee periódicamente un **sensor simulado** y
registra `timestamp + valor` en un archivo de log.

Objetivos principales:

- Empaquetar un programa compilado como servicio `systemd`.
- Manejar logging, argumentos de configuración y señales (SIGTERM).
- Producir builds reproducibles con `make`.
- Documentar instalación, pruebas y desinstalación.
- Mantener evidencia de uso de asistentes de IA en `ai/`.

---

**Contenido rápido**

- Binario: `build/assignment-sensor`
- Código fuente: `src/assignment-sensor.c`
- Unidad systemd: `systemd/assignment-sensor.service`
- Tests manuales: `tests/manual-tests.md`
- Evidencia IA: carpeta `ai/`

Índice
- Descripción general
- Estructura del repositorio
- Requisitos
- Clonado y compilación
- Ejecución y ejemplos
- Interfaz CLI
- Logging y fallback
- Instalación con systemd
- Configuración del servicio
- Pruebas
- Desinstalación
- Evidencia de uso de IA

## Descripción general

El binario principal se llama `assignment-sensor`. Comportamiento resumen:

- Usa un “sensor falso” como fuente de datos (por defecto `/dev/urandom`).
- Cada N segundos (configurable) lee un valor de 32 bits del dispositivo.
- Por cada lectura escribe una línea en el log con este formato:

```text
2025-09-19T15:03:21.123Z | 0xDEADBEEF
```

Es decir: timestamp ISO-8601 en UTC con milisegundos + valor en hexadecimal.

Si no se indica `--logfile`, el programa intenta por defecto:

- `/tmp/assignment_sensor.log`
- Si `/tmp` no es escribible, hace fallback a `/var/tmp/assignment_sensor.log`.

El programa corre en bucle hasta recibir una señal `SIGTERM` (por ejemplo
`systemctl stop`), momento en el que:

- Sale del bucle principal.
- Escribe una línea de cierre en el log.
- Cierra archivos y termina con código 0.

Si ocurre un error fatal de arranque (intervalo inválido, no puede abrir log,
no puede abrir el dispositivo), imprime un mensaje en `stderr` y termina con
código distinto de 0 para que `systemd` lo detecte.

### Elección del “mock sensor”

Se usa `/dev/urandom` como sensor simulado porque:

- Está disponible en prácticamente cualquier distribución Linux.
- No se bloquea (a diferencia de `/dev/random`).
- Permite obtener valores arbitrarios de 32 bits que se pueden formatear en
  hexadecimal, facilitando la verificación de muestreos distintos en el tiempo.

## Estructura del repositorio

.
├─ `src/`
│  └─ `assignment-sensor.c`        # Programa en C
├─ `systemd/`
│  └─ `assignment-sensor.service`  # Unidad systemd
├─ `tests/`
│  └─ `manual-tests.md`            # Plan de pruebas manuales
├─ `ai/`
│  ├─ `prompt-log.md`              # Registro cronológico de prompts
│  ├─ `reflection.md`              # Reflexión sobre el uso de IA
│  └─ `provenance.json`            # Herramientas/modelos usados
├─ `build/`
│  └─ `assignment-sensor`          # Binario compilado (generado por `make`)
├─ `Makefile`                       # Build con make
└─ `Readme.md`                      # Este archivo

## Requisitos

- Sistema Linux con `systemd` (por ejemplo Ubuntu, Debian, etc.).
- Compilador C (`gcc` o `clang`).
- `make`.
- Permisos para instalar binarios en `/usr/local/bin` y unidades en
  `/etc/systemd/system` (normalmente mediante `sudo`).

Funciona dentro de WSL siempre que `systemd` esté habilitado.

## Clonado y compilación

Clonar el repositorio:

```bash
git clone https://github.com/juanAG1210/ejercicio3.git
cd ejercicio3
```

Compilar:

```bash
make
```

El binario compilado queda en:

```
build/assignment-sensor
```

Probar directamente (sin systemd):

```bash
./build/assignment-sensor --interval 2
# En otra terminal:
tail -n 5 /tmp/assignment_sensor.log
```

Detener el proceso manualmente:

```bash
pkill -f assignment-sensor
```

## Interfaz de línea de comandos

Sintaxis general:

```
assignment-sensor [--interval <segundos>] [--logfile <ruta>] [--device <ruta>]
```

Opciones principales:

- `--interval <segundos>`: Intervalo de muestreo en segundos (entero). Rango
  válido: (0, 86400]. Por defecto: `5`.
- `--logfile <ruta>`: Ruta explícita del archivo de log. Si se proporciona y
  el archivo no se puede abrir, el programa imprime un error en `stderr` y
  termina con código ≠ 0 (no hace fallback silencioso).
- `--device <ruta>`: Dispositivo o archivo a usar como fuente de datos.
  Por defecto: `/dev/urandom`.
- `-h` / `--help`: Muestra la ayuda y termina.

### Comportamiento de logging y fallback

Si NO se pasa `--logfile`, se intenta abrir `/tmp/assignment_sensor.log` en
modo append. Si falla, se intenta `/var/tmp/assignment_sensor.log`. Si
ambas rutas fallan, se imprime un error en `stderr` y el programa termina con
código de salida ≠ 0.

El archivo de log se abre con buffer de línea (line-buffered) para evitar
escribir líneas cortadas en caso de parada repentina.

Formato típico del log:

```
# assignment-sensor started, logging to /tmp/assignment_sensor.log, interval=5 s, device=/dev/urandom
2025-09-19T15:03:21.123Z | 0xDEADBEEF
2025-09-19T15:03:26.124Z | 0x1234ABCD
# assignment-sensor stopping gracefully
```

## Instalación e integración con systemd

### 1) Instalar el binario

Desde la raíz del repositorio, tras compilar:

```bash
sudo make install
```

Esto instala el binario en:

```
/usr/local/bin/assignment-sensor
```

Alternativa manual:

```bash
sudo mkdir -p /usr/local/bin
sudo cp build/assignment-sensor /usr/local/bin/assignment-sensor
sudo chmod 755 /usr/local/bin/assignment-sensor
```

### 2) Instalar y habilitar la unidad systemd

Copiar el archivo de unidad:

```bash
sudo cp systemd/assignment-sensor.service /etc/systemd/system/assignment-sensor.service
sudo chmod 644 /etc/systemd/system/assignment-sensor.service
```

Recargar systemd y habilitar el servicio:

```bash
sudo systemctl daemon-reload
sudo systemctl enable --now assignment-sensor.service
```

A partir de este momento el servicio se arranca junto con el sistema y
ejecuta, por ejemplo:

```
/usr/local/bin/assignment-sensor --interval 5
```

El log se escribe en `/tmp/assignment_sensor.log`, o en
`/var/tmp/assignment_sensor.log` si `/tmp` no es escribible.

## Configuración del servicio

Para cambiar intervalo, ruta de log o dispositivo, edite
`/etc/systemd/system/assignment-sensor.service` y modifique la línea
`ExecStart`:

```ini
[Service]
Type=simple
ExecStart=/usr/local/bin/assignment-sensor \
  --interval 10 \
  --logfile /var/tmp/assignment_sensor.log \
  --device /dev/urandom
Restart=on-failure
RestartSec=2
```

Después de cualquier cambio en la unidad:

```bash
sudo systemctl daemon-reload
sudo systemctl restart assignment-sensor.service
```

### Ejecutar como usuario no root (opcional)

Por defecto el servicio corre como root. Para usar un usuario dedicado:

```bash
sudo useradd --system --no-create-home --shell /usr/sbin/nologin assignment-sensor
# Asegurar permisos en la ruta de log (por ejemplo /var/tmp)
# Editar la unidad y añadir o descomentar:
# User=assignment-sensor
# Group=assignment-sensor
sudo systemctl daemon-reload
sudo systemctl restart assignment-sensor.service
```

## Pruebas

El archivo `tests/manual-tests.md` contiene un plan detallado de pruebas
manuales. A continuación un resumen de los casos más importantes.

### Camino feliz (servicio funcionando)

```bash
sudo systemctl status assignment-sensor.service
```

Esperado: `active (running)`.

Ver últimas líneas del log:

```bash
tail -n 5 /tmp/assignment_sensor.log
# o, si hubo fallback:
tail -n 5 /var/tmp/assignment_sensor.log
```

### Fallback de `/tmp` a `/var/tmp`

En un entorno donde `/tmp` no sea escribible (por ejemplo un contenedor):

- Ejecutar el programa sin `--logfile`.
- Verificar que no hay log en `/tmp` y que el log se creó en
  `/var/tmp/assignment_sensor.log`.

### Manejo de SIGTERM

```bash
sudo systemctl stop assignment-sensor.service
sudo systemctl status assignment-sensor.service
tail -n 3 /tmp/assignment_sensor.log
```

Esperado: última línea similar a `# assignment-sensor stopping gracefully`.

### Ruta de fallo (dispositivo inválido)

```bash
./build/assignment-sensor --device /dev/fake0
echo "Código de salida: $?"
```

Esperado: mensaje de error en `stderr` y código de salida ≠ 0.

### Reinicio automático (Restart=on-failure)

Con la unidad configurada con `Restart=on-failure` y `RestartSec=2`:

```bash
pgrep -f assignment-sensor
sudo kill -9 <PID>
sudo systemctl status assignment-sensor.service
```

Esperado: systemd reinicia el servicio y vuelve a estar `active (running)`.

## Deshabilitar y desinstalar

### Deshabilitar el servicio

```bash
sudo systemctl disable --now assignment-sensor.service
sudo systemctl daemon-reload
```

### Eliminar archivos

Eliminar la unidad:

```bash
sudo rm -f /etc/systemd/system/assignment-sensor.service
sudo systemctl daemon-reload
```

Eliminar el binario:

```bash
sudo rm -f /usr/local/bin/assignment-sensor
```

Si se instaló con `make install`, también se puede ejecutar:

```bash
sudo make uninstall
```

Los logs en `/tmp` o `/var/tmp` se pueden eliminar manualmente.

## Evidencia de uso de IA (carpeta `ai/`)

El repositorio incluye la carpeta `ai/` con la evidencia requerida de
desarrollo asistido por IA:

- `ai/prompt-log.md`: Registro cronológico de prompts y respuestas resumidas,
  con referencia a commits/archivos relevantes.
- `ai/reflection.md`: Reflexión corta (≤ 500 palabras) sobre:

  - Qué se pidió a la IA.
  - Qué se aceptó o se modificó.
  - Cómo se validó el código y la documentación.
  - Qué se aprendió del proceso.

- `ai/provenance.json`: Archivo JSON donde se registran:
  - Nombre aproximado del modelo usado.
  - Proveedor (por ejemplo OpenAI).
  - Fechas aproximadas de uso.

---



