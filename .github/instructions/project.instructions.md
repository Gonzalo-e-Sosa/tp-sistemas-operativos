---
applyTo: '**'
---

1. **Project Context**:
   - Este proyecto corresponde a la asignatura **Sistemas Operativos** (Ingeniería en Informática, 2025, 2do Cuatrimestre).
   - Los trabajos prácticos deben implementarse en **C**.
   - Se desarrollarán dos ejercicios principales:
     1. **Generador de Datos de Prueba con Procesos y Memoria Compartida**.
     2. **Cliente-Servidor de Micro Base de Datos con Transacciones**.
   - El entorno de ejecución es **Linux**, con monitoreo mediante comandos como `ps`, `htop`, `vmstat`, `ipcs`, `netstat`, `lsof`.

2. **Coding Guidelines**:
   - Todo el código debe estar escrito en **C estándar**, siguiendo buenas prácticas de programación estructurada y modular.
   - Se debe entregar **código fuente + Makefile** que compile sin errores ni warnings.
   - No se aceptan binarios precompilados.
   - Utilizar **memoria compartida (SHM)**, **semaforización** y **sockets TCP/IP** donde corresponda.
   - Implementar manejo controlado de errores y validación de parámetros de entrada.
   - Liberar siempre los recursos del sistema (memoria compartida, semáforos, archivos temporales, sockets).
   - El CSV debe contener en la primera fila el encabezado de columnas, siendo **ID** el primer campo obligatorio.
   - En el servidor, garantizar consistencia de datos mediante **transacciones con bloqueo exclusivo**.

3. **Delivery Guidelines**:
   - Todo debe entregarse en un archivo **ZIP** que contenga:
     - Código fuente en C.
     - Makefile.
     - Lote de prueba y archivos auxiliares.
     - Evidencias de monitoreo en Linux.
   - Validación automática:
     - Los **IDs deben ser correlativos y únicos** (sin saltos ni duplicados).
     - Los parámetros incorrectos deben mostrar un mensaje de ayuda claro.

4. **AI Interaction**:
   - Al generar código, seguir estrictamente los requisitos del enunciado.
   - Explicar el razonamiento detrás de estructuras como procesos, memoria compartida, semáforos o sockets.
   - Proponer mejoras en robustez, control de errores y modularidad del código.
   - Sugerir ejemplos de comandos de monitoreo y scripts auxiliares para validar la ejecución.
   - Priorizar soluciones **portables y escalables**, pensando en mantener y extender el código.
   - En caso de ambigüedad en el enunciado, sugerir alternativas viables con explicación.

5. **Exercise Breakdown**:
   - **Ejercicio 1 (Procesos + SHM):**
     - Crear un proceso coordinador y N procesos generadores.
     - Los generadores piden bloques de 10 IDs al coordinador.
     - Los registros se envían por SHM y el coordinador los guarda en CSV.
     - Monitorear con `ipcs`, `/dev/shm`, `ps`, `htop`, `vmstat`.
   - **Ejercicio 2 (Cliente-Servidor + Transacciones):**
     - Implementar un servidor con soporte de múltiples clientes concurrentes.
     - Los clientes pueden consultar y modificar el CSV mediante un protocolo definido por el grupo.
     - Implementar **BEGIN/COMMIT TRANSACTION** con bloqueo exclusivo.
     - Manejar cierres inesperados de procesos.
     - Monitorear con `netstat`, `lsof`, `htop`.

