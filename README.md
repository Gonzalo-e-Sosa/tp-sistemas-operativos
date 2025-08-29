# TP Sistemas Operativos

## Ejercicio 1: Generador de datos de stock con hilos

El programa `main.c` implementa un generador de datos de prueba para un sistema de stock de productos, cumpliendo con los requisitos del primer ejercicio del TP de Sistemas Operativos.

### Descripción general

- El programa utiliza **hilos (pthread)** para simular procesos generadores de registros de productos.
- Cada hilo solicita bloques de IDs únicos y genera registros con datos aleatorios y consistentes.
- Todos los registros se almacenan en memoria y luego se escriben en un archivo CSV llamado `mock_stock.csv`.

### Estructura de los datos

Cada registro contiene:
- **ID**: Entero autoincremental, único y correlativo.
- **Código**: Alfanumérico secuencial (ej: P001, P002).
- **Nombre**: Seleccionado de una lista predefinida de productos.
- **Lote**: Cadena aleatoria única por producto.
- **Fecha de ingreso**: Fecha aleatoria entre 01-01-2024 y 31-12-2025.
- **Fecha de vencimiento**: Al menos 1 año posterior a la fecha de ingreso.
- **Cantidad**: Entero aleatorio entre 10 y 500.

### Funcionamiento del programa

1. Se definen constantes para la cantidad de hilos y registros por hilo.
2. Cada hilo ejecuta la función `generador`, que:
	 - Solicita un ID único usando un mutex para evitar condiciones de carrera.
	 - Genera los campos del producto de acuerdo a las reglas del enunciado.
3. Al finalizar todos los hilos, el proceso principal escribe todos los registros en el archivo `mock_stock.csv` con el encabezado correspondiente.

### Ejecución

Para compilar y ejecutar:

```sh
make
./main
```

El archivo `mock_stock.csv` se generará en el mismo directorio, listo para ser usado o validado con herramientas como `awk`, `grep` o `cut`.

### Extensión y monitoreo

- Para agregar más variedad de productos, solo hay que ampliar la lista `nombres` en el código.
- Se recomienda monitorear la ejecución y los recursos usando comandos como:
	- `ps aux | grep main`
	- `htop`
	- `vmstat`
	- `lsof | grep mock_stock.csv`

### Notas

- El código es portable y puede adaptarse fácilmente para usar procesos y memoria compartida en lugar de hilos, según la siguiente etapa del TP.