#!/bin/bash

DIR_DATOS="../datos_PAR_curso2526/"
DIR_RESULTADOS="./resultados_boxplot/"
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

mkdir -p ${DIR_RESULTADOS}

# Dataset Zoo
echo "Procesando Zoo..."
./main -k 7 -d "${DIR_DATOS}zoo_set.dat" -c "${DIR_DATOS}zoo_set_const_15.dat" -n $N_EJECUCIONES -s $SEMILLA -m c > ${DIR_RESULTADOS}resultados_zoo_15.csv
./main -k 7 -d "${DIR_DATOS}zoo_set.dat" -c "${DIR_DATOS}zoo_set_const_30.dat" -n $N_EJECUCIONES -s $SEMILLA -m c > ${DIR_RESULTADOS}resultados_zoo_30.csv

# Dataset Glass
echo "Procesando Glass..."
./main -k 7 -d "${DIR_DATOS}glass_set.dat" -c "${DIR_DATOS}glass_set_const_15.dat" -n $N_EJECUCIONES -s $SEMILLA -m c > ${DIR_RESULTADOS}resultados_glass_15.csv
./main -k 7 -d "${DIR_DATOS}glass_set.dat" -c "${DIR_DATOS}glass_set_const_30.dat" -n $N_EJECUCIONES -s $SEMILLA -m c > ${DIR_RESULTADOS}resultados_glass_30.csv

# Dataset Bupa
echo "Procesando Bupa..."
./main -k 16 -d "${DIR_DATOS}bupa_set.dat" -c "${DIR_DATOS}bupa_set_const_15.dat" -n $N_EJECUCIONES -s $SEMILLA -m c > ${DIR_RESULTADOS}resultados_bupa_15.csv
./main -k 16 -d "${DIR_DATOS}bupa_set.dat" -c "${DIR_DATOS}bupa_set_const_30.dat" -n $N_EJECUCIONES -s $SEMILLA -m c > ${DIR_RESULTADOS}resultados_bupa_30.csv