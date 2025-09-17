[x] Utilizar parametros de entrada para generar N procesos generadores
[] Implementar memoria compartida (SHM) para comunicación entre generadores y coordinador
	- [ ] Definir la estructura de memoria compartida y los nombres de semáforos POSIX
	- [ ] Inicializar SHM y semáforos en el main
	- [ ] Crear el hilo (o proceso) coordinador que lee de SHM y escribe en el CSV
	- [ ] Modificar los generadores para que escriban en SHM y usen los semáforos
	- [ ] Sincronizar la entrega de IDs (bloques de 10) y el acceso a SHM
	- [ ] Liberar recursos SHM y semáforos al finalizar
	- [ ] Validar funcionamiento y concurrencia
[] Hacer que los generadores soliciten bloques de 10 IDs al coordinador (no uno a uno)
[] El coordinador debe recibir registros por SHM y escribirlos en el CSV a medida que llegan
[] Sincronizar el acceso a SHM y la entrega de IDs usando semáforos
[] Definir claramente el rol del proceso coordinador (puede ser main o un hilo/proceso separado)
[] Validar que no haya condiciones de carrera ni pérdida de registros
[] (Opcional) Mejorar robustez ante errores de SHM/semaforización