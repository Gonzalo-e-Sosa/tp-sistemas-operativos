# Devcontainer para Sistemas Operativos TP

Este entorno permite desarrollar y depurar los ejercicios en VS Code usando Docker.

## ¿Qué incluye?
- Compilador C, make y herramientas de monitoreo (`htop`, `ipcs`, `netstat`, etc).
- Forward del puerto 8080 para pruebas cliente-servidor.
- Permisos extra para depuración de procesos.

## Uso rápido
1. Abrir la carpeta en VS Code.
2. Instalar la extensión "Dev Containers".
3. Seleccionar "Reopen in Container".
4. Compilar y ejecutar normalmente dentro del contenedor.

Se puede monitorear procesos y recursos desde la terminal del contenedor usando comandos como:

```sh
ps aux
htop
ipcs -m
vmstat
netstat -tuln
lsof -i
```
