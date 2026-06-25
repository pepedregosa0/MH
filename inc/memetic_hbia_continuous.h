/*
Archivo: memetic_hbia_continuous.h
Autor: josepc
Descripcion: Implementacion de la clase HBIADiscrete que representa
el algoritmo de HBIA para problemas continuos (CEC2017) hibridado con Local Search para mejorar la explotacion.
*/

#pragma once

#include "mh.h"
#include "problem.h"
#include "random.hpp"
#include <vector>
#include <cmath>
#include <algorithm>

extern "C" {
	#include "cec17.h"
}

class HBIAContinuous : public MH<double>
{
private:
	struct Bird {
		tSolution<double> sol;
		double fitness;
		double aggressiveness;

		// Para la ordenación de la banda
		bool operator<(const Bird& otro) const
		{
			return fitness < otro.fitness;
		}
	};

public:
	// Vector público para recuperar los 14 hitos al final de la ejecución
	std::vector<double> error_history;

	HBIAContinuous() : MH<double>() {}
	virtual ~HBIAContinuous() {}

	ResultMH<double> optimize(Problem<double> &problem, int maxevals) override
	{
		int k_birds = 30; 
		int n = problem.getSolutionSize();
		auto domain = problem.getSolutionDomainRange();

		std::vector<Bird> banda(k_birds);
		Bird best_bird;
		best_bird.fitness = std::numeric_limits<double>::max();

		int evals = 0;
		error_history.clear();

		// 14 hitos obligatorios del CEC 2017
		std::vector<double> milestones = {0.01, 0.02, 0.03, 0.05, 0.1, 0.2, 0.3, 0.4, 0.5, 0.6, 0.7, 0.8, 0.9, 1.0};
		int next_milestone_idx = 0;

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

			// Capturar hitos
			while (next_milestone_idx < milestones.size() && evals >= milestones[next_milestone_idx] * maxevals)
			{
				error_history.push_back(cec17_error(best_bird.fitness));
				next_milestone_idx++;
			}
		}

		int estancamiento = 0;
		int theta_estancamiento = std::max(5, (int)(std::log(n) + 1)); 

		// BUCLE PRINCIPAL
		while (evals < maxevals)
		{
			bool improved_in_iteration = false;

			for (int i = 0; i < k_birds && evals < maxevals; ++i)
			{
				double delta = 0.0;
				// DISTANCIA CONTINUA (Distancia Euclidiana)
				double max_dist = std::sqrt(n * std::pow(domain.second - domain.first, 2));

				for (int j = 0; j < n; ++j)
					delta += std::pow(banda[i].sol[j] - best_bird.sol[j], 2);

				delta = std::sqrt(delta) / (max_dist + 1e-6);

				tSolution<double> new_sol = banda[i].sol;

				// FASE 2: Ataque
				if (delta < 0.5)
				{
					double rho = Random::get<double>(0.0, 1.0) * banda[i].aggressiveness;
					for (int j = 0; j < n; ++j)
						new_sol[j] = new_sol[j] + rho * (best_bird.sol[j] - new_sol[j]);
				}
				else
				{
					double rand_val = Random::get<double>(0.0, 1.0);
					for (int j = 0; j < n; ++j)
						new_sol[j] = new_sol[j] + rand_val * std::cos(rand_val * M_PI) * (best_bird.sol[j] - new_sol[j]);
				}

				problem.fix(new_sol);
				double new_fitness = problem.fitness(new_sol);
				evals++;

				// Actualizar agresividad
				if (new_fitness < banda[i].fitness) {
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

				// Capturar hitos
				while (next_milestone_idx < milestones.size() && evals >= milestones[next_milestone_idx] * maxevals)
				{
					error_history.push_back(cec17_error(best_bird.fitness));
					next_milestone_idx++;
				}
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
					
					// Hay un pajaro mejor que el actual
					if (banda[i].fitness < best_bird.fitness)
						best_bird = banda[i];

					// Capturar hitos de la reorganización
					while (next_milestone_idx < milestones.size() && evals >= milestones[next_milestone_idx] * maxevals)
					{
						error_history.push_back(cec17_error(best_bird.fitness));
						next_milestone_idx++;
					}
				}
				estancamiento = 0; 
			}
		}

		return ResultMH<double>(best_bird.sol, best_bird.fitness, evals);
	}
};