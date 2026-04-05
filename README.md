
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
2. **Algoritmo Greedy (COPKM):** Enfoque constructivo determinista basado en una modificación del algoritmo *K-Medias* [4, 6]. Selecciona iterativamente la asignación que menor incremento de infactibilidad produce.
3. **Búsqueda Local (BL):** Metaheurística basada en trayectorias que utiliza la estrategia del **Primer Mejor** (*First Improvement*). Emplea un vecindario dinámico generado mediante el cambio individual de clúster y un operador de reparación inteligente para la solución inicial.

## Estructura del Proyecto
El código sigue una arquitectura fuertemente modular, separando la lógica del dominio del problema de los motores de optimización:

* `src/`: Implementación de la evaluación del fitness del PAR, cálculo de *infeasibility*, reparación de soluciones (`ppar.cpp`) y desarrollo de la heurística (`greedy.cpp`).
* `inc/`: Ficheros de cabecera con las definiciones de clases (`ppar.h`, `localsearch.h`, `randomsearch.h`).
* `common/`: Framework base de interfaces abstractas y utilidades genéricas de optimización (`mh.h`, `problem.h`, `random.hpp`).
* Directorio Raíz: Contiene el orquestador `main.cpp`, archivos de configuración y *scripts* de automatización.

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

Ejemplo de ejecución individual: Ejecutar la base de datos Zoo con un 30% de restricciones, buscando 7 clústeres, usando la Búsqueda Local con la semilla aleatoria 42:

``` bash
./main -d zoo_set.dat -c zoo_set_const_30.dat -a LocalSearch -k 7 -s 42 -m i
```

Parámetros y Opciones:

-m <modo>: Tipo de volcado de datos: i (individual), t (tabla LaTeX), c (formato CSV).

-a <algoritmo>: RandomSearch, Greedy o LocalSearch.

-e <evals>: Límite del criterio de parada (por defecto: 100.000).

-h: muestra toda la ayuda y opciones disponibles.

## Automatización de Experimentos
Para agilizar la obtención de resultados sobre los distintos datasets (Zoo, Glass, Bupa), se proveen scripts en Bash que lanzan pruebas masivas:

- `ejecucion_tablas.sh`: Ejecuta todos los algoritmos sobre las bases de datos al 15% y 30% de restricciones y exporta el código LaTeX directamente a la carpeta resultados_latex/.
- `ejecutar_para_boxplot.sh`: Ejecuta múltiples semillas independientes en formato CSV para su posterior análisis estadístico y visualización de robustez.

