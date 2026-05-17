/*
Archivo: memetic.h
Autor: josepc
Descripcion: Implementacion de la clase Memetic, que representan
algoritmos de busqueda memetica.
*/

#pragma once
#include <mh.h>
#include "random.hpp"
#include <vector>
#include <utility>
#include <algorithm> // Para swap

using namespace std;

// Algoritmo de barajado (Fisher-Yates)
void BarajarRSI(vector<int>& rsi) 
{
	for (int i = rsi.size() - 1; i > 0; i--)
	{
		int j = Random::get<int>(0, i); 
		swap(rsi[i], rsi[j]);
	}
}

template <typename tDomain>
void BLS(tSolution<tDomain>& solucion, double& mejor_fitness, Problem<tDomain>& problem,
		 int& evaluaciones, int maxevals_global)
{
	int N = solucion.size();
    // Se utiliza max para garantizar que epsilon sea al menos 1 (por si el 10% de N es menor que 1, lo cual no tendría sentido)
	int epsilon = max(1, (int)(0.1 * N));
	int fallos = 0;
	int i = 0;
	int maxevals_local = 100;

	auto limites = problem.getSolutionDomainRange();
	tDomain min_val = limites.first;
	tDomain max_val = limites.second;
	
	// Generamos vecindario virtual barajado (Fisher-Yates)
	vector<int> RSI(N);
	BarajarRSI(RSI);

	int evals_locales = 0; // Límite de 100 evaluaciones por llamada a la BLS

	while (fallos < epsilon && i < N && evaluaciones < maxevals_global && evals_locales < maxevals_local)
	{
		int p = RSI[i];
		tDomain valor_original = solucion[p];
		bool hay_mejora = false;

		for (tDomain val = min_val; val <= max_val &&
			 evaluaciones < maxevals_global &&
			 evals_locales < maxevals_local; val++) 
		{
			if (val == valor_original) continue;

			solucion[p] = val;
			problem.fix(solucion); 
			double fit = problem.fitness(solucion);
			evaluaciones++;
			evals_locales++;

			if (fit < mejor_fitness)
			{
				mejor_fitness = fit;
				hay_mejora = true;
			}
			else
				solucion[p] = valor_original; // Deshacemos el cambio
		}

		if (!hay_mejora)
			fallos++;
		i++;
	}
	problem.fix(solucion); // Integridad final garantizada
}

template <typename tDomain> class MemeticAlgorithm : public GeneticAlgorithm<tDomain>
{
public:
    MemeticAlgorithm(CrossoverOp<tDomain> cruce) : GeneticAlgorithm<tDomain>(cruce) {}

    // Método virtual para cada variante (All, Rand, Best)
    virtual void aplicar_memetismo(vector<tSolution<tDomain>>& poblacion, 
								   vector<double>& fitness_pob, Problem<tDomain>& problem,
								   int& evaluaciones, int maxevals) = 0;

    virtual ResultMH<tDomain> optimize(Problem<tDomain> &problem, int maxevals) override
	{
        double prob_cruce = 0.8; 
        auto limites = problem.getSolutionDomainRange();
        tDomain min_val = limites.first;
        tDomain max_val = limites.second;
        
        int tam_pob = this->tam_poblacion;
        vector<tSolution<tDomain>> poblacion(tam_pob);
        vector<double> fitness_pob(tam_pob);
        int evaluaciones = 0;
        int mejor_idx_historico = 0;
        
        // Inicialización
        for (int i = 0; i < tam_pob && evaluaciones < maxevals; i++)
		{
            poblacion[i] = problem.createSolution();
            problem.fix(poblacion[i]);
            fitness_pob[i] = problem.fitness(poblacion[i]);
            evaluaciones++;
            if (fitness_pob[i] < fitness_pob[mejor_idx_historico])
                mejor_idx_historico = i;
        }
        
        int generacion = 0; // Contador de generaciones para el memetismo

        // Bucle evolutivo tipo AGG
        while (evaluaciones < maxevals)
		{
            vector<tSolution<tDomain>> nueva_poblacion(tam_pob);
            vector<double> nuevo_fitness(tam_pob);
            
            for (int i = 0; i < tam_pob && evaluaciones < maxevals; i += 2)
			{
                int p1 = SeleccionPorTorneo(fitness_pob, 3);
                int p2 = SeleccionPorTorneo(fitness_pob, 3);
                
                vector<tDomain> h1, h2;
                if (Random::get<double>(0.0, 1.0) < prob_cruce)
                    this->cruce_op(poblacion[p1], poblacion[p2], h1, h2);
				else 
				{
                    h1 = poblacion[p1];
                    h2 = poblacion[p2];
                }
                
                // MUTACIÓN MEMÉTICA: 0.1 / N (por gen en lugar de por cromosoma)
                double prob_mut_gen = 0.1 / h1.size();
                for(size_t g = 0; g < h1.size(); g++)
				{
                    if(Random::get<double>(0.0, 1.0) < prob_mut_gen)
					{
                        tDomain actual = h1[g];
                        tDomain nuevo = Random::get<tDomain>(min_val, max_val);
                        while(nuevo == actual)
							nuevo = Random::get<tDomain>(min_val, max_val);
                        h1[g] = nuevo;
                    }
                }
                for(size_t g = 0; g < h2.size(); g++)
				{
                    if(Random::get<double>(0.0, 1.0) < prob_mut_gen)
					{
                        tDomain actual = h2[g];
                        tDomain nuevo = Random::get<tDomain>(min_val, max_val);
                        while(nuevo == actual)
							nuevo = Random::get<tDomain>(min_val, max_val);
                        h2[g] = nuevo;
                    }
                }
                
                problem.fix(h1);
                problem.fix(h2);
                
                nuevo_fitness[i] = problem.fitness(h1);
                nueva_poblacion[i] = h1;
                evaluaciones++;
                
                if (i + 1 < tam_pob && evaluaciones < maxevals)
				{
                    nuevo_fitness[i+1] = problem.fitness(h2);
                    nueva_poblacion[i+1] = h2;
                    evaluaciones++;
                }
            }
            
            if (evaluaciones >= maxevals) break;
            
            // Elitismo del AGG
            int mejor_idx_nueva = 0, peor_idx_nueva = 0;
            for (int i = 1; i < tam_pob; i++)
			{
                if (nuevo_fitness[i] < nuevo_fitness[mejor_idx_nueva])
					mejor_idx_nueva = i;
                if (nuevo_fitness[i] > nuevo_fitness[peor_idx_nueva])
					peor_idx_nueva = i;
            }
            
            if (fitness_pob[mejor_idx_historico] < nuevo_fitness[mejor_idx_nueva])
			{
                nueva_poblacion[peor_idx_nueva] = poblacion[mejor_idx_historico];
                nuevo_fitness[peor_idx_nueva] = fitness_pob[mejor_idx_historico];
                mejor_idx_nueva = peor_idx_nueva; 
            }
            
            poblacion = nueva_poblacion;
            fitness_pob = nuevo_fitness;
            mejor_idx_historico = mejor_idx_nueva;
            
            generacion++; // Aumentamos la generación

            // Aplicamos el memetismo cada 10 generaciones
            if (generacion % 10 == 0) {
                aplicar_memetismo(poblacion, fitness_pob, problem, evaluaciones, maxevals);
                
                // Tras la Búsqueda Local, buscamos de nuevo al mejor individuo histórico
                mejor_idx_historico = 0;
                for (int i = 1; i < tam_pob; i++)
				{
                    if (fitness_pob[i] < fitness_pob[mejor_idx_historico])
                        mejor_idx_historico = i;
                }
            }
        }
        return ResultMH<tDomain>(poblacion[mejor_idx_historico], fitness_pob[mejor_idx_historico], evaluaciones);
    }
};

