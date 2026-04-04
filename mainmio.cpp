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

	if (argc > 1)
		seed = std::stol(argv[1]);

    // Configuracion de algoritmos
    RandomSearch<int> ralg;
    GreedySearch rgreedy;
    LocalSearch<int> rls;

    vector<pair<string, MH<int> *>> algoritmos = {
        make_pair("RandomSearch", &ralg),
        make_pair("Greedy", &rgreedy),
        make_pair("LocalSearch", &rls)
    };

	// Cargar datos
	string f_datos = "../datos_PAR_curso2526/zoo_set.dat";
	string f_const = "../datos_PAR_curso2526/zoo_set_const_30.dat";
	Par::ProblemPar problema(7, f_datos, f_const);
	Problem<int> *problem = dynamic_cast<Problem<int> *>(&problema);

	for (int i = 0; i < algoritmos.size(); i++) 
	{
		Random::seed(seed); // Reiniciar semilla para cada algoritmo para que sea justa la comparacion [7]
		cout << "Ejecutando " << algoritmos[i].first << "..." << endl;
		
		auto mh = algoritmos[i].second;
		
		int num_evaluaciones = 1;
		// Solo es para el random
		if (algoritmos[i].first == "RandomSearch")
			num_evaluaciones = 100000;
		
		ResultMH<int> result = mh->optimize(*problem, num_evaluaciones);
		
		cout << "Best solution: " << result.solution << endl;
		cout << "Best fitness: " << result.fitness << endl;
		cout << "Evaluations: " << result.evaluations << endl;
	}


	return 0;
}
