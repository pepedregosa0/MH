#include <cassert>
#include <greedy.h>
#include <iostream>
#include <pincrem.h>

using namespace std;

template <class T> void print_vector(string name, const vector<T> &sol) {
  cout << name << ": ";

  for (auto elem : sol) {
    cout << elem << ", ";
  }
  cout << endl;
}

/**
 * Create random solutions until maxevals has been achieved, and returns the
 * best one.
 *
 * @param problem The problem to be optimized
 * @param maxevals Maximum number of evaluations allowed
 * @return A pair containing the best solution found and its fitness
 */
ResultMHInt GreedySearch::optimize(ProblemInt &problem, int maxevals) {
  assert(maxevals > 0);
  vector<bool> values;
  ProblemIncrem realproblem = dynamic_cast<ProblemIncrem &>(problem); // esto solo se puede usar en greedy de verdad que suspendes te la juegas he te lo advierto
  tSolution<int> sol(problem.getSolutionSize());
  print_vector("sol_initial", sol);

  auto size = problem.getSolutionSize();

  for (int i = 0; i < size; i++) {
    values.push_back(i);
  }

  for (int r = 0; r < size / 2; r++) {
    vector<float> heuristics;

    for (auto option : values) {
      tHeuristic value = ((option % 2) == 1 ? sol.size() - option : sol.size());
      heuristics.push_back(value);
    }
    // print_vector("heuristics", heuristics);

    auto posi = min_element(heuristics.begin(), heuristics.end());
    int posi_int = posi - heuristics.begin();

    sol[values[posi_int]] = 1;
    values[posi_int] = values.back();
    values.pop_back();
  }
  
  tFitness fitness = problem.fitness(sol);
  return ResultMH(sol, fitness, 1);
}

int ProblemPar::IncrementoInfeasibility(int instancia_i, int cluster_c, const tSolution<int> &solucion_actual)
{
    int penalizacion = 0;
    
    // Recorremos la fila de la instancia_i en la matriz para ver sus relaciones con cada instancia j
    for (size_t j = 0; j < num_instancias; j++)
    {
        int tipo_restriccion = m_restricciones[instancia_i][j];
        
        if (tipo_restriccion != 0 && instancia_i != j) 
        {
            int cluster_j = solucion_actual[j];
            
            // Si es Must-Link (1) y las estamos poniendo en clusters distintos
            if (tipo_restriccion == 1 && cluster_c != cluster_j)
                penalizacion++;
            // Si es Cannot-Link (-1) y las estamos poniendo en el mismo cluster
            else if (tipo_restriccion == -1 && cluster_c == cluster_j)
                penalizacion++;
        }
    }
    return penalizacion;
}
