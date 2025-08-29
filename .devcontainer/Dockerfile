# Dockerfile para Sistemas Operativos - TP

# Imagen base ligera de Linux
FROM debian:bookworm-slim

# Instala herramientas necesarias para compilar, monitorear y desarrollar
RUN apt-get update && \
    apt-get install -y --no-install-recommends \
        build-essential \
        make \
        iproute2 \
        procps \
        net-tools \
        htop && \
    rm -rf /var/lib/apt/lists/*

# Copia el código fuente y el Makefile al contenedor
WORKDIR /app
COPY .. /app

# Compila el proyecto
RUN make || true

# Expone el puerto para el servidor (ajustar si es necesario)
EXPOSE 8080

# Variables de entorno opcionales
ENV PORT=8080
ENV CSV_FILE=data.csv

# Ejecuta el binario principal (ajustar nombre si es necesario)
CMD ["./main"]

# Notas:
# - El binario principal debe llamarse 'main' o ajustar el CMD.
# - El contenedor libera recursos al detenerse por diseño de Linux.
# - Para ejecutar:
#   docker build -t sistemas-operativos-tp .
#   docker run --rm -it -p 8080:8080 sistemas-operativos-tp
# - Para ejecución en segundo plano:
#   docker run -d -p 8080:8080 sistemas-operativos-tp
