/*
Archivo: hbia_discrete_ext.h
Autor: josepc
Descripcion: Implementacion de la clase HBIADiscreteExtended que representa
el algoritmo de HBIA para problemas discretos (PAR), hibridado con Local Search para mejorar la explotacion,
además se ha añadido un reinicio catastrófico tipo CHC.
*/

#pragma once

#include "mh.h"
#include "problem.h"
#include "random.hpp"
#include <vector>
#include <cmath>
#include <algorithm>

// Heredamos de MH<int> para el PAR
class HBIADiscreteExtended : public MH<int>
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

	// Busqueda local suave
	void apply_local_search(Problem<int> &problem, Bird &bird, int &evals, int maxevals)
	{
		int n = problem.getSolutionSize();
		auto domain = problem.getSolutionDomainRange();
		bool improvement = true;

		std::vector<int> indices(n);
		for (int i = 0; i < n; i++)
			indices[i] = i;

		// Le damos un presupuesto limitado a la BL para no agotar las 100.000 totales de golpe
		int evals_bl = 0;
		int max_evals_bl = 2000; 

		while (improvement && evals < maxevals && evals_bl < max_evals_bl)
		{
			improvement = false;

			// Barajado (Fisher-Yates) para exploración sin sesgo
			for (int i = n - 1; i > 0; i--)
			{
				int j = Random::get<int>(0, i);
				std::swap(indices[i], indices[j]);
			}

			for (int i = 0; i < n && !improvement && evals < maxevals && evals_bl < max_evals_bl; i++)
			{
				int gen = indices[i];
				int old_val = bird.sol[gen];

				// Exploramos el vecindario virtual (Primer Mejor)
				for (int val = domain.first; val <= domain.second && !improvement && evals < maxevals; val++)
				{
					if (val == old_val) continue;

					bird.sol[gen] = val;
					problem.fix(bird.sol); // Reparamos si se queda algún clúster vacío
					double new_fit = problem.fitness(bird.sol);
					evals++;
					evals_bl++;

					if (new_fit < bird.fitness) {
						bird.fitness = new_fit;
						improvement = true;
					} else {
						bird.sol[gen] = old_val; // Revertir si no mejora
					}
				}
			}
		}
	}

public:
	HBIADiscreteExtended() : MH<int>() {}
	virtual ~HBIADiscreteExtended() {}

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
					
					if (new_fitness < best_bird.fitness)
					{
						best_bird = banda[i];
						improved_in_iteration = true;
					}
				}
				else
					banda[i].aggressiveness = std::min(2.5, banda[i].aggressiveness + 0.15);
			}

			// FASE 3: REORGANIZATION Y MEMÉTICA Y REINICIO CATASTROFICO
			if (!improved_in_iteration)
				estancamiento++;
			else
				estancamiento = 0;

			if (estancamiento >= theta_estancamiento) {
                
                // Búsqueda Local al Líder
                double old_best_fit = best_bird.fitness;
                apply_local_search(problem, best_bird, evals, maxevals);

                if (best_bird.fitness < old_best_fit)
                {
                    estancamiento = 0;
                    banda[0] = best_bird; // Salvamos al líder
                }

                // Reinicio Catastrófico si el atasco es crítico
                if (estancamiento >= theta_estancamiento * 2) {
                    for (int i = 0; i < k_birds && evals < maxevals; ++i) {
                        // Preservamos solo a los pájaros que tengan el fitness del líder
                        if (banda[i].fitness > best_bird.fitness) {
                            banda[i].sol = problem.createSolution();
                            problem.fix(banda[i].sol);
                            banda[i].fitness = problem.fitness(banda[i].sol);
                            banda[i].aggressiveness = 1.0;
                            evals++;
                        }
                    }
                    estancamiento = 0; // Reseteamos el contador tras el apocalipsis
                } 
                else // muere el 20% peor
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
                }
            }
		}
		
		return ResultMH<int>(best_bird.sol, best_bird.fitness, evals);
	}
};