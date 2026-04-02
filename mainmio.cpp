#include <iostream>
#include <problem.h>
#include <random.hpp>
#include <string>
#include <util.h>
// Real problem class
#include "pincrem.h"

#include "ppar.h"

// All all algorithms
// #include "greedy.h"
// #include "randomsearch.h"

using namespace std;
int main(int argc, char *argv[]) 
{
  // Inicializar semilla
  Random::seed(42);

  Par::ProblemPar problema(7, "../datos_PAR_curso2526/zoo_set.dat", "../datos_PAR_curso2526/zoo_set_const_15.dat");

  cout << "Generando solucion parcial" << endl;

  tSolution<int> sol= problema.createSolution();

  cout << "Solucion generada. Calculando fitness" << endl;

  tFitness fit = problema.fitness(sol);

  cout << "El fitness de la solucion aleatoria es: " << fit << endl;

  problema.printInfo();

  // Create the algorithms
  // RandomSearch<int> ralg = RandomSearch<int>();
  // GreedySearch rgreedy = GreedySearch();
  // Create the specific problem
  // ProblemIncrem rproblem = ProblemIncrem(10);
  // Solve using evaluations
  //vector<pair<string, MH<int> *> > algoritmos = {make_pair("RandomSearch", &ralg),
  //                                         make_pair("Greedy", &rgreedy)};
  // Problem<int> *problem = dynamic_cast<Problem<int> *>(&rproblem);

  //for (int i = 0; i < algoritmos.size(); i++) 
  //{
  //  Random::seed(seed);
  //  cout << algoritmos[i].first << endl;
  //  auto mh = algoritmos[i].second;
  //  ResultMH result = mh->optimize(*problem, 1000);
  //  cout << "Best solution: " << result.solution << endl;
  //  cout << "Best fitness: " << result.fitness << endl;
  //  cout << "Evaluations: " << result.evaluations << endl;
  //}

  return 0;
}
