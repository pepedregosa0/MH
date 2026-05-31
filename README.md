
--------------------------------------------------------------------------------

# Optimización Metaheurística: Problema de Agrupamiento con Restricciones (PAR)

Este repositorio contiene el desarrollo e implementación en **C++** de diferentes algoritmos metaheurísticos aplicados a la resolución del **Problema de Agrupamiento con Restricciones (PAR)**. 

Este proyecto fue desarrollado como parte de la asignatura de *Metaheurísticas* del Grado en Ingeniería Informática de la Universidad de Granada (UGR).

## Descripción del Problema
El PAR es una generalización del clásico problema de análisis de clústeres (*clustering*). El objetivo es encontrar una partición que agrupe un conjunto de instancias en *k* clústeres, minimizando la distancia media intra-clúster mientras se cumple un conjunto de restricciones de usuario.

Las restricciones a nivel de instancia son:
* **Must-Link (ML):** Dos instancias dadas deben pertenecer forzosamente al mismo clúster.
* **Cannot-Link (CL):** Dos instancias dadas no pueden pertenecer al mismo clúster.

Dado que encontrar una partición que satisfaga el 100% de las restricciones puede ser geométricamente imposible en altas densidades, el problema se modela utilizando **restricciones débiles** (*soft constraints*). Se optimiza una única función de *fitness* penalizada: `Fitness = Desviacion_General + (λ * Incumplimientos)`.

## Algoritmos Implementados
El framework incluye el diseño y adaptación de tres enfoques de resolución:

1. **Búsqueda Aleatoria (Random Search):** Algoritmo de exploración estocástica ciega que genera soluciones aleatorias factibles, utilizado como *baseline* comparativo.
2. **Algoritmo Greedy (COPKM):** Enfoque constructivo determinista basado en una modificación del algoritmo *K-Medias*. Selecciona iterativamente la asignación que menor incremento de infactibilidad produce.
3. **Búsqueda Local (BL):** Metaheurística basada en trayectorias que utiliza la estrategia del **Primer Mejor** (*First Improvement*). Emplea un vecindario dinámico generado mediante el cambio individual de clúster y un operador de reparación inteligente para la solución inicial.
4. **Algoritmos Genéticos (AGG y AGE):** Implementación de un algoritmo genético generacional (AGG) y un algoritmo genético estacionario (AGE) con operadores de selección por torneo, cruce uniforme y mutación adaptativa.
5. **Algoritmos Meméticos (AM):** Extensión de los algoritmos genéticos con la incorporación de una fase de búsqueda local intensiva sobre los mejores individuos de cada generación.
Variantes de cada algoritmo:
- AGG-UN: AGG con selección por torneo, cruce uniforme y mutación
- AGG-SF: AGG con selección por torneo, cruce de un punto y mutación
- AGE-UN: AGE con selección por torneo, cruce uniforme y mutación
- AGE-SF: AGE con selección por torneo, cruce de un punto y mutación
- AM-All: Memético con búsqueda local sobre toda la población
- AM-Rand: Memético con búsqueda local sobre individuos seleccionados aleatoriamente
- AM-Best: Memético con búsqueda local solo sobre el mejor individuo de cada generación

## Estructura del Proyecto
El código sigue una arquitectura fuertemente modular, separando la lógica del dominio del problema de los motores de optimización:

* `src/`: Implementación de la evaluación del fitness del PAR, cálculo de *infeasibility*, reparación de soluciones (`ppar.cpp`) y desarrollo de la heurística (`greedy.cpp`).
* `inc/`: Ficheros de cabecera con las definiciones de clases (`ppar.h`, `localsearch.h`, `randomsearch.h`, `genetic.h`, `memetic.h`, `operadores.h`).
* `common/`: Framework base de interfaces abstractas y utilidades genéricas de optimización (`mh.h`, `problem.h`, `random.hpp`).
* Directorio Raíz: Contiene el orquestador `main.cpp`, archivos de configuración y *scripts* de automatización.
* `resultados_boxplot/`:  En este directorio se guardarán los CSV generados por el script 'ejecutar_para_boxplot.sh', tambien contiene los scripts de Python para generar los boxplots e histograma a partir de esos CSV (`boxplot.py`, `histograma.py`).

## Compilación e Instalación
El proyecto utiliza **CMake** para garantizar una compilación sencilla y sin errores en entornos Linux/Unix.

```bash
# 1. Generar los archivos de construcción
cmake .

# 2. Compilar el ejecutable
make
```

## Uso del Programa
El programa orquestador está completamente parametrizado por línea de comandos, no requiriendo recompilaciones entre ejecuciones.
Sintaxis general:
``` bash
./main -d <datos.dat> -c <restricciones.const> -m <modo> [opciones]
```

Ejemplo de ejecución individual: Ejecutar la base de datos Bupa con un 30% de restricciones, buscando 16 clústeres, usando Greedy con la semilla aleatoria 42:

``` bash
  ./main -d ./datos_PAR_curso2526/bupa_set.dat -c ./datos_PAR_curso2526/bupa_set_const_30.dat -k 16 -n 1 -s 42 -m i -a Greedy

```

Parámetros y Opciones:
Argumentos obligatorios:
  -d <archivo>    Ruta al archivo de datos (.dat)
  -c <archivo>    Ruta al archivo de restricciones (_const.dat)
  -m <modo>       Modo de ejecución: 'i' (individual), 't' (tabla), 'c' (csv)

Opciones extra:
  -s <semilla>    Semilla aleatoria (por defecto: 42)
  -a <algoritmo>  Nombre del algoritmo (obligatorio en modo 'i')
    Algoritmos disponibles:
            Random, Greedy, BL,
            AGG-UN, AGG-SF, AGE-UN, AGE-SF,
            AM-All, AM-Rand, AM-Best
  -n <ejecs>      Número de ejecuciones (para modo 'c', por defecto: 10)
  -k <clusters>   Número de clusters (por defecto: 7)
  -e <evals>      Número de evaluaciones (por defecto: 100000)
  -h              Mostrar esta ayuda


## Automatización de Experimentos
Para agilizar la obtención de resultados sobre los distintos datasets (Zoo, Glass, Bupa), se proveen scripts en Bash que lanzan pruebas masivas:

- `ejecucion_tablas.sh`: Ejecuta todos los algoritmos sobre las bases de datos al 15% y 30% de restricciones y exporta el código LaTeX directamente a la carpeta resultados_latex/.
- `ejecutar_para_boxplot.sh`: Ejecuta múltiples semillas independientes en formato CSV para su posterior análisis estadístico y visualización de robustez, además de generar automáticamente los boxplots a partir de esos CSV en la carpeta 'graficas_generadas'.

