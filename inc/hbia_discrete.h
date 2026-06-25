/*
Archivo: hbia_discrete.h
Autor: josepc
Descripcion: Implementacion de la clase HBIADiscrete que representa
el algoritmo de HBIA para problemas discretos (PAR).
*/

#pragma once

#include "mh.h"
#include "problem.h"
#include "random.hpp"
#include <vector>
#include <cmath>
#include <algorithm>

// Heredamos de MH<int> para el PAR
class HBIADiscrete : public MH<int>
{
private:
	struct Bird {
		tSolution<int> sol;
		double fitness;
		double aggressiveness;

		// Para la ordenación de la banda
		bool operator<(const Bird& otro) const
		{
			return fitness < otro.fitness;
		}
	};

public:
	HBIADiscrete() : MH<int>() {}
	virtual ~HBIADiscrete() {}

	ResultMH<int> optimize(Problem<int> &problem, int maxevals) override
	{
		int k_birds = 30; 
		int n = problem.getSolutionSize();
		auto domain = problem.getSolutionDomainRange();

		std::vector<Bird> banda(k_birds);
		Bird best_bird;
		best_bird.fitness = std::numeric_limits<double>::max();

		int evals = 0;

		// FASE 1: Acecho (Inicialización)
		for (int i = 0; i < k_birds; ++i)
		{
			banda[i].sol = problem.createSolution();
			problem.fix(banda[i].sol);
			banda[i].fitness = problem.fitness(banda[i].sol);
			banda[i].aggressiveness = 1.0;
			evals++;

			if (banda[i].fitness < best_bird.fitness)
				best_bird = banda[i];
		}

		int estancamiento = 0;
		int theta_estancamiento = std::max(5, (int)(std::log(n) + 1)); 

		// BUCLE PRINCIPAL
		while (evals < maxevals)
		{
			bool improved_in_iteration = false;

			for (int i = 0; i < k_birds && evals < maxevals; ++i)
			{
				// DISTANCIA DISCRETA (Porcentaje de clústeres distintos)
				double delta = 0.0;
				for (int j = 0; j < n; ++j)
				{
					if (banda[i].sol[j] != best_bird.sol[j]) 
						delta += 1.0;
				}
				delta /= n; 

				tSolution<int> new_sol = banda[i].sol;

				// FASE 2: Ataque
				if (delta < 0.5)
				{
					// Ataque rapido: Probabilidad de copiar el clúster del líder
					double rho = Random::get<double>(0.0, 1.0) * banda[i].aggressiveness;
					for (int j = 0; j < n; ++j)
					{
						if (Random::get<double>(0.0, 1.0) < std::min(rho, 1.0))
							new_sol[j] = best_bird.sol[j];
					}
				}
				else
				{
					// Ataque tardio: Mutación aleatoria en base a la agresividad
					for (int j = 0; j < n; ++j)
					{
						if (Random::get<double>(0.0, 1.0) < banda[i].aggressiveness * 0.3)
							new_sol[j] = Random::get<int>(domain.first, domain.second);
					}
				}

				problem.fix(new_sol);
				double new_fitness = problem.fitness(new_sol);
				evals++;

				// Actualizar agresividad
				if (new_fitness < banda[i].fitness)
				{
					banda[i].sol = new_sol;
					banda[i].fitness = new_fitness;
					banda[i].aggressiveness = 1.0; // Se relaja al acertar
					
                    // Hay un pajaro mejor que el actual
					if (new_fitness < best_bird.fitness)
					{
						best_bird = banda[i];
						improved_in_iteration = true;
					}
				}
				else
					banda[i].aggressiveness = std::min(2.5, banda[i].aggressiveness + 0.15);
			}

			// FASE 3: Reorganizacion
			if (!improved_in_iteration)
				estancamiento++;
			else
				estancamiento = 0;

			if (estancamiento >= theta_estancamiento)
			{
				std::sort(banda.begin(), banda.end());

				int num_to_kill = std::max(1, (int)(k_birds * 0.20));
				for (int i = k_birds - num_to_kill; i < k_birds && evals < maxevals; ++i)
				{
					banda[i].sol = problem.createSolution();
					problem.fix(banda[i].sol);
					banda[i].fitness = problem.fitness(banda[i].sol);
					banda[i].aggressiveness = 1.0;
					evals++;
				}
				estancamiento = 0; 
			}
		}

		return ResultMH<int>(best_bird.sol, best_bird.fitness, evals);
	}
};