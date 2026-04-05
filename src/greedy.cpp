/*
Archivo: greedy.cpp
Autor: josepc
Descripcion: Implementacion de la clase GreedySearch, que representa un 
algoritmo constructivo para el problema
*/

#include <cassert>
#include <greedy.h>
#include <iostream>
#include <pincrem.h>
#include "ppar.h"

using namespace std;
using namespace Par;

template <class T> void print_vector(string name, const vector<T> &sol) {
  cout << name << ": ";

  for (auto elem : sol) {
    cout << elem << ", ";
  }
  cout << endl;
}

std::vector<int> GreedySearch::GenerarRSI(int n) 
{
    std::vector<int> RSI(n);
    for (int i = 0; i < n; i++)
        RSI[i] = i;
    
    // Barajado de Fisher-Yates
    for (int i = n - 1; i > 0; i--)
	{
        int j = Random::get<int>(0, i);
        std::swap(RSI[i], RSI[j]);
    }
    return RSI;
}

ResultMHInt GreedySearch::optimize(ProblemInt &problem, int maxevals) 
{
	assert(maxevals > 0);
	// esto solo se puede usar en greedy de verdad que suspendes te la juegas he, te lo advierto
	Par::ProblemPar &realproblem = dynamic_cast<Par::ProblemPar &>(problem);
	
	size_t size = problem.getSolutionSize();
	size_t num_clusters = realproblem.getSolutionDomainRange().second + 1;
	// Solucion sin asignar
	tSolution<int> sol(size, -1);

	// Generar centroides aleatorios
	std::vector<std::vector<double>> centroides = realproblem.GenerarCentroidesAleatorios();

	// Generar RSI
	std::vector<int> RSI = GenerarRSI(size);

	// Bucle principal greedy
	bool hay_cambios = true;
	do {
		hay_cambios = false;
		
		for (int i : RSI) 
		{
			int mejor_cluster = -1;
			int menor_infeasibility = std::numeric_limits<int>::max();
			double menor_distancia = std::numeric_limits<double>::max();

			for (int c = 0; c < num_clusters; c++)
			{
				int infeasibility_actual = realproblem.IncrementoInfeasibility(i, c, sol);
				double distancia_actual = Par::Distancia(realproblem.getInstancias()[i], centroides[c]);

				if (infeasibility_actual < menor_infeasibility || 
					(infeasibility_actual == menor_infeasibility && distancia_actual < menor_distancia)) 
				{
					mejor_cluster = c;
					menor_infeasibility = infeasibility_actual;
					menor_distancia = distancia_actual;
				}
			}

			if (sol[i] != mejor_cluster) 
			{
				sol[i] = mejor_cluster;
				hay_cambios = true;
			}
		}

		// Recalcular centroides después de asignar todas las instancias
		centroides = realproblem.CalcularCentroides(sol);

	} while (hay_cambios);

	return ResultMHInt(sol, problem.fitness(sol), 1);
}


