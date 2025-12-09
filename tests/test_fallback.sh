echo "Preparar un entorno donde /tmp no sea escribible por el proceso."

echo "Ejecutar el programa sin indicar --logfile:"

./assignment-sensor --interval 2


echo "Verificar:"

ls -l /tmp/assignment_sensor.log        # No debería existir
tail -n 5 /var/tmp/assignment_sensor.log


echo "Esperado: el log se crea y escribe en /var/tmp."