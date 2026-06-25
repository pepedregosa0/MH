/*
Archivo: pcec2017.h
Autor: josepc
Descripcion: Clase para el problema de CEC2017, hereda de Problem<double>
*/

#pragma once

#include "problem.h"
#include <vector>
#include <string>
#include <algorithm>

// la cabecera en extern "C" para que C++ pueda enlazarla correctamente
extern "C" {
	#include "cec17.h"
}

class ProblemCEC2017 : public Problem<double>
{
private:
	size_t dimension;
	int funcid;
	std::string algname;

public:
	// Constructor: Recibe el nombre del algoritmo (para el fichero de salida),
	// el ID de la función (1-30) y la dimensión (10, 30, 50, etc.)
	ProblemCEC2017(const std::string& algname, int funcid, size_t dimension)
		: Problem<double>(), dimension(dimension), funcid(funcid), algname(algname) 
	{
		
		// Inicializamos el benchmark llamando a la API
		cec17_init(this->algname.c_str(), this->funcid, this->dimension);
	}

	virtual ~ProblemCEC2017() {}

	virtual size_t getSolutionSize() override
	{
		return dimension;
	}

	// El rango de búsqueda estandarizado para todas las funciones del CEC2017 es [-100, 100]
	virtual std::pair<double, double> getSolutionDomainRange() override
	{
		return std::make_pair(-100.0, 100.0);
	}

	// Evaluación de la solución
	virtual tFitness fitness(const tSolution<double>& solution) override
	{
		// La API en C de cec17_fitness recibe un double*. 
		// Usamos const_cast por si la firma en C no lo marca como const
		double fit = cec17_fitness(const_cast<double*>(solution.data()));
		return fit;
	}

	// Generar solución aleatoria
	virtual tSolution<double> createSolution() override
	{
		tSolution<double> sol(dimension);
		for (size_t i = 0; i < dimension; ++i)
			sol[i] = Random::get<double>(-100.0, 100.0);
		return sol;
	}

	// Comprobar que ningún valor del array se salga del dominio continuo
	virtual bool isValid(const tSolution<double>& solution) override
	{
		for (double val : solution)
		{
			if (val < -100.0 || val > 100.0)
				return false;
		}
		return true;
	}

	// Si algún valor se sale de los límites, lo redondeamos a los bordes
	virtual void fix(tSolution<double>& solution) override 
	{
		for (double& val : solution)
		{
			if (val < -100.0) val = -100.0;
			if (val > 100.0) val = 100.0;
		}
	}
};