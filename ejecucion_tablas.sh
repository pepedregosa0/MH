#!/bin/bash
# Script para obtener las tablas para la documentación, modo -m t tabla

DIR_DATOS="../datos_PAR_curso2526/"
SEMILLA=42
N_EJECUCIONES=10

# Compilar si es necesario
echo "Compilando..."
cmake .
make

# Comprobamos si 'make' terminó con éxito código 0. Si falla, abortamos.
if [ $? -ne 0 ]; then
    echo "Error en la compilación. Abortando las ejecuciones."
    exit 1
fi

echo "Compilación exitosa. Iniciando las 6 ejecuciones..."
echo "----------------------------------------------------"

echo "=========================================================="
echo " Ejecutando experimentos para GLASS k=7 "
echo "=========================================================="
# Dataset Glass 15 restricciones
./main -k 7 -d "${DIR_DATOS}glass_set.dat" -c "${DIR_DATOS}glass_set_const_15.dat" -n $N_EJECUCIONES -s $SEMILLA -m t
# Dataset Glass 30 restricciones
./main -k 7 -d "${DIR_DATOS}glass_set.dat" -c "${DIR_DATOS}glass_set_const_30.dat" -n $N_EJECUCIONES -s $SEMILLA -m t

echo "=========================================================="
echo " Ejecutando experimentos para ZOO k=7 "
echo "=========================================================="
# Dataset Zoo 15 restricciones
./main -k 7 -d "${DIR_DATOS}zoo_set.dat" -c "${DIR_DATOS}zoo_set_const_15.dat" -n $N_EJECUCIONES -s $SEMILLA -m t
# Dataset Zoo 30 restricciones
./main -k 7 -d "${DIR_DATOS}zoo_set.dat" -c "${DIR_DATOS}zoo_set_const_30.dat" -n $N_EJECUCIONES -s $SEMILLA -m t

echo "=========================================================="
echo " Ejecutando experimentos para BUPA k=16 "
echo "=========================================================="
# Dataset Bupa 15 restricciones
./main -k 16 -d "${DIR_DATOS}bupa_set.dat" -c "${DIR_DATOS}bupa_set_const_15.dat" -n $N_EJECUCIONES -s $SEMILLA -m t
# Dataset Bupa 30 restricciones
./main -k 16 -d "${DIR_DATOS}bupa_set.dat" -c "${DIR_DATOS}bupa_set_const_30.dat" -n $N_EJECUCIONES -s $SEMILLA -m t
echo "----------------------------------------------------"