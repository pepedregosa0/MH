/*
Archivo: bmb.h
Autor: josepc
Descripcion: Implementacion de la clase BMB que representa
el algoritmo de Búsqueda por Múltiples Arranques.
*/

#pragma once

#include "mh.h"
#include "problem.h"
#include "localsearch.h" // Incluimos tu BL de la Práctica 1
#include <limits>

using namespace std;

template <typename tDomain>
class BMB : public MH<tDomain> {
public:
	BMB() : MH<tDomain>() {}
	virtual ~BMB() {}
	
	ResultMH<tDomain> optimize(Problem<tDomain> &problem, int maxevals) override 
	{
		int num_iteraciones = 5;
		int maxevals_bl = maxevals / num_iteraciones; // 100000 / 5 = 20000
		
		tSolution<tDomain> mejor_global;
		double mejor_coste_global = numeric_limits<double>::max();
		unsigned int total_evals = 0;

		// Instanciamos BL de la practica 1
		LocalSearch<tDomain> buscador_local;

		for (int i = 0; i < num_iteraciones; i++) 
		{
			// Ejecutamos la Búsqueda Local
			ResultMH<tDomain> res_bl = buscador_local.optimize(problem, maxevals_bl);
			
			total_evals += res_bl.evaluations;

			// Actualizamos la mejor solución histórica encontrada en los arranques
			if (res_bl.fitness < mejor_coste_global || i == 0) 
			{
				mejor_coste_global = res_bl.fitness;
				mejor_global = res_bl.solution;
			}
		}

		return ResultMH<tDomain>(mejor_global, mejor_coste_global, total_evals);
	}
};