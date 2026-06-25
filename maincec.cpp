#include <iostream>
#include <vector>
#include <string>
#include <fstream>
#include <iomanip>
#include "random.hpp"
#include "pcec2017.h"
#include "hbia_continuous.h"

using namespace std;

int main()
{
    // Dimensiones requeridas y número de ejecuciones reducidas como indicó tu profe
    vector<int> dimensions = {10, 30, 50}; 
    int num_runs = 10; 

    HBIAContinuous hbia_alg;

    cout << "==========================================" << endl;
    cout << " INICIANDO BENCHMARK CEC 2017 CON HBIA    " << endl;
    cout << "==========================================" << endl;

    for (int dim : dimensions)
    {
        int max_evals = 10000 * dim;

        for (int func = 1; func <= 30; ++func)
        {
            if (func == 2) {
                cout << "Saltando Funcion 2 (eliminada del benchmark oficial)..." << endl;
                continue;
            }
            cout << "Ejecutando Funcion " << func << " en Dim " << dim << "..." << flush;

            // Matriz de resultados: 14 filas (hitos) x 10 columnas (runs)
            vector<vector<double>> results_matrix(14, vector<double>(num_runs, 0.0));

            for (int run = 0; run < num_runs; ++run) {
                Random::seed(run + 1); // Semilla reproducible
                
                // ProblemCEC2017 llama por debajo a cec17_init
                ProblemCEC2017 problem("HBIA", func, dim); 
                
                // Ejecutamos la metaheurística
                hbia_alg.optimize(problem, max_evals);

                // Volcamos el historial de los 14 hitos en la columna correspondiente
                for (int i = 0; i < 14; ++i)
                {
                    if (i < hbia_alg.error_history.size())
                    {
                        results_matrix[i][run] = hbia_alg.error_history[i];
                    }
                    else
                    {
                        // Resguardo anticaídas si por redondeos falta la última evaluación
                        results_matrix[i][run] = hbia_alg.error_history.back(); 
                    }
                }
            }
            cout << " ¡Completada!" << endl;

            // Guardar al archivo .txt con el formato exacto requerido por extract.py
            string filename = "resultados_HBIA/HBIA_" + to_string(func) + "_" + to_string(dim) + ".txt";
            ofstream outfile(filename);
            if (outfile.is_open()) {
                for (int i = 0; i < 14; ++i) {
                    for (int run = 0; run < num_runs; ++run) {
                        outfile << scientific << setprecision(8) << results_matrix[i][run];
                        if (run < num_runs - 1) outfile << "\t";
                    }
                    outfile << "\n";
                }
                outfile.close();
            }
        }
    }

    cout << "\n¡Benchmark finalizado! Archivos .txt generados en este directorio." << endl;
    return 0;
}