/////////////////////////////////////////
/// AM-All: Se aplica a toda la población
/////////////////////////////////////////
template <typename tDomain> class AM_All : public MemeticAlgorithm<tDomain>
{
public:
    AM_All(CrossoverOp<tDomain> cruce) : MemeticAlgorithm<tDomain>(cruce) {}
    virtual void aplicar_memetismo(vector<tSolution<tDomain>>& poblacion, 
								   vector<double>& fitness_pob, Problem<tDomain>& problem, 
								   int& evaluaciones, int maxevals) override
	{
        for (size_t i = 0; i < poblacion.size() && evaluaciones < maxevals; i++)
            BLS(poblacion[i], fitness_pob[i], problem, evaluaciones, maxevals);
    }
};

/////////////////////////////////////////////////////////////
/// AM-Rand: Se aplica aleatoriamente con 10% de probabilidad
/////////////////////////////////////////////////////////////
template <typename tDomain> class AM_Rand : public MemeticAlgorithm<tDomain>
{
public:
    AM_Rand(CrossoverOp<tDomain> cruce) : MemeticAlgorithm<tDomain>(cruce) {}
    virtual void aplicar_memetismo(vector<tSolution<tDomain>>& poblacion,
								   vector<double>& fitness_pob, Problem<tDomain>& problem,
								   int& evaluaciones, int maxevals) override
	{
        for (size_t i = 0; i < poblacion.size() && evaluaciones < maxevals; i++)
		{
            if (Random::get<double>(0.0, 1.0) < 0.1)
                BLS(poblacion[i], fitness_pob[i], problem, evaluaciones, maxevals);
        }
    }
};




/////////////////////////////////////////////////////////////////////////
/// AM-Best: Se aplica solo a los mejores 0.1*N individuos (5 individuos)
/////////////////////////////////////////////////////////////////////////

// Para ordenar índices según fitness
struct ComparadorFitness {
    const vector<double>& fitness; // Referencia al vector original

    // Constructor que inicializa la referencia
    ComparadorFitness(const vector<double>& f) : fitness(f) {}

    // Sobrecarga del operador de comparación que usará el sort
    bool operator()(int a, int b) const
	{
        return fitness[a] < fitness[b];
    }
};

template <typename tDomain> class AM_Best : public MemeticAlgorithm<tDomain>
{
public:
    AM_Best(CrossoverOp<tDomain> cruce) : MemeticAlgorithm<tDomain>(cruce) {}
    virtual void aplicar_memetismo(vector<tSolution<tDomain>>& poblacion,
								   vector<double>& fitness_pob, Problem<tDomain>& problem,
								   int& evaluaciones, int maxevals) override
	{
        int n_best = max(1, (int)(0.1 * poblacion.size())); // Calcular el 10%
        
        // Creamos un vector de índices y lo ordenamos por fitness para saber quiénes son los mejores
        vector<int> indices(poblacion.size());
        for(size_t i = 0; i < poblacion.size(); i++)
			indices[i] = i;
        
        sort(indices.begin(), indices.end(), ComparadorFitness(fitness_pob));
        
        for (int i = 0; i < n_best && evaluaciones < maxevals; i++)
		{
            int idx = indices[i];
            BLS(poblacion[idx], fitness_pob[idx], problem, evaluaciones, maxevals);
        }
    }
};
