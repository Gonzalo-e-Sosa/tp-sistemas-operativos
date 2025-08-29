---
applyTo: '**'
---

1. **Project Context**:
   - El primer ejercicio consiste en desarrollar un sistema en C que genere datos de prueba mediante procesos y memoria compartida.
   - Los datos producidos se almacenarán en un archivo **CSV**.
   - En este caso, los registros representarán un **stock de productos**.

2. **Mock Data Definition**:
   Cada registro en el CSV debe tener los siguientes campos (en este orden):
   - **ID** (entero autoincremental, generado por el proceso coordinador).
   - **Código de producto** (alfanumérico corto, por ejemplo `P001`, `P002`).
   - **Nombre** (cadena con el nombre del producto, ej: "Paracetamol 500mg").
   - **Lote** (alfanumérico, ej: `L1234`, `L5678`).
   - **Fecha de ingreso** (formato `DD-MM-YYYY`).
   - **Fecha de vencimiento** (formato `DD-MM-YYYY`, siempre mayor a la fecha de ingreso).
   - **Cantidad** (entero positivo, ej: `50`, `120`).

   Ejemplo de encabezado CSV: ID,Codigo,Descripcion,Lote,FechaIngreso,FechaVencimiento,Cantidad

   Ejemplo de registro: 1,P001,Paracetamol 500mg,L1234,2025-08-20,2027-08-20,100

   
3. **Mock Data Generation Rules**:
- Los **códigos** se deben generar de forma secuencial (`P001`, `P002`, ...).
- El **nombre** debe seleccionarse de una lista predefinida de productos (ej: "Ibuprofeno 400mg", "Alcohol en gel 250ml", "Jeringa 5ml").
- Los **lotes** deben ser aleatorios pero únicos por producto.
- La **fecha de ingreso** debe generarse en un rango (ej: entre `01-01-2024` y `31-12-2025`).
- La **fecha de vencimiento** debe ser al menos 1 año posterior a la fecha de ingreso.
- La **cantidad** debe generarse como un entero aleatorio en un rango configurable (ej: `10` a `500`).
- El **ID** siempre debe ser correlativo y único, comenzando en `1`.

4. **AI Interaction**:
- Generar automáticamente un lote de registros de prueba (ej: 50 o 100 registros).
- Garantizar que:
  - Los IDs sean únicos y correlativos.
  - No existan lotes repetidos para el mismo producto.
  - Las fechas tengan sentido lógico (ingreso < vencimiento).
- Producir un archivo **CSV válido y listo para ser usado** en el ejercicio.
- Explicar cómo extender la lista de productos para generar más variedad.

5. **Delivery Guidelines**:
- Guardar el archivo generado como `mock_stock.csv`.
- Entregarlo junto con el código fuente y el Makefile.
- Asegurarse de que el CSV pueda ser validado fácilmente con scripts (`awk`, `grep`, `cut`).
