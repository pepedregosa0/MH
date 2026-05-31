/*
Archivo: es.h
Autor: josepc
Descripcion: Implementacion de la clase EnfriamientoSimulado que representa
el algoritmo de enfriamiento simulado.
*/

#pragma once

#include "mh.h"
#include "problem.h"
#include "random.hpp"
#include <cmath>
#include <vector>

using namespace std;

template <typename tDomain> class EnfriamientoSimulado : public MH<tDomain>
{
public:
	EnfriamientoSimulado() : MH<tDomain>() {}
	virtual ~EnfriamientoSimulado() {}

	// Arranca desde una solución aleatoria
	ResultMH<tDomain> optimize(Problem<tDomain> &problem, int maxevals) override
	{
		tSolution<tDomain> sol_inicial = problem.createSolution();
		problem.fix(sol_inicial);
		// Reutilizamos la lógica llamando a la nueva función
		return optimize_starting_from(problem, maxevals, sol_inicial);
	}

	// Para el ILS-ES, arranca desde la solución mutada que le pasamos por parámetro
	ResultMH<tDomain> optimize_starting_from(Problem<tDomain> &problem, int maxevals, tSolution<tDomain> sol_inicial)
	{
		int n = problem.getSolutionSize();
		auto domain = problem.getSolutionDomainRange();
		tDomain min_val = domain.first;
		tDomain max_val = domain.second;
		
		// Parámetros de iteración
		int max_vecinos = 10 * n;
		int max_exitos = 1 * n;
		int M = maxevals / max_vecinos;
		
		// Solución Inicial
		tSolution<tDomain> S = sol_inicial;
		
		double coste_S = problem.fitness(S);
		int evals = 1; // Evaluamos la inicial
		
		// Guardamos el mejor histórico
		tSolution<tDomain> mejor_historico = S;
		double mejor_coste_historico = coste_S;

		// Inicialización de Temperaturas
		double T0 = (0.2 * coste_S) / (-std::log(0.3));
		double Tf = 1e-3;
		double T = T0;
		
		// Factor de enfriamiento
		double beta = (T0 - Tf) / (M * T0 * Tf);

		int exitos = 1; 
		
		// Preparamos un vector de índices para generar vecinos sin repetición (Fisher-Yatesx)
		std::vector<int> indices(n);
		for(int i = 0; i < n; i++) indices[i] = i;
		
		while (evals < maxevals && exitos > 0) 
		{
			exitos = 0;
			int vecinos = 0;
			
			// Forzamos que se baraje al entrar al bucle de temperatura
			int idx_gen = n; 
			
			// L(T) a Temperatura constante
			while (vecinos < max_vecinos && exitos < max_exitos && evals < maxevals) 
			{
				
				// Si hemos consumido todos los índices, volvemos a barajar (Fisher-Yates)
				if (idx_gen == n)
				{
					for (int j = n - 1; j > 0; j--)
					{
						int k = Random::get<int>(0, j);
						std::swap(indices[j], indices[k]);
					}
					idx_gen = 0; // Reiniciamos el contador
				}
				
				// Obtenemos el gen a mutar siguiendo el orden barajado (sin repetición)
				int gen = indices[idx_gen];
				idx_gen++;
				
				// Generamos un solo vecino modificando el gen seleccionado
				tSolution<tDomain> S_vecino = S;
				tDomain valor_actual = S_vecino[gen];
				
				// Le asignamos un clúster distinto
				tDomain nuevo_valor = Random::get<tDomain>(min_val, max_val);
				while (nuevo_valor == valor_actual)
					nuevo_valor = Random::get<tDomain>(min_val, max_val);

				S_vecino[gen] = nuevo_valor;
				
				// Reparamos para mantener factibilidad
				problem.fix(S_vecino);
				
				double coste_vecino = problem.fitness(S_vecino);
				evals++;
				vecinos++;
				
				// Diferencia de coste para minimizar
				double delta_f = coste_vecino - coste_S;
				double prob_accept = std::exp(-delta_f / T);
				
				// CRITERIO DE ACEPTACION
				if (delta_f < 0 || Random::get<double>(0.0, 1.0) <= prob_accept)
				{
					S = S_vecino;
					coste_S = coste_vecino;
					exitos++;
					
					if (coste_S < mejor_coste_historico) {
						mejor_historico = S;
						mejor_coste_historico = coste_S;
					}
				}
			}
			
			// Enfriamiar temperatura
			T = T / (1.0 + beta * T);
		}
		
		return ResultMH<tDomain>(mejor_historico, mejor_coste_historico, evals);
	}

};