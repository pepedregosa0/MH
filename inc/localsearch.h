#pragma once
#include <mh.h>
#include "random.hpp"
#include <vector>
#include <utility>
#include <algorithm> // Para swap

using namespace std;

template <typename tDomain> class LocalSearch : public MH<tDomain> 
{
private:
    vector<pair<int, tDomain>> GenerarVecindarioVirtual(const tSolution<tDomain>& solucion, Problem<tDomain>& problem) 
    {
        vector<pair<int, tDomain>> vecindario;
        int n = problem.getSolutionSize();
		// Obtenemos los limites (ej: 0 a k-1) para generar los vecinos
        auto rango = problem.getSolutionDomainRange(); 

        for (int i = 0; i < n; i++) 
		{
            tDomain cluster_actual = solucion[i];
            for (tDomain c = rango.first; c <= rango.second; c++) 
			{
                if (c != cluster_actual)
                    vecindario.push_back(make_pair(i, c));
            }
        }
        return vecindario;
    }

    // Algoritmo de barajado (Fisher-Yates) para el vecindario
    void BarajarVecindario(vector<pair<int, tDomain>>& vecindario) 
    {
        for (int i = vecindario.size() - 1; i > 0; i--)
		{
            int j = Random::get<int>(0, i); 
            swap(vecindario[i], vecindario[j]);
        }
    }

public:
    LocalSearch() : MH<tDomain>() {}
    virtual ~LocalSearch() {}

    ResultMH<tDomain> optimize(Problem<tDomain> &problem, int maxevals) override 
    {
        assert(maxevals > 0);
        
        // Generar solucion inicial
        tSolution<tDomain> sol_actual = problem.createSolution();
        tFitness fitness_actual = problem.fitness(sol_actual);
        
        int evaluaciones = 1; // Ya hemos evaluado la solucion inicial
        bool hay_mejora = true;

        while (evaluaciones < maxevals && hay_mejora) 
        {
            hay_mejora = false;

            auto vecindario = GenerarVecindarioVirtual(sol_actual, problem);
            BarajarVecindario(vecindario);

            // Bucle de exploracion "Primer Mejor"
            for (size_t v = 0; v < vecindario.size(); v++) 
            {
                if (evaluaciones >= maxevals) break;

                int indice = vecindario[v].first;
                tDomain nuevo_valor = vecindario[v].second;
                tDomain valor_antiguo = sol_actual[indice];

                // Aplicar movimiento
                sol_actual[indice] = nuevo_valor;

                // Comprobar que el vecino es valido (no deja clusters vacios)
                if (!problem.isValid(sol_actual)) 
                {
                    sol_actual[indice] = valor_antiguo; // Deshacer y saltar
                    continue;
                }

                // Evaluar vecino
                tFitness fitness_vecino = problem.fitness(sol_actual);
                evaluaciones++;

                if (fitness_vecino < fitness_actual) {
                    fitness_actual = fitness_vecino;
                    hay_mejora = true;
                    break; // Al ser primer mejor, cortamos y empezamos de nuevo
                } 
                else
                    sol_actual[indice] = valor_antiguo; // Deshacer
            }
        }

        return ResultMH<tDomain>(sol_actual, fitness_actual, evaluaciones);
    }
};