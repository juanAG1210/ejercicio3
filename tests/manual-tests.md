# Plan de pruebas manuales para `assignment-sensor`

Este documento describe pruebas manuales para verificar:

- Funcionamiento normal del servicio.
- Fallback de `/tmp` a `/var/tmp`.
- Manejo de SIGTERM (apagado limpio).
- Manejo de errores y reinicios (`Restart=on-failure`).

---

## 1. Camino feliz (servicio logueando normalmente)

1. Compilar e instalar:

   ```bash
   make
   sudo make install
   sudo cp systemd/assignment-sensor.service /etc/systemd/system/assignment-sensor.service
   sudo systemctl daemon-reload
   sudo systemctl enable --now assignment-sensor.service

 #"Comprobar estado:"

sudo systemctl status assignment-sensor.service


"Esperado: active (running)."

#"Después de unos intervalos (por ejemplo 15 s), revisar el log:"

tail -n 5 /tmp/assignment_sensor.log