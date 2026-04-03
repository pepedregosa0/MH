#include <iostream>
#include <string>
#include <vector>
#include "ppar.h"
#include "random.hpp"
#include "greedy.h"
#include "randomsearch.h"
#include "localsearch.h"

using namespace std;

int main(int argc, char *argv[]) 
{
    // Verificacion de argumentos de entrada
    if (argc < 5) {
        cerr << "Uso: " << argv[0] << " <clusters> <fich_datos> <fich_const> <num_ejecuciones>" << endl;
        return 1;
    }

    // Parseo de parametros
    int n_clusters = stoi(argv[1]);
    string f_datos = argv[2];
    string f_const = argv[3];
    int n_ejecuciones = stoi(argv[4]);
    const long int seed = 42;
	const int num_evaluaciones = 100000;

    // Inicializacion del problema
    Par::ProblemPar problema(n_clusters, f_datos, f_const);
    Problem<int> *problem = dynamic_cast<Problem<int> *>(&problema);

    // Configuracion de algoritmos
    RandomSearch<int> ralg;
    // GreedySearch rgreedy;
    LocalSearch<int> rls;

    vector<pair<string, MH<int> *>> algoritmos = {
        make_pair("RandomSearch", &ralg),
        // make_pair("Greedy", &rgreedy),
        make_pair("LocalSearch", &rls)
    };

    // CABECERA PARA EL CSV (Importante para el script de Python)
    cout << "alg,fitness" << endl;

    // Ejecutamos cada algoritmo 'n_ejecuciones' veces para tener estadistica (boxplot)
    for (int e = 0; e < n_ejecuciones; e++) 
    {
        for (auto const& [nombre, mh] : algoritmos) 
        {
            // Cambiamos la semilla en cada ejecucion para que no salga siempre lo mismo
            // pero que sea reproducible si usamos la misma 
            Random::seed(seed + e); 
            
            ResultMH<int> result = mh->optimize(*problem, num_evaluaciones);
            
            // IMPRESION EN FORMATO CSV
            cout << nombre << "," << result.fitness << endl;
        }
    }

    return 0;
}