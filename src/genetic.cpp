/*
Archivo: genetic.cpp
Autor: josepc
Descripcion: Implementacion de los algoritmos geneticos.
*/

#include <cassert>
#include <greedy.h>
#include <iostream>
#include <pincrem.h>
#include "ppar.h"
#include "genetic.h"

using namespace std;
using namespace Par;

////////////////////////////////////////////////////////
/// Implementacion Algoritmo Genético Generacional (AGG)
////////////////////////////////////////////////////////
template <typename tDomain>
ResultMH<tDomain> AGG<tDomain>::optimize(Problem<tDomain> &problem, int maxevals) {
	double prob_cruce = 0.8;

	auto limites = problem.getSolutionDomainRange();
	tDomain min_val = limites.first;
	tDomain max_val = limites.second;

	vector<tSolution<tDomain>> poblacion(tam_poblacion);
	vector<double> fitness_pob(tam_poblacion);
	int evaluaciones = 0;
	
	int mejor_idx_historico = 0;
	
	// inicialización de la población
	for (int i = 0; i < tam_poblacion && evaluaciones < maxevals; i++)
	{
		poblacion[i] = problem.createSolution();
		problem.fix(poblacion[i]);
		fitness_pob[i] = problem.fitness(poblacion[i]);
		evaluaciones++;
		
		if (fitness_pob[i] < fitness_pob[mejor_idx_historico])
			mejor_idx_historico = i;
	}
	
	// Bucle evolutivo generacional
	while (evaluaciones < maxevals) {
		vector<tSolution<tDomain>> nueva_poblacion(tam_poblacion);
		vector<double> nuevo_fitness(tam_poblacion);
		
		for (int i = 0; i < tam_poblacion && evaluaciones < maxevals; i += 2) 
		{
			int p1 = SeleccionPorTorneo(fitness_pob, 3);
			int p2 = SeleccionPorTorneo(fitness_pob, 3);
			
			vector<tDomain> h1, h2;
			
			// cruce o copia
			if (Random::get<double>(0.0, 1.0) < prob_cruce) {
				cruce_op(poblacion[p1], poblacion[p2], h1, h2); 
			} else {
				h1 = poblacion[p1];
				h2 = poblacion[p2];
			}
			
			// mutacion
			MutacionUniforme(h1, prob_mutacion, max_val + 1);
			MutacionUniforme(h2, prob_mutacion, max_val + 1);
			
			// Evitar soluciones no válidas
			problem.fix(h1);
			problem.fix(h2);
			
			// Evaluación 1
			nuevo_fitness[i] = problem.fitness(h1);
			nueva_poblacion[i] = h1;
			evaluaciones++;
			
			// Evaluacion 2 (Comprobando límite de evaluaciones)
			if (i + 1 < tam_poblacion && evaluaciones < maxevals) {
				nuevo_fitness[i+1] = problem.fitness(h2);
				nueva_poblacion[i+1] = h2;
				evaluaciones++;
			}
		}
		
		// Elitismo: Comprobamos si el mejor histórico es mejor que el nuevo mejor, si no, lo copiamos
		if (evaluaciones >= maxevals) break; // Seguridad si cortamos por límite de evals
		
		int mejor_idx_nueva = 0;
		int peor_idx_nueva = 0;
		
		for (int i = 1; i < tam_poblacion; i++)
		{
			if (nuevo_fitness[i] < nuevo_fitness[mejor_idx_nueva]) mejor_idx_nueva = i;
			if (nuevo_fitness[i] > nuevo_fitness[peor_idx_nueva]) peor_idx_nueva = i;
		}
		
		// Si el mejor padre es mejor que el mejor hijo, el padre pisa al peor hijo
		if (fitness_pob[mejor_idx_historico] < nuevo_fitness[mejor_idx_nueva])
		{
			nueva_poblacion[peor_idx_nueva] = poblacion[mejor_idx_historico];
			nuevo_fitness[peor_idx_nueva] = fitness_pob[mejor_idx_historico];
			mejor_idx_nueva = peor_idx_nueva; 
		}
		
		// Reemplazo generacional
		poblacion = nueva_poblacion;
		fitness_pob = nuevo_fitness;
		mejor_idx_historico = mejor_idx_nueva;
	}
	
	return ResultMH<tDomain>(poblacion[mejor_idx_historico], fitness_pob[mejor_idx_historico], evaluaciones);
}


