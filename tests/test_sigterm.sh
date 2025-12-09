echo "Con el servicio corriendo:"

sudo systemctl stop assignment-sensor.service


echo "Revisar estado:"

sudo systemctl status assignment-sensor.service


Esperado: inactive (dead).

echo "Revisar final del log:"

tail -n 3 /tmp/assignment_sensor.log


echo "Esperado: última línea similar a:"

# assignment-sensor stopping gracefully


echo "No debe haber líneas cortadas o incompletas."