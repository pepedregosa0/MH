/*
Archivo: main.cpp
Autor: josepc
Descripcion: Main para ejecutar los algoritmos de MH y obtener tablas de resultados
*/

#include <iostream>
#include <string>
#include <map>
#include <unistd.h> // Para getopt

// Para medicion de tiempos
#include <chrono>

// Para inicializar la semilla
#include "random.hpp"

// Clase del problema
#include "ppar.h"

// Todos los algoritmos
#include "greedy.h"
#include "randomsearch.h"
#include "localsearch.h"

using namespace std;
template <typename S>
ostream &operator<<(ostream &os, const vector<S> &vector) 
{
  // Printing all the elements using <<
  for (auto i : vector)
    os << i << " ";
  return os;
}


void print_use()
{
	cout << "Uso: ./main -d <dataset> -c <restricciones> -m <modo> [opciones]\n\n";
    cout << "Argumentos obligatorios:\n";
    cout << "  -d <archivo>    Ruta al archivo de datos (.dat)\n";
    cout << "  -c <archivo>    Ruta al archivo de restricciones (_const.dat)\n";
    cout << "  -m <modo>       Modo de ejecución: 'i' (individual), 't' (tabla), 'c' (csv)\n\n";
    cout << "Opciones extra:\n";
    cout << "  -s <semilla>    Semilla aleatoria (por defecto: 42)\n";
    cout << "  -a <algoritmo>  Nombre del algoritmo (obligatorio en modo 'i')\n";
	cout << "                  Algoritmos disponibles: Random, Greedy, BL\n";
    cout << "  -n <ejecs>      Número de ejecuciones (para modo 'c', por defecto: 10)\n";
	cout << "  -k <clusters>   Número de clusters (por defecto: 7)\n";
	cout << "  -e <evals>      Número de evaluaciones (por defecto: 100000)\n";
	cout << "  -h              Mostrar esta ayuda\n";
	cout << "\nEjemplo de uso:\n";
	cout << "  ./main -d zoo_set.dat -c zoo_set_const_30.dat -m c -n 20\n";
	cout << "  (Ejecuta los algoritmos en modo csv para generar una tabla con 20 ejecuciones cada uno)\n";
}

