---
applyTo: '**/Dockerfile'
---

1. **Project Context**:
   - El proyecto está desarrollado en **C** y requiere ejecutarse sobre un entorno **Linux**.
   - La aplicación incluye procesos con **memoria compartida, semáforos y sockets**.
   - Se debe compilar mediante un **Makefile** y ejecutar el binario resultante dentro del contenedor.
   - El sistema debe permitir ejecutar los **ejercicios prácticos** de la materia Sistemas Operativos.

2. **Dockerfile Guidelines**:
   - Usar una **imagen base ligera de Linux**, preferentemente `debian:bookworm-slim` o `ubuntu:22.04`.
   - Instalar las herramientas necesarias:
     - `build-essential` (para compilar programas en C).
     - `make` (para ejecutar el Makefile).
     - `iproute2`, `procps`, `net-tools`, `htop` (para monitoreo de procesos y sockets).
   - Copiar el código fuente y el `Makefile` al contenedor en `/app`.
   - Establecer el **directorio de trabajo** en `/app`.
   - Ejecutar `make` en la etapa de build.
   - Configurar el contenedor para que ejecute el binario compilado por defecto.
   - Exponer el puerto necesario para el **servidor cliente-servidor** (por ejemplo, `8080`).
   - Asegurarse de que el contenedor libere correctamente recursos al detenerse.

3. **AI Interaction**:
   - Generar un **Dockerfile multistage** si es necesario (para reducir el tamaño de la imagen final).
   - Explicar cada instrucción del Dockerfile con comentarios.
   - Sugerir buenas prácticas como:
     - Minimizar capas (`RUN` con comandos encadenados).
     - Mantener la imagen ligera.
     - Definir variables de entorno si hacen falta (ej. `PORT`, `CSV_FILE`).
   - Proponer un comando de ejecución ejemplo (`docker run ...`) para probar la aplicación.
   - Asegurarse de que el contenedor pueda ejecutarse en segundo plano (`-d`) y aceptar conexiones de clientes externos.

4. **Delivery Guidelines**:
   - Incluir el `Dockerfile` en la raíz del proyecto.
   - Verificar que con:
     ```bash
     docker build -t sistemas-operativos-tp .
     docker run --rm -it -p 8080:8080 sistemas-operativos-tp
     ```
     la aplicación compile y se ejecute correctamente.
   - Documentar en un `README.md` cómo levantar el contenedor y conectarse (clientes, pruebas de procesos, monitoreo).

