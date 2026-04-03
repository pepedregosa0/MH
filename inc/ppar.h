#pragma once
#include <problem.h>
#include <vector>

namespace Par {

/////////////////////////////////////////////////////////
/// Calculo de la distancia euclidea entre dos instancias
/////////////////////////////////////////////////////////
double Distancia(const std::vector<double> &instancia1, const std::vector<double> &instancia2);

struct Restriccion
{
	unsigned i; // Instancia 1
	unsigned j; // Instancia 2
	int tipo; // -1 CNL cant not link, 0 no hay restriccion, 1 ML must link
};

class ProblemPar : public Problem<int> 
{
private:
	std::vector<std::vector<double>> instancias;
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
	int CalcularInfeasibility(const tSolution<int> &solucion);
	double CalcularDesviacion(const tSolution<int> &solucion);

	/////////////////////////////////////////////////////////
	/// Funciones auxiliares para el calculo de la desviacion
	/////////////////////////////////////////////////////////
	std::vector<std::vector<double>> CalcularCentroides(const tSolution<int> &solucion);
    double CalcularDistanciaIntraCluster(int cluster, const std::vector<double> &centroide, const tSolution<int> &solucion);


	public:
	ProblemPar(size_t num_clusters, std::string ruta_instancias, std::string ruta_restricciones);
	
	void printInfo();
	
	virtual size_t getSolutionSize() override { return num_instancias; }
	
	virtual std::pair<int, int> getSolutionDomainRange() override 
	{
		return std::make_pair(0, num_clusters - 1);
	}
	
	tFitness fitness(const tSolution<int> &solucion);
	
	tSolution<int> createSolution() override;
	
	virtual bool isValid(const tSolution<int> &solution) override;
	
	virtual void fix(tSolution<int> &solution) override;

	//////////////////////////////////////////////////////////////////////////////////////////////
	/// Funcion para el algoritmo greedy, calcula incremento al asignar una instancia a un cluster
	//////////////////////////////////////////////////////////////////////////////////////////////
	int IncrementoInfeasibility(int instancia_i, int cluster_c, 
								const tSolution<int> &solucion_actual);
};

}