////////////////////////////////////////////////////////
/// Implementacion Algoritmo Genético Estacionario (AGE)
////////////////////////////////////////////////////////
template <typename tDomain>
ResultMH<tDomain> AGE<tDomain>::optimize(Problem<tDomain> &problem, int maxevals) {
	double prob_cruce = 1.0; // Siempre cruzamos en AGE

	auto limites = problem.getSolutionDomainRange();
	tDomain min_val = limites.first;
	tDomain max_val = limites.second;
	
	vector<tSolution<tDomain>> poblacion(tam_poblacion);
	vector<double> fitness_pob(tam_poblacion);
	int evaluaciones = 0;
	
	for (int i = 0; i < tam_poblacion && evaluaciones < maxevals; i++)
	{
		poblacion[i] = problem.createSolution();
		problem.fix(poblacion[i]);
		fitness_pob[i] = problem.fitness(poblacion[i]);
		evaluaciones++;
	}
	
	// Bucle evolutivo estacionario
	while (evaluaciones < maxevals) {
		int p1 = SeleccionPorTorneo(fitness_pob, 3);
		int p2 = SeleccionPorTorneo(fitness_pob, 3);
		
		vector<tDomain> h1, h2;
		
		// Cruce o copia
		if (Random::get<double>(0.0, 1.0) < prob_cruce)
			cruce_op(poblacion[p1], poblacion[p2], h1, h2);
		else
		{
			h1 = poblacion[p1];
			h2 = poblacion[p2];
		}
		
		// Mutacion
		MutacionUniforme(h1, prob_mutacion, max_val + 1);
		MutacionUniforme(h2, prob_mutacion, max_val + 1);
		
		// Evitar soluciones no válidas
		problem.fix(h1);
		problem.fix(h2);
		
		// Evaluación 1
		double fit1 = problem.fitness(h1);
		evaluaciones++;
		
		// Evaluación 2 (Comprobando límite de evaluaciones)
		double fit2 = 0.0;
		if (evaluaciones < maxevals)
		{
			fit2 = problem.fitness(h2);
			evaluaciones++;
		}
		else
			break;
		
		// Buscamos a los 2 peores individuos de toda la población para reemplazarlos
		int peor1 = 0, peor2 = 1;
		if (fitness_pob[1] > fitness_pob[0])
		{
			peor1 = 1;
			peor2 = 0;
		}
		
		for (int i = 2; i < tam_poblacion; i++)
		{
			if (fitness_pob[i] > fitness_pob[peor1])
			{
				peor2 = peor1;
				peor1 = i;
			}
			else if (fitness_pob[i] > fitness_pob[peor2])
				peor2 = i;
		}
		
		// El mejor de los hijos compite contra el peor absoluto (peor1).
		// El peor de los hijos compite contra el segundo peor (peor2).
		if (fit1 < fit2)
		{ 
			// El Hijo 1 es el mejor de los dos
			if (fit1 < fitness_pob[peor1])
			{
				poblacion[peor1] = h1;
				fitness_pob[peor1] = fit1;
			}
			if (fit2 < fitness_pob[peor2])
			{
				poblacion[peor2] = h2;
				fitness_pob[peor2] = fit2;
			}
		}
		else
		{ 
			// El Hijo 2 es el mejor de los dos (o son iguales)
			if (fit2 < fitness_pob[peor1])
			{
				poblacion[peor1] = h2;
				fitness_pob[peor1] = fit2;
			}
			if (fit1 < fitness_pob[peor2])
			{
				poblacion[peor2] = h1;
				fitness_pob[peor2] = fit1;
			}
		}
	}
	// Al final del bucle, buscamos el mejor absoluto de la población para devolverlo
	int best_idx = 0;
	for (int i = 1; i < tam_poblacion; i++)
	{
		if (fitness_pob[i] < fitness_pob[best_idx])
			best_idx = i;
	}
	
	return ResultMH<tDomain>(poblacion[best_idx], fitness_pob[best_idx], evaluaciones);
}

// Instanciación explícita
template class AGG<int>;
template class AGE<int>;
