##
# Proyecto final Sistemas Operativos
#
# @file
# @version 0.1

# Variable del compilador a usar
CC = gcc
# Variable de las banderas a usar, `-g` es para agregar símbolos de depuración
CFLAGS = -g

# Al ejecutar `make` en la carpeta del proyecto se llaman a las instrucciones
# `clean`, `build` y `run` antes.
default: clean build run

# Al ejecutar `make build` en la carpeta del proyecto se compila el archivo
# que contiene el código fuente.
build:
	${CC} ${CFLAGS} RoundRobinAlgorithm_con_Semaforo.c -o proyecto.o

# Al ejecutar `make run` en la carpeta del proyecto se verifica que se haya
# ejecutado `build` para crear el binario (ejecutable), luego este se ejecuta.
run: build
	./proyecto.o

# Al ejecutar `make clean` en la carpeta del proyecto se borra el binario.
clean:
	rm ./proyecto.o

# end
