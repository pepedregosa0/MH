/*
Archivo: ils_es.h
Autor: josepc
Descripcion: Implementacion de la clase ILS_ES que representa
el algoritmo de Búsqueda Local Iterativa hibridado con Enfriamiento Simulado.
*/

#pragma once

#include "mh.h"
#include "problem.h"
#include "es.h"
#include "random.hpp"
#include <vector>
#include <algorithm> // Para swap

using namespace std;

template <typename tDomain>
class ILS_ES : public MH<tDomain> {
private:
	double porcentaje_mutacion = 0.20; // 20% de mutación

	// Mutación Fuerte
	tSolution<tDomain> MutacionFuerte(const tSolution<tDomain>& sol_actual, Problem<tDomain>& problem)
	{
		int n = problem.getSolutionSize();
		int num_mutaciones = max(1, (int)(n * porcentaje_mutacion)); // Al menos mutar 1 gen
		
		auto domain = problem.getSolutionDomainRange();
		tDomain min_val = domain.first;
		tDomain max_val = domain.second;

		tSolution<tDomain> sol_mutada = sol_actual;
		
		// Barajamos un array de índices para elegir qué genes mutar sin repetir
		vector<int> indices(n);
		for(int i = 0; i < n; i++)
			indices[i] = i;
		
		for(int i = n - 1; i > 0; i--)
		{
			int r = Random::get<int>(0, i);
			swap(indices[i], indices[r]);
		}
		
		// Cambiamos el clúster a los 'num_mutaciones' primeros índices barajados
		for(int m = 0; m < num_mutaciones; m++)
		{
			int gen = indices[m];
			tDomain valor_actual = sol_mutada[gen];
			
			tDomain nuevo_valor = Random::get<tDomain>(min_val, max_val);
			while(nuevo_valor == valor_actual)
				nuevo_valor = Random::get<tDomain>(min_val, max_val);

			sol_mutada[gen] = nuevo_valor;
		}
		
		// Es vital reparar la solución dentro de la propia función
		problem.fix(sol_mutada);
		
		return sol_mutada;
	}

public:
	ILS_ES() : MH<tDomain>() {}
	virtual ~ILS_ES() {}

	ResultMH<tDomain> optimize(Problem<tDomain> &problem, int maxevals) override
	{
		int iteraciones = 5; 
		int maxevals_interno = maxevals / iteraciones; 
		
		// Usamos ES como optimizador interno en vez de LS
		EnfriamientoSimulado<tDomain> enfriamiento;
		
		// Solución Inicial Aleatoria y primer Enfriamiento
		tSolution<tDomain> sol_inicial = problem.createSolution();
		problem.fix(sol_inicial);
		
		ResultMH<tDomain> mejor_historico = enfriamiento.optimize_starting_from(problem, maxevals_interno, sol_inicial);
		int total_evals = mejor_historico.evaluations;

		// Bucle de ILS (4 iteraciones restantes)
		for (int i = 1; i < iteraciones; i++)
		{
			// Mutamos la mejor solución histórica encontrada hasta ahora
			tSolution<tDomain> sol_mutada = MutacionFuerte(mejor_historico.solution, problem);
			
			// Aplicamos ES
			ResultMH<tDomain> res_es = enfriamiento.optimize_starting_from(problem, maxevals_interno, sol_mutada);
			total_evals += res_es.evaluations;
			
			// CRITERIO DE ACEPTACION: Solo aceptamos si mejora el mejor histórico
			if (res_es.fitness < mejor_historico.fitness)
				mejor_historico = res_es;
		}
		
		mejor_historico.evaluations = total_evals;
		return mejor_historico;
	}
};