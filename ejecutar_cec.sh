#!/bin/bash

# Salir inmediatamente si algún comando falla
set -e

echo "=== 1. Compilando el proyecto con CMake ==="
cmake .
make

echo -e "\n=== 2. Creando directorios de resultados ==="
mkdir -p results_HBIA results_MHBIA results_HBIAExt

VENV_DIR="venv"

if [ ! -d "$VENV_DIR" ]; then
    echo "El entorno virtual '$VENV_DIR' no existe. Creándolo..."
    python3 -m venv "$VENV_DIR"
fi

echo "Activando el entorno virtual..."
source "$VENV_DIR/bin/activate"

echo -e "\n=== 3. Instalando requisitos de Python ==="
if [ -f "requirements.txt" ]; then
    python3 -m pip install -r requirements.txt
else
    echo "Aviso: No se encontró requirements.txt, saltando paso."
fi

echo -e "\n=== 4. Lanzando ejecuciones en paralelo ==="
# Se lanzan en segundo plano usando '&'
./maincec -a HBIA &
PID1=$!

./maincec -a MHBIA &
PID2=$!

./maincec -a HBIAExt &
PID3=$!

echo "Ejecuciones en segundo plano iniciadas (PIDs: $PID1, $PID2, $PID3). Esperando a que terminen..."
# 'wait' detiene el script hasta que los tres procesos en segundo plano finalicen
wait $PID1 $PID2 $PID3

echo -e "\n=== 5. Ejecutando script de extracción ==="
if [ -f "extract.py" ]; then
    python3 extract.py
else
    echo "Error: No se encontró extract.py"
    exit 1
fi

echo -e "\n=== Proceso completado con éxito ==="