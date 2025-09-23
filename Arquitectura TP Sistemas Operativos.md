Arquitectura TP Sistemas Operativos

# Servidor concurrente: Hilos vs. `fork()` para manejar conexiones

## 1️⃣ Arquitectura con hilos (KLT o ULT)

    ┌───────────────────────────────┐
    │       Proceso Servidor         │
    │  (socket de escucha principal) │
    └───────────────┬────────────────┘
                    │
              accept() nueva conexión
                    │
    ┌───────────────┴────────────────┐
    │  Crea hilo para manejar cliente │
    └───────────────┬────────────────┘
                    │
            ┌───────┴────────┐
            │   Hilo 1       │  <─── Cliente 1 (proceso aparte)
            │ (usa socket)   │
            └────────────────┘
            ┌───────┴────────┐
            │   Hilo 2       │  <─── Cliente 2 (proceso aparte)
            │ (usa socket)   │
            └────────────────┘
            ┌───────┴────────┐
            │   Hilo N       │  <─── Cliente N (proceso aparte)
            │ (usa socket)   │
            └────────────────┘


- Todos los hilos comparten memoria y recursos del proceso servidor.
- El socket es el canal IPC entre cada cliente y el hilo que lo atiende.

---

## 2️⃣ Arquitectura con `fork()` (procesos pesados)

    ┌───────────────────────────────┐
    │       Proceso Servidor         │
    │  (socket de escucha principal) │
    └───────────────┬────────────────┘
                    │
              accept() nueva conexión
                    │
    ┌───────────────┴────────────────┐
    │  fork() para manejar cliente    │
    └───────────────┬────────────────┘
                    │
            ┌───────┴────────┐
            │ Proceso hijo 1 │  <─── Cliente 1 (proceso aparte)
            │ (usa socket)   │
            └────────────────┘
            ┌───────┴────────┐
            │ Proceso hijo 2 │  <─── Cliente 2 (proceso aparte)
            │ (usa socket)   │
            └────────────────┘
            ┌───────┴────────┐
            │ Proceso hijo N │  <─── Cliente N (proceso aparte)
            │ (usa socket)   │
            └────────────────┘

- Cada conexión se maneja en un proceso independiente.
- El socket aceptado se hereda por el hijo.
- Comunicación con el servidor requiere IPC explícito.


---

## 3️⃣ Arquitectura híbrida (fork + hilos)

    ┌───────────────────────────────┐
    │       Proceso Servidor         │
    │  (socket de escucha principal) │
    └───────────────┬────────────────┘
                    │
              Pre-fork de procesos hijos
                    │
    ┌───────────────┴────────────────┐
    │   Proceso hijo 1               │
    │  (pool de hilos)                │
    └───────┬───────────────┬────────┘
            │               │
      ┌─────┴─────┐   ┌─────┴─────┐
      │  Hilo 1   │   │  Hilo 2   │  ... atienden clientes
      └───────────┘   └───────────┘

    ┌───────────────┴────────────────┐
    │   Proceso hijo 2               │
    │  (pool de hilos)                │
    └───────┬───────────────┬────────┘
            │               │
      ┌─────┴─────┐   ┌─────┴─────┐
      │  Hilo 1   │   │  Hilo 2   │  ... atienden clientes
      └───────────┘   └───────────┘

- El servidor principal crea varios procesos hijos (aislamiento).
- Cada hijo mantiene un pool de hilos para atender múltiples clientes.
- Combina aislamiento de procesos con eficiencia de hilos

---

## 4️⃣ Comparativa: Hilos vs. `fork()` para manejar conexiones

| Aspecto | **Hilos** | **Procesos con `fork()`** |
|---------|-----------|---------------------------|
| **Espacio de memoria** | Compartido entre todos los hilos → comunicación interna muy rápida. | Separado para cada proceso hijo → comunicación con el servidor requiere IPC explícito. |
| **Coste de creación** | Muy bajo (solo pila y estructuras mínimas). | Alto: duplicación del espacio de direcciones (aunque con *copy-on-write* se optimiza). |
| **Cambio de contexto** | Más rápido: cambio entre hilos del mismo proceso. | Más lento: cambio entre procesos implica cambio de espacio de direcciones. |
| **Aislamiento** | Bajo: un fallo en un hilo puede tumbar todo el servidor. | Alto: un fallo en un hijo no afecta directamente al servidor (salvo recursos compartidos como sockets). |
| **Comunicación interna** | Directa en memoria compartida (requiere sincronización). | Necesita IPC (pipes, colas, memoria compartida explícita, sockets). |
| **Escalabilidad** | Muy buena para muchas conexiones concurrentes. | Menos eficiente para muchas conexiones: más consumo de memoria y CPU. |
| **Multiprocesador** | Hilos pueden ejecutarse en núcleos distintos. | Procesos también pueden ejecutarse en núcleos distintos, pero con más coste de coordinación. |

## 4️⃣ Comparativa General

| Aspecto | **Hilos** | **`fork()`** | **Híbrido (`fork()` + hilos)** |
|---------|-----------|--------------|--------------------------------|
| **Espacio de memoria** | Compartido entre hilos. | Separado por proceso. | Separado por grupo de hilos (por proceso hijo). |
| **Coste de creación** | Muy bajo. | Alto (aunque optimizado con COW). | Medio: pocos procesos, varios hilos por proceso. |
| **Cambio de contexto** | Rápido entre hilos. | Más lento entre procesos. | Rápido entre hilos de un mismo hijo; más lento entre hijos. |
| **Aislamiento** | Bajo. | Alto. | Medio-alto: aislamiento entre grupos de hilos. |
| **Comunicación interna** | Directa en memoria compartida. | IPC obligatorio. | Directa entre hilos del mismo hijo; IPC entre hijos. |
| **Escalabilidad** | Muy alta. | Menor para muchas conexiones. | Alta y más robusta que solo hilos. |
| **Robustez ante fallos** | Baja: un fallo puede tumbar todo. | Alta: fallo de un hijo no afecta al resto. | Media-alta: fallo de un hijo no afecta a otros, pero sí a sus hilos. |

---

## 📌 Conclusiones prácticas

- **Modelo con hilos**:
  - Ideal para **muchas conexiones concurrentes** con intercambio frecuente de datos entre manejadores.
  - Más eficiente en uso de memoria y CPU.
  - Requiere mucho cuidado con la sincronización para evitar *race conditions*.

- **Modelo con `fork()`**:
  - Útil cuando se quiere **aislar completamente** cada conexión (por seguridad o estabilidad).
  - Más robusto ante fallos de un manejador, pero más costoso en recursos.
  - Comunicación con el proceso principal más compleja (IPC obligatorio).

---

## 📌 Conclusiones

- **Hilos**: máxima eficiencia y velocidad, pero menos aislamiento.
- **`fork()`**: máximo aislamiento, pero más coste y complejidad en comunicación.
- **Híbrido**: equilibrio entre rendimiento y aislamiento; usado en servidores como Apache en modo *worker*.

**Recomendación**:  
- Alto rendimiento y muchas conexiones → **Hilos** o **Híbrido**.  
- Seguridad y aislamiento prioritarios → **`fork()`** o **Híbrido**.

---

**Recomendación general**:  
En servidores de alto rendimiento (como web servers o chat servers), se suele usar **hilos** o **pools de hilos**.  
El modelo con `fork()` se usa más en entornos donde la **seguridad y el aislamiento** son prioritarios (por ejemplo, servidores que ejecutan código no confiable de clientes).