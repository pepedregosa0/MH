#pragma once
#include <problem.h>
#include <vector>

namespace Par {

/////////////////////////////////////////////////////////
/// Calculo de la distancia euclidea entre dos instancias
/////////////////////////////////////////////////////////
double Distancia(const std::vector<float> &instancia1, const std::vector<float> &instancia2);

struct Restriccion
{
	unsigned i; // Instancia 1
	unsigned j; // Instancia 2
	int tipo; // -1 CNL cant not link, 0 no hay restriccion, 1 ML must link
};

class ProblemPar : public Problem<int> 
{
private:
	std::vector<std::vector<float>> instancias;
	std::vector<std::vector<short>> m_restricciones;
	std::vector<Restriccion> l_restricciones;
	size_t num_instancias;
	size_t num_clusters;
	double lambda;


	//////////////////////////////
	/// Leer archivo de instancias
	//////////////////////////////
	int CargarInstancias(std::string ruta_instancias);

	//////////////////////////////
	/// Leer archivo restricciones
	//////////////////////////////
	int CargarRestricciones(std::string ruta_restricciones);

	////////////////////////////////////////////////////
	/// Funciones auxiliares para el calculo del fitness
	////////////////////////////////////////////////////
	void CalcularLambda();
	double CalcularDesviacion(const tSolution<int> &solucion);
	int CalcularInfeasibility(const tSolution<int> &solucion);

public:
	ProblemPar(size_t num_clusters, std::string ruta_instancias, std::string ruta_restricciones);
	
	virtual size_t getSolutionSize() override { return num_clusters; }

	virtual std::pair<int, int> getSolutionDomainRange() override 
	{
		return std::make_pair(false, true);
	}
	
	tFitness fitness(const tSolution<int> &solucion) override
	{
		double desviacion = CalcularDesviacion(solucion);
		int infeasibility = CalcularInfeasibility(solucion);
	
		return (desviacion + this->lambda * infeasibility);
	}

	tSolution<int> createSolution() override 
	{
		tSolution<int> solution(num_clusters);
		for (int i = 0; i < solution.size(); i++) 
		{
			solution[i] = Random::get<bool>();
		}
		return solution;
	}

	virtual bool isValid(const tSolution<int> &solution) override { return true; }

	virtual void fix(tSolution<int> &solution) override {}
};

};
