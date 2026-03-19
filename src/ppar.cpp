#include "ppar.h"

#include <math.h>
#include <sstream>
#include <iostream>
#include <fstream>
#include <string>

using namespace Par;

/////////////////////////////////////////
/// Implementacion de la clase ProblemPar
/////////////////////////////////////////

/////////////////////
/// PRIVATE FUNCTIONS
/////////////////////

int ProblemPar::CargarInstancias(std::string ruta_instancias)
{
	std::ifstream a_instancias(ruta_instancias);

	if (!a_instancias.is_open())
	{
		std::cerr << "Error al abrir archivo " << ruta_instancias << std::endl;
		return 1;
	}

	std::string linea;
	while (std::getline(a_instancias, linea))
	{
		std::stringstream ss(linea);
		std::string valor;
		std::vector<float> fila;

		while (std::getline(ss, valor, ','))
			fila.push_back(stof(valor));
		
		this->instancias.push_back(fila);
	}

	a_instancias.close();
	this->num_instancias = this->instancias.size();
	return 0;
}

int ProblemPar::CargarRestricciones(std::string ruta_restricciones)
{
	std::ifstream a_restricciones(ruta_restricciones);

	if (!a_restricciones.is_open())
	{
		std::cerr << "Error al abrir archivo " << ruta_restricciones << std::endl;
		return 1;
	}

	// Reservar memoria para la matriz de restricciones
	this->m_restricciones = std::vector<std::vector<short>>(num_instancias, std::vector<short>(num_instancias, 0));

	std::string linea;
	size_t i = 0, j = 0;
	while (std::getline(a_restricciones, linea))
	{
		j = 0;
		std::stringstream ss(linea);
		std::string valor;
		Restriccion actual;

		while (std::getline(ss, valor, ','))
		{
			int tipo_restriccion = stoi(valor);
			this->m_restricciones[i][j] = tipo_restriccion;
			if (i < j && tipo_restriccion != 0)
			{
				Restriccion actual;
				actual.tipo = tipo_restriccion;
				actual.i = i;
				actual.j = j;
				this->l_restricciones.emplace_back(actual);
			}
			j++;
		}
		i++;
	}
	a_restricciones.close();

	return 0;
}

void ProblemPar::CalcularLambda()
{
	double max_distancia = 0;

	for (size_t i = 0; i < instancias.size() - 1; i++)
	{
		for (size_t j = i+1; j < instancias.size(); j++)
		{
			double distancia = Distancia(instancias[i], instancias[j]);
			if (distancia > max_distancia)
				max_distancia = distancia;
		}
	}
	if (l_restricciones.size() > 0)
		this->lambda = max_distancia / l_restricciones.size();
	else
		this->lambda = 0.0;
}

double ProblemPar::CalcularDesviacion(const tSolution<int> &solucion)
{
	return 0;
}

int ProblemPar::CalcularInfeasibility(const tSolution<int> &solucion)
{
	return 0;
}

////////////////////
/// PUBLIC FUNCTIONS
////////////////////

ProblemPar::ProblemPar(size_t num_clusters, std::string ruta_instancias, std::string ruta_restricciones) : Problem<int>() 
{
	this->num_clusters = num_clusters;
	int err1 = CargarInstancias(ruta_instancias);
	int err2 = CargarRestricciones(ruta_restricciones);
	if (err1 || err2)
	{
		std::cerr << "Error al cargar los datos" << std::endl;
		return;
	}
	CalcularLambda();
}
