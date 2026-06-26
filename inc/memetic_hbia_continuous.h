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

class HBIAMemeticoContinuous : public MH<double>
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

    // Busqueda local estocastica
    void apply_local_search(Problem<double> &problem, Bird &bird, int &evals, int maxevals, const std::vector<double>& milestones, int& next_milestone_idx, std::vector<double>& error_history, double& global_best_fit)
    {
        int n = problem.getSolutionSize();
        auto domain = problem.getSolutionDomainRange();
        int evals_bl = 0;
        int max_evals_bl = 2000;
        double step = (domain.second - domain.first) * 0.05;

        while (evals < maxevals && evals_bl < max_evals_bl && step > 1e-5)
        {
            bool improved = false;
            for (int j = 0; j < n && evals < maxevals && evals_bl < max_evals_bl; ++j)
            {
                double old_val = bird.sol[j];
                
                bird.sol[j] = old_val + Random::get<double>(-step, step);
                problem.fix(bird.sol);
                double new_fit = problem.fitness(bird.sol);
                evals++; evals_bl++;

                if (new_fit < bird.fitness)
                {
                    bird.fitness = new_fit;
                    improved = true;
                }
                else
                    bird.sol[j] = old_val;

                if (bird.fitness < global_best_fit) global_best_fit = bird.fitness;

                while (next_milestone_idx < milestones.size() && evals >= milestones[next_milestone_idx] * maxevals)
                {
                    error_history.push_back(cec17_error(global_best_fit));
                    next_milestone_idx++;
                }
            }
            if (!improved)
                step *= 0.5; 
        }
    }

public:
    std::vector<double> error_history;

    HBIAMemeticoContinuous() : MH<double>() {}
    virtual ~HBIAMemeticoContinuous() {}

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

        std::vector<double> milestones = {0.01, 0.02, 0.03, 0.05, 0.1, 0.2, 0.3, 0.4, 0.5, 0.6, 0.7, 0.8, 0.9, 1.0};
        int next_milestone_idx = 0;

        for (int i = 0; i < k_birds; ++i)
        {
            banda[i].sol = problem.createSolution();
            problem.fix(banda[i].sol);
            banda[i].fitness = problem.fitness(banda[i].sol);
            banda[i].aggressiveness = 1.0;
            evals++;

            if (banda[i].fitness < best_bird.fitness) best_bird = banda[i];

            while (next_milestone_idx < milestones.size() && evals >= milestones[next_milestone_idx] * maxevals)
            {
                error_history.push_back(cec17_error(best_bird.fitness));
                next_milestone_idx++;
            }
        }

        int estancamiento = 0;
        int theta_estancamiento = std::max(5, (int)(std::log(n) + 1)); 

        while (evals < maxevals)
        {
            bool improved_in_iteration = false;

            for (int i = 0; i < k_birds && evals < maxevals; ++i)
            {
                double delta = 0.0;
                double max_dist = std::sqrt(n * std::pow(domain.second - domain.first, 2));

                for (int j = 0; j < n; ++j)
                    delta += std::pow(banda[i].sol[j] - best_bird.sol[j], 2);

                delta = std::sqrt(delta) / (max_dist + 1e-6);

                tSolution<double> new_sol = banda[i].sol;

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

                if (new_fitness < banda[i].fitness)
                {
                    banda[i].sol = new_sol;
                    banda[i].fitness = new_fitness;
                    banda[i].aggressiveness = 1.0; 
                    
                    if (new_fitness < best_bird.fitness)
                    {
                        best_bird = banda[i];
                        improved_in_iteration = true;
                    }
                }
                else
                    banda[i].aggressiveness = std::min(2.5, banda[i].aggressiveness + 0.15);

                while (next_milestone_idx < milestones.size() && evals >= milestones[next_milestone_idx] * maxevals)
                {
                    error_history.push_back(cec17_error(best_bird.fitness));
                    next_milestone_idx++;
                }
            }

            if (!improved_in_iteration)
                estancamiento++;
            else
                estancamiento = 0;

            // FASE 3: REORGANIZACION Y MEMÉTICA
            if (estancamiento >= theta_estancamiento)
            {
                // Inyectar BL al Líder
                double old_best_fit = best_bird.fitness;
                apply_local_search(problem, best_bird, evals, maxevals, milestones, next_milestone_idx, error_history, best_bird.fitness);
                
                if (best_bird.fitness < old_best_fit) {
                    estancamiento = 0;
                    banda[0] = best_bird; 
                }

                // Reorganización Base del HBIA (Muerte 20%)
                std::sort(banda.begin(), banda.end());

                int num_to_kill = std::max(1, (int)(k_birds * 0.20));
                for (int i = k_birds - num_to_kill; i < k_birds && evals < maxevals; ++i)
                {
                    banda[i].sol = problem.createSolution();
                    problem.fix(banda[i].sol);
                    banda[i].fitness = problem.fitness(banda[i].sol);
                    banda[i].aggressiveness = 1.0;
                    evals++;
                    
                    if (banda[i].fitness < best_bird.fitness)
                        best_bird = banda[i];

                    while (next_milestone_idx < milestones.size() && evals >= milestones[next_milestone_idx] * maxevals)
                    {
                        error_history.push_back(cec17_error(best_bird.fitness));
                        next_milestone_idx++;
                    }
                }
            }
        }

        return ResultMH<double>(best_bird.sol, best_bird.fitness, evals);
    }
};
