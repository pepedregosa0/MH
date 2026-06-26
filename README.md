
--------------------------------------------------------------------------------

# Optimización Metaheurística: Metaheurísticas PARA EL PAR Y CEC 2017

*Nota*: El acrónimo HBIA hace referencia al algoritmo moderno "*Hitchcock Birds Inspired Algorithm*", implementado tanto en sus variantes discretas (para el PAR) como continuas (para el CEC 2017) y sus mejoras.

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
6. **Enfriamiento Simulado (ES):** Metaheurística inspirada en el proceso de enfriamiento de metales, que permite aceptar soluciones peores con una probabilidad decreciente para escapar de óptimos locales.
7. **Búsqueda Multiarranque Básica (BMB):** Estrategia que ejecuta múltiples instancias de una búsqueda local desde soluciones iniciales aleatorias, seleccionando la mejor solución encontrada.
8. **Búsqueda Local Iterativa (ILS):** Técnica que alterna entre fases de búsqueda local y perturbación de la solución para escapar de óptimos locales.
9. **Búsqueda Local Iterativa con Enfriamiento Simulado (ILS-ES):** Hibridación de ILS con ES, donde la fase de búsqueda local se implementa mediante un algoritmo de enfriamiento simulado para mejorar la exploración del espacio de soluciones.
10. Hitchcock Birds Inspired Algorithm (HBIA): Nuevo algoritmo moderno adaptado a espacio discreto (PAR) y optimización continua (CEC 2017).
11. Algoritmo Memético basado en HBIA (MHBIA): Hibridación de las variantes de HBIA con búsqueda local.
12. HBIA Extendido (HBIAExt): Versión mejorada y extendida del algoritmo memético incluyendo una mejora de reinicio catastrófico.

## Estructura del Proyecto
El código sigue una arquitectura fuertemente modular, separando la lógica del dominio del problema de los motores de optimización:

* `src/`: Implementación de la evaluación del fitness del PAR, cálculo de *infeasibility*, reparación de soluciones (`ppar.cpp`), desarrollo de la heurística (`greedy.cpp`), código oficial del benchmark (`cec17.c` / `cec17_test_func.c`).
* `inc/`: Ficheros de cabecera con las definiciones de clases (`ppar.h`, `localsearch.h`, `randomsearch.h`, `genetic.h`, `memetic.h`, `operadores.h`, `es.h`, `bmb.h`, `ils.h`, `ils_es.h`, `hbia_discrete.h`, `hbia_discrete_ext.h`, `memetic_hbia_discrete.h`, `hbia_continuous.h`, `hbia_continuous_ext.h`, `memetic_hbia_continuous.h`).
* `common/`: Framework base de interfaces abstractas y utilidades genéricas de optimización (`mh.h`, `problem.h`, `random.hpp`).
* Directorio Raíz: Contiene el orquestador `main.cpp`, archivos de configuración y *scripts* de automatización, `maincec.cpp` para el CEC2017, requirements.txt para los scripts de python.
* `resultados_boxplot/`:  En este directorio se guardarán los CSV generados por el script 'ejecutar_para_boxplot.sh', tambien contiene los scripts de Python para generar los boxplots e histograma a partir de esos CSV (`boxplot.py`, `histograma.py`).
* Carpetas de salida: Para almacenar historiales del CEC: `results_HBIA/` `results_MHBIA/` `results_HBIAExt/`

## Compilación e Instalación
El proyecto utiliza **CMake** para garantizar una compilación sencilla y sin errores en entornos Linux/Unix.

```bash
# 1. Generar los archivos de construcción
cmake .

# 2. Compilar los ejecutables
make
```
Al compilar desde la raíz se generarán simultáneamente dos ejecutables principales: `main` (para el PAR) y `maincec` (para el benchmark CEC 2017).

## Uso del Programa
### A) PAR (Ejecutable: ./main)
El programa orquestador está completamente parametrizado por línea de comandos, no requiriendo recompilaciones entre ejecuciones.
Sintaxis general:
``` bash
./main -d <datos.dat> -c <restricciones.const> -m <modo> [opciones]
```

Ejemplo de ejecución individual: Ejecutar la base de datos Bupa con un 30% de restricciones, buscando 16 clústeres, usando Greedy con la semilla aleatoria 42:

``` bash
  ./main -d ./datos_PAR_curso2526/bupa_set.dat -c ./datos_PAR_curso2526/bupa_set_const_30.dat -k 16 -n 1 -s 42 -m i -a Greedy
```
``` txt
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
            AM-All, AM-Rand, AM-Best,
            ES, BMB, ILS, ILS-ES,
            HBIA, MHBIA, HBIAExt
  -n <ejecs>      Número de ejecuciones (para modo 'c', por defecto: 10)
  -k <clusters>   Número de clusters (por defecto: 7)
  -e <evals>      Número de evaluaciones (por defecto: 100000)
  -h              Mostrar esta ayuda
```
### B) BENCHMARK CEC 2017 (Ejecutable: ./maincec)
El nuevo ejecutable principal para el entorno continuo acepta la flag `-a` para seleccionar la variante y `-n` para las ejecuciones independientes `-h` para help.

Sintaxis:
``` bash
./maincec -a <algoritmo> [-n <ejecuciones>]
```

Algoritmos disponibles: HBIA, MHBIA, HBIAExt.

Ejemplo manual:
``` bash
./maincec -a MHBIA
```

## Automatización de Experimentos
Para agilizar la obtención de resultados, se proveen scripts en Bash que lanzan pruebas masivas:

- `ejecucion_tablas.sh`: Ejecuta todos los algoritmos sobre las bases de datos al 15% y 30% de restricciones y exporta el código LaTeX directamente a la carpeta resultados_latex/.
- `ejecutar_para_boxplot.sh`: Ejecuta múltiples semillas independientes en formato CSV para su posterior análisis estadístico y visualización de robustez, además de generar automáticamente los boxplots a partir de esos CSV en la carpeta 'graficas_generadas'.
- `ejecutar_cec.sh:`: Script para el CEC 2017. Compila el código, gestiona un entorno virtual de Python, instala dependencias (`requirements.txt`), lanza las 3 variantes continuas en paralelo en segundo plano y extrae los datos estructurados con `extract.py`