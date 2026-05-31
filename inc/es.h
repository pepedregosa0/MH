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

	ResultMH<tDomain> optimize(Problem<tDomain> &problem, int maxevals) override
	{
		int n = problem.getSolutionSize();
		
		// Obtenemos el dominio (0 y k-1) para mutar correctamente
		auto domain = problem.getSolutionDomainRange();
		int min_val = domain.first;
		int max_val = domain.second;
		
		int max_vecinos = 10 * n;
		int max_exitos = 1 * n;
		int M = maxevals / max_vecinos;
		
		// Solucion inicial aleatoria
		tSolution<tDomain> S = problem.createSolution();
		problem.fix(S);
		
		double coste_S = problem.fitness(S);
		int evals = 1;
		
		// Guardamos el mejor histórico para devolverlo al final
		tSolution<tDomain> mejor_historico = S;
		double mejor_coste_historico = coste_S;

		// Inicialización de Temperaturas
		double T0 = (0.2 * coste_S) / (-log(0.3));
		double Tf = 1e-3;
		double T = T0;
		
		// Factor de enfriamiento
		double beta = (T0 - Tf) / (M * T0 * Tf);

		// Variable para controlar si la temperatura anterior tuvo éxitos
		int exitos = 1; 
		
		while (evals < maxevals && exitos > 0)
		{
			exitos = 0;
			int vecinos = 0;
			
			// L(T) a Temperatura constante
			while (vecinos < max_vecinos && exitos < max_exitos && evals < maxevals)
			{
				// Generar vecino único
				tSolution<tDomain> S_vecino = S;
				int gen = Random::get<int>(0, n - 1);
				int valor_actual = S_vecino[gen];
				
				// Le asignamos un clúster estrictamente distinto
				int nuevo_valor = Random::get<int>(min_val, max_val);

				while (nuevo_valor == valor_actual)
					nuevo_valor = Random::get<int>(min_val, max_val);

				S_vecino[gen] = nuevo_valor;
				
				// Reparamos la solución por si hemos dejado un clúster vacío
				problem.fix(S_vecino);
				
				double coste_vecino = problem.fitness(S_vecino);
				evals++;
				vecinos++;
				
				// Diferencia de coste (f(s') - f(s))
				double delta_f = coste_vecino - coste_S;
				
				// CRITERIO DE ACEPTACION
				// Se acepta si mejora (delta_f < 0) o si se cumple la probabilidad exponencial
				if (delta_f < 0 || Random::get(0.0, 1.0) <= exp(-delta_f / T))
				{
					S = S_vecino;
					coste_S = coste_vecino;
					exitos++;
					
					// Actualizar el mejor absoluto encontrado en toda la búsqueda
					if (coste_S < mejor_coste_historico)
					{
						mejor_historico = S;
						mejor_coste_historico = coste_S;
					}
				}
			}
			
			// Enfriar la temperatura
			T = T / (1.0 + beta * T);
		}
		
		// Devolvemos la mejor solución encontrada
		return ResultMH<tDomain>(mejor_historico, mejor_coste_historico, evals);
	}

	// Sobrecarga para ILS-ES
	ResultMH<tDomain> optimize_starting_from(Problem<tDomain> &problem, int maxevals, tSolution<tDomain> sol_inicial)
	{
		int n = problem.getSolutionSize();
		auto domain = problem.getSolutionDomainRange();
		tDomain min_val = domain.first;
		tDomain max_val = domain.second;
		
		int max_vecinos = 10 * n;
		int max_exitos = 1 * n;
		int M = maxevals / max_vecinos;
		
		// Partimos de la solución mutada que nos pasan por parámetro
		tSolution<tDomain> S = sol_inicial;
		
		double coste_S = problem.fitness(S);
		int evals = 1;
		
		tSolution<tDomain> mejor_historico = S;
		double mejor_coste_historico = coste_S;

		// Inicialización de Temperaturas
		double T0 = (0.2 * coste_S) / (-log(0.3));
		double Tf = 0.001;
		double T = T0;
		double beta = (T0 - Tf) / (M * T0 * Tf);
		int exitos = 1; 
		
		while (evals < maxevals && exitos > 0)
		{
			exitos = 0;
			int vecinos = 0;
			
			while (vecinos < max_vecinos && exitos < max_exitos && evals < maxevals)
			{
				// Generar vecino único
				tSolution<tDomain> S_vecino = S;
				int gen = Random::get<int>(0, n - 1);
				tDomain valor_actual = S_vecino[gen];
				
				// Le asignamos un clúster estrictamente distinto
				tDomain nuevo_valor = Random::get<tDomain>(min_val, max_val);

				while (nuevo_valor == valor_actual)
					nuevo_valor = Random::get<tDomain>(min_val, max_val);

				S_vecino[gen] = nuevo_valor;
				
				// Reparamos la solución por si hemos dejado un clúster vacío
				problem.fix(S_vecino);
				
				double coste_vecino = problem.fitness(S_vecino);
				evals++;
				vecinos++;
				
				// Diferencia de coste (f(s') - f(s))
				double delta_f = coste_vecino - coste_S;
				
				// CRITERIO DE ACEPTACION
				// Se acepta si mejora (delta_f < 0) o si se cumple la probabilidad exponencial
				if (delta_f < 0 || Random::get<double>(0.0, 1.0) <= exp(-delta_f / T))
				{
					S = S_vecino;
					coste_S = coste_vecino;
					exitos++;
					
					// Actualizar el mejor absoluto encontrado en toda la búsqueda
					if (coste_S < mejor_coste_historico)
					{
						mejor_historico = S;
						mejor_coste_historico = coste_S;
					}
				}
			}
			// Enfriar la temperatura
			T = T / (1.0 + beta * T);
		}
		return ResultMH<tDomain>(mejor_historico, mejor_coste_historico, evals);
	}
};