int main(int argc, char *argv[]) 
{
	int opt;
	string f_datos = "", f_const = "", algoritmo = "";
	char modo = 'x';
	int semilla = 42;
	int n_ejecuciones = 10;
	int n_clusters = 7;
	int num_evaluaciones = 100000;

	while ((opt = getopt(argc, argv, "d:c:m:s:a:n:k:e:h")) != -1)
	{
		switch (opt) 
		{
			case 'd': f_datos = optarg; break;
            case 'c': f_const = optarg; break;
            case 'm': modo = optarg[0]; break;
            case 's': semilla = stoi(optarg); break;
            case 'a': algoritmo = optarg; break;
            case 'n': n_ejecuciones = stoi(optarg); break;
            case 'k': n_clusters = stoi(optarg); break;
            case 'e': num_evaluaciones = stoi(optarg); break;
            case 'h': default: print_use(); return 1; // sin args o -h muestra ayuda
		}
	}

	if (f_datos.empty() || f_const.empty() || modo == 'x')
	{
		cerr << "Error: Faltan argumentos obligatorios.\n\n";
		print_use();
		return 1;
	}

    // Inicializacion del problema
    Par::ProblemPar problema(n_clusters, f_datos, f_const);
    Problem<int> *problem = dynamic_cast<Problem<int> *>(&problema);
	Random::seed(semilla);

    // Configuracion de algoritmos
    RandomSearch<int> ralg;
    GreedySearch rgreedy;
    LocalSearch<int> rls;

    map<string, MH<int>*> algoritmos = {
        make_pair("Random", &ralg),
        make_pair("Greedy", &rgreedy),
        make_pair("BL", &rls)
    };

	if (modo == 'i') // Modo individual
	{
		if (algoritmo.empty() || algoritmos.find(algoritmo) == algoritmos.end())
		{
			cerr << "Error: En modo individual, debe especificar un algoritmo valido con -a.\n";
			cout << "Algoritmos disponibles: Random, Greedy, BL\n";
			return 1;
		}
		cout << "Ejecutando " << algoritmo << "...\n";
		auto mh = algoritmos[algoritmo];
		ResultMH<int> result = mh->optimize(*problem, num_evaluaciones);
		cout << "Mejor solucion: " << result.solution << endl;
		cout << "Mejor fitness: " << result.fitness << endl;
		cout << "Evaluationes: " << result.evaluations << endl;
	}
	else if (modo == 'c') // Modo CSV para boxplot
	{
		// CABECERA PARA EL CSV
		cout << "alg,fitness" << endl;

		for (int i = 0; i < n_ejecuciones; i++) 
		{
			for (auto const& [nombre, mh] : algoritmos) 
			{
				// Cambiamos la semilla en cada ejecucion para que no salga siempre lo mismo
				// pero que sea reproducible si usamos la misma 
				Random::seed(semilla + i); 
				
				ResultMH<int> result = mh->optimize(*problem, num_evaluaciones);
				
				// IMPRESION EN FORMATO CSV
				cout << nombre << "," << result.fitness << endl;
			}
		}
	}
	else if (modo == 't') // Modo tabla LaTeX
	{
		// Cabecera del entorno LaTeX
        cout << "\\begin{table}[htpb]" << "\n";
        cout << "\\centering" << "\n";
        
        // Personalizacion del caption con los nombres de los archivos usados
        cout << "\\caption{Resultados obtenidos para el dataset " << f_datos 
             << " con archivo " << f_const << "}" << "\n";
             
        cout << "\\begin{tabular}{lccccc}" << "\n";
        cout << "\\hline" << "\n";
        cout << "Algoritmo & Fitness & Distancia & Incumplimiento & Evaluaciones & Tiempo (seg) \\\\" << "\n";
        cout << "\\hline" << "\n";

        for (auto const& [nombre, mh] : algoritmos) 
		{
			// Variables para acumular y hacer la media
			double media_fitness = 0.0;
			double media_distancia = 0.0;
			double media_incumplimientos = 0.0;
			double media_evaluaciones = 0.0;
			double media_tiempo = 0.0;

			for (int i = 0; i < n_ejecuciones; i++)
			{
				Random::seed(semilla + i); // Cambiamos la semilla en cada ejecucion

				// Medir tiempo
				auto start = chrono::high_resolution_clock::now();
				ResultMH<int> result = mh->optimize(*problem, num_evaluaciones);
				auto end = chrono::high_resolution_clock::now();
				chrono::duration<double> tiempo = end - start;

				// Acumular resultados para la media
				media_fitness += result.fitness;
				media_distancia += problema.CalcularDesviacion(result.solution);
				media_incumplimientos += problema.CalcularInfeasibility(result.solution);
				media_evaluaciones += result.evaluations;
				media_tiempo += tiempo.count();
			}

			// Calcular medias
			media_fitness /= n_ejecuciones;
			media_distancia /= n_ejecuciones;
			media_incumplimientos /= n_ejecuciones;
			media_evaluaciones /= n_ejecuciones;
			media_tiempo /= n_ejecuciones;

            printf("%s & %.3f & %.3f & %.3f & %.0f & %.4f \\\\\n", 
                   nombre.c_str(),
				   media_fitness,
				   media_distancia,
                   media_incumplimientos,
				   media_evaluaciones,
				   media_tiempo
				);
        }

        // Cierre del entorno LaTeX
        cout << "\\hline" << "\n";
        cout << "\\end{tabular}" << "\n";
        cout << "\\end{table}" << "\n";
	}

    return 0;
}