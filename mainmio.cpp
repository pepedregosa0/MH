#include <iostream>
#include <problem.h>
#include <random.hpp>
#include <string>
#include <util.h>
// Real problem class
#include "pincrem.h"

#include "ppar.h"

// All all algorithms
#include "greedy.h"
#include "randomsearch.h"
#include "random.hpp"
#include "localsearch.h"

using namespace std;
int main(int argc, char *argv[]) 
{
	long int seed = 42; // Semilla por defecto

    // Configuracion de algoritmos
    //RandomSearch<int> ralg;
    // GreedySearch rgreedy;
    LocalSearch<int> rls;

    vector<pair<string, MH<int> *>> algoritmos = {
        // make_pair("RandomSearch", &ralg),
        // make_pair("Greedy", &rgreedy),
        make_pair("LocalSearch", &rls)
    };

	// Cargar datos
	Par::ProblemPar problema(7, "../datos_PAR_curso2526/zoo_set.dat", "../datos_PAR_curso2526/zoo_set_const_30.dat");
	Problem<int> *problem = dynamic_cast<Problem<int> *>(&problema);

	for (int i = 0; i < algoritmos.size(); i++) 
	{
		Random::seed(seed); // Reiniciar semilla para cada algoritmo para que sea justa la comparacion [7]
		cout << "Ejecutando " << algoritmos[i].first << "..." << endl;
		
		auto mh = algoritmos[i].second;
		
		// Solo es para el random
		int num_evaluaciones = 100000;
		
		ResultMH<int> result = mh->optimize(*problem, num_evaluaciones);
		
		cout << "Best solution: " << result.solution << endl;
		cout << "Best fitness: " << result.fitness << endl;
		cout << "Evaluations: " << result.evaluations << endl;
	}


	return 0;
}
