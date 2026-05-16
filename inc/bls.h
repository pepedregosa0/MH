/*
Archivo: bls.h
Autor: josepc
Descripcion: Implementacion del algoritmo de busqueda local suave.
*/
#pragma once
#include <vector>
#include <problem.h>
#include "random.hpp"

using namespace std;

// Algoritmo de barajado (Fisher-Yates)
void BarajarRSI(std::vector<int>& rsi) 
{
    for (int i = rsi.size() - 1; i > 0; i--)
    {
        int j = Random::get<int>(0, i); 
        std::swap(rsi[i], rsi[j]);
    }
}

/// Busqueda Local Suave (BLS) para una solución dada, con

void BLS(tSolution<int> &sol_actual, tFitness &fitness_actual, int &evals_globales, Problem<int> &problem, int k) 
{
    int N = sol_actual.size();
    int max_evals_bls = 100; // Tope estricto de 100 evaluaciones por llamada
    int epsilon = max(1, (int)(0.1 * N)); // Límite de fallos permitidos
    
    // barajamos los índices
    vector<int> RSI(N);
    
	// Rellenamos el RSI con los índices de las posiciones
	for (int i = 0; i < N; i++)
		RSI[i] = i;

    BarajarRSI(RSI); // Barajamos el RSI para el orden aleatorio de exploración	

    int fallos = 0;
    int i = 0;
    int evals_locales = 0;

    // Bucle principal de la BLS
    while (fallos < epsilon && i < N && evals_locales < max_evals_bls) 
    {
        int p = RSI[i];
        int valor_original_posicion = sol_actual[p]; // Guardamos inicio
        
        // Bucle de exploración para una sola posición probando todos los clústeres
        for (int val = 0; val < k && evals_locales < max_evals_bls; val++) 
        {
            if (val == valor_original_posicion) continue; // No gastamos evaluación si es el mismo

            int valor_antiguo = sol_actual[p]; // Valor justo antes de este movimiento

            // Aplicar movimiento
            sol_actual[p] = val;

            // Comprobar que el vecino es valido (Tu código original)
            if (!problem.isValid(sol_actual)) 
            {
                sol_actual[p] = valor_antiguo; // Deshacer y saltar
                continue;
            }

            // Evaluar vecino
            tFitness fitness_vecino = problem.fitness(sol_actual);
            evals_locales++;
            evals_globales++; // Actualizamos el contador global del Memético

            // Criterio de aceptación
            if (fitness_vecino < fitness_actual)
                fitness_actual = fitness_vecino;
            else
                sol_actual[p] = valor_antiguo; // Deshacer
        }

        // Si después de probar todos los clústeres, la instancia sigue 
        // en el mismo clúster que estaba al principio, cuenta como fallo
        if (sol_actual[p] == valor_original_posicion)
            fallos++;

        i++;
    }
}