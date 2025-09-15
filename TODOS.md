
[] Implementar memoria compartida (SHM) para comunicación entre generadores y coordinador
[] Hacer que los generadores soliciten bloques de 10 IDs al coordinador (no uno a uno)
[] El coordinador debe recibir registros por SHM y escribirlos en el CSV a medida que llegan
[] Sincronizar el acceso a SHM y la entrega de IDs usando semáforos
[] Definir claramente el rol del proceso coordinador (puede ser main o un hilo/proceso separado)
[] Validar que no haya condiciones de carrera ni pérdida de registros
[] (Opcional) Mejorar robustez ante errores de SHM/semaforización