/*
Archivo: maincec.cpp
Autor: josepc
Descripcion: Main para ejecutar los algoritmos de MH en el problema CEC2017
*/

#include <iostream>
#include <vector>
#include <string>
#include <fstream>
#include <iomanip>
#include <unistd.h> // Para getopt
#include "random.hpp"
#include "pcec2017.h"

// Incluimos las tres versiones continuas de tu algoritmo
#include "hbia_continuous.h"
#include "memetic_hbia_continuous.h"
#include "hbia_continuous_ext.h"

using namespace std;

void print_use()
{
	cout << "Uso: ./main_cec -a <algoritmo> [-n <ejecuciones>]\n";
	cout << "Algoritmos disponibles: HBIA, HBIAM, HBIAExt\n";
	cout << "Ejemplo: ./main_cec -a HBIAM -n 10\n";
}

int main(int argc, char *argv[]) {
	int opt;
	string algoritmo = "";
	int num_runs = 10; // 10 por defecto

	// Parseo de argumentos
	while ((opt = getopt(argc, argv, "a:n:h")) != -1) {
		switch (opt) {
			case 'a': algoritmo = optarg; break;
			case 'n': num_runs = stoi(optarg); break;
			case 'h':
			default:
				print_use();
				return 1;
		}
	}

	// Validación del algoritmo
	if (algoritmo != "HBIA" && algoritmo != "HBIAM" && algoritmo != "HBIAExt")
	{
		cout << "Error: Algoritmo '" << algoritmo << "' no reconocido.\n";
		print_use();
		return 1;
	}

	vector<int> dimensions = {10, 30, 50};

	// Instanciamos las 3 clases
	HBIAContinuous hbia_base;
	HBIAMemeticoContinuous hbia_memetico;
	HBIAExtContinuous hbia_ext;

	cout << "==========================================" << endl;
	cout << " INICIANDO BENCHMARK CEC 2017 CON " << algoritmo << endl;
	cout << "==========================================" << endl;

	for (int dim : dimensions)
	{
		int max_evals = 10000 * dim;

		for (int func = 1; func <= 30; ++func)
		{
			// Nos saltamos la funcion 2 (rota en el benchmark oficial)
			if (func == 2)
			{
				cout << "Saltando Funcion 2 (eliminada del benchmark oficial)..." << endl;
				continue;
			}

			cout << "Ejecutando Funcion " << func << " en Dim " << dim << "..." << flush;

			vector<vector<double>> results_matrix(14, vector<double>(num_runs, 0.0));

			for (int run = 0; run < num_runs; ++run)
			{
				Random::seed(run + 1);
				
				// Inicializa automáticamente la carpeta con el nombre del flag
				ProblemCEC2017 problem(algoritmo, func, dim);

				vector<double>* err_hist_ptr = nullptr;

				// Selección dinámica de la metaheurística
				if (algoritmo == "HBIA")
				{
					hbia_base.optimize(problem, max_evals);
					err_hist_ptr = &hbia_base.error_history;
				}
				else if (algoritmo == "HBIAM")
				{
					hbia_memetico.optimize(problem, max_evals);
					err_hist_ptr = &hbia_memetico.error_history;
				}
				else if (algoritmo == "HBIAExt")
				{
					hbia_ext.optimize(problem, max_evals);
					err_hist_ptr = &hbia_ext.error_history;
				}

				// Volcar el historial de hitos en la matriz
				for (int i = 0; i < 14; ++i)
				{
					if (i < err_hist_ptr->size())
						results_matrix[i][run] = (*err_hist_ptr)[i];
					else
						results_matrix[i][run] = err_hist_ptr->back();
				}
			}
			cout << " ¡Completada!" << endl;

			// Guardar al archivo .txt con el nombre oficial que exige CEC
			string filename = "resultados_" + algoritmo + "/" + algoritmo + "_" + to_string(func) + "_" + to_string(dim) + ".txt";
			ofstream outfile(filename);
			if (outfile.is_open())
			{
				for (int i = 0; i < 14; ++i)
				{
					for (int run = 0; run < num_runs; ++run)
					{
						outfile << scientific << setprecision(8) << results_matrix[i][run];
						if (run < num_runs - 1)
							outfile << "\t";
					}
					outfile << "\n";
				}
				outfile.close();
			}
		}
	}

	cout << "\n¡Benchmark finalizado! Archivos generados con exito." << endl;
	return 0;
}