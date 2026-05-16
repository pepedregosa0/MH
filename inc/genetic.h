/*
Archivo: genetic.h
Autor: josepc
Descripcion: Implementacion de la clase Genetic, que representa
algoritmos de busqueda genetica.
*/

#pragma once
#include <mh.h>
#include "random.hpp"
#include <vector>

using namespace std;

template <typename tDomain>
using CrossoverOp = void (*)(const vector<tDomain>&, const vector<tDomain>&, 
							 vector<tDomain>&, vector<tDomain>&);

template <typename tDomain>
class GeneticAlgorithm : public MH<tDomain>
{
protected:
	CrossoverOp<tDomain> cruce_op;
	int tam_poblacion = 50;
	double prob_mutacion = 0.1;
public:
	GeneticAlgorithm(CrossoverOp<tDomain> cruce) : MH<tDomain>(), cruce_op(cruce) {}
	virtual ~GeneticAlgorithm() {}

	virtual ResultMH<tDomain> optimize(Problem<tDomain> &problem, int maxevals) override = 0;
};


/////////////
/// Clase AGG
/////////////
template <typename tDomain>
class AGG : public GeneticAlgorithm<tDomain>
{
public:
	AGG(CrossoverOp<tDomain> cruce) : GeneticAlgorithm<tDomain>(cruce) {}
	virtual ResultMH<tDomain> optimize(Problem<tDomain> &problem, int maxevals) override;
};


/////////////
/// Clase AGE
//////////////
template <typename tDomain>
class AGE : public GeneticAlgorithm<tDomain>{
public:
	AGE(CrossoverOp<tDomain> cruce) : GeneticAlgorithm<tDomain>(cruce) {}
	virtual ResultMH<tDomain> optimize(Problem<tDomain> &problem, int maxevals) override;
};