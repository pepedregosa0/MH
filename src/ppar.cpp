/*
Archivo: ppar.cpp
Autor: josepc
Descripcion: Implementacion de la clase ProblemPar, que representa el problema 
de clustering con restricciones PAR, hereda de Problem<int>
*/

#include "ppar.h"

#include <math.h>
#include <sstream>
#include <iostream>
#include <fstream>
#include <string>

using namespace Par;

double Par::Distancia(const std::vector<double> &instancia1, const std::vector<double> &instancia2)
{
	double distancia = 0;
	for (int i = 0; i < instancia1.size(); i++)
		distancia += pow(instancia1[i] - instancia2[i], 2);
	distancia = sqrt(distancia);

	return distancia;
}

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
		std::vector<double> fila;

		while (std::getline(ss, valor, ','))
			fila.push_back(std::stod(valor));
		
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
			int tipo_restriccion = std::stoi(valor);
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

double ProblemPar::CalcularDistanciaIntraCluster(int cluster, const std::vector<double> &centroide, const tSolution<int> &solucion)
{
	double dist_total = 0.0;
    int tam = 0;
	
    // Sumar las distancias de las instancias que pertenecen a este cluster
    for (size_t i = 0; i < num_instancias; i++) 
    {
		if (solucion[i] == cluster) 
        {
			dist_total += Distancia(instancias[i], centroide);
            tam++;
        }
    }
    // Devolver la media
    if (tam > 0) return dist_total / tam;
    return 0.0;
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

void ProblemPar::printInfo()
{
	std::cout << "--- INFO DEL PROBLEMA PAR ---" << std::endl;
    std::cout << "Numero de instancias (n): " << num_instancias << std::endl;
    std::cout << "Numero de clusters (k): " << num_clusters << std::endl;
    std::cout << "Total de restricciones en lista |R|: " << l_restricciones.size() << std::endl;
    std::cout << "Valor de Lambda calculado: " << lambda << std::endl;
    std::cout << "-----------------------------" << std::endl;
}

tFitness ProblemPar::fitness(const tSolution<int> &solucion)
{
	double desviacion = CalcularDesviacion(solucion);
	int infeasibility = CalcularInfeasibility(solucion);
	
	return (desviacion + this->lambda * infeasibility);
}

tSolution<int> ProblemPar::createSolution()
{
	tSolution<int> solucion(this->num_instancias);
	
	for (size_t i = 0; i < this->num_instancias; i++)
		solucion[i] = Random::get<int>(0, this->num_clusters - 1);
	
	if (!isValid(solucion))
		fix(solucion);
	
	return solucion;
}

bool ProblemPar::isValid(const tSolution<int> &solution) 
{ 
	// Creamos un vector de contadores inicializado a 0 para cada cluster
    std::vector<int> tam_cluster(this->num_clusters, 0);
	
    // Contamos cuantas instancias tiene cada cluster en esta solucion
    for (size_t i = 0; i < this->num_instancias; i++)
		tam_cluster[solution[i]]++;
	
    // Comprobamos si algun cluster se ha quedado vacio
    for (size_t c = 0; c < this->num_clusters; c++) 
    {
		if (tam_cluster[c] == 0) 
			return false;
    }
	
    return true; // Todos los clusteres tienen al menos 1 instancia
}

void ProblemPar::fix(tSolution<int> &solution) 
{
	std::vector<int> tam_cluster(this->num_clusters, 0);
	
    // Contar cuantas instancias tiene cada cluster actualmente
    for (size_t i = 0; i < this->num_instancias; i++) 
		tam_cluster[solution[i]]++;
	
    // Comprobar si algun grupo se ha quedado vacio
    for (size_t c = 0; c < this->num_clusters; c++) 
    {
		if (tam_cluster[c] == 0) 
        {
			// Buscar un cluster tenga mas de 1 instancia
            int cluster_donante = -1;
            do {
				cluster_donante = Random::get<int>(0, this->num_clusters - 1);
            } while (tam_cluster[cluster_donante] <= 1);
			
            // Robar su primera instancia y asignarla al cluster vacio 'c'
            for (size_t i = 0; i < this->num_instancias; i++) 
            {
				if (solution[i] == cluster_donante) 
                {
					solution[i] = c;
                    tam_cluster[cluster_donante]--;
                    tam_cluster[c]++;
                    break; // siguiente cluster vacio
                }
            }
        }
    }
}

// Solo para greedy
int ProblemPar::IncrementoInfeasibility(int instancia_i, int cluster_c, const tSolution<int> &solucion_actual)
{
	int penalizacion = 0;
	
    // Recorremos las relaciones de la instancia_i con todas las demás instancias j
    for (size_t j = 0; j < num_instancias; j++)
    {
		// Solo evaluamos si j es diferente de i y si j ya tiene asignado un cluster en la solucion actual
        if (instancia_i != j && solucion_actual[j] != -1)
        {
			int tipo_restriccion = m_restricciones[instancia_i][j]; 
			
            if (tipo_restriccion == 1 && cluster_c != solucion_actual[j]) // Must-Link
			penalizacion++;
            else if (tipo_restriccion == -1 && cluster_c == solucion_actual[j]) // Cannot-Link
			penalizacion++;
        }
	}
	return penalizacion;
}

std::vector<std::vector<double>> ProblemPar::GenerarCentroidesAleatorios()
{
	size_t num_caracteristicas = instancias.size();
    std::vector<std::vector<double>> centroides_iniciales(num_clusters, std::vector<double>(num_caracteristicas));
	
    // Para cada dimensión (característica), buscamos su min y max
    for (size_t j = 0; j < num_caracteristicas; j++) 
    {
		double min_val = instancias[0][j];
        double max_val = instancias[0][j];
		
        for (size_t i = 1; i < num_instancias; i++) 
        {
			if (instancias[i][j] < min_val) min_val = instancias[i][j];
            if (instancias[i][j] > max_val) max_val = instancias[i][j];
        }
		
        // Generamos el valor aleatorio para esta dimensión en los k centroides
        for (size_t c = 0; c < num_clusters; c++) 
		centroides_iniciales[c][j] = Random::get<double>(min_val, max_val);
    }
	
    return centroides_iniciales;
}

std::vector<std::vector<double>> &ProblemPar::getInstancias()
{
	return this->instancias;
}

std::vector<std::vector<double>> ProblemPar::CalcularCentroides(const tSolution<int> &solucion)
{
	size_t num_caracteristicas = instancias.size();
	std::vector<std::vector<double>> centroides(num_clusters, std::vector<double>(num_caracteristicas, 0.0));
	std::vector<int> tam_cluster(num_clusters, 0);
	
	// Sum caracteristicas por cada cluster
	for (size_t i = 0; i < num_instancias; i++) 
	{
		int c = solucion[i];
		tam_cluster[c]++;
		for (size_t j = 0; j < num_caracteristicas; j++) 
		centroides[c][j] += instancias[i][j];
	}
	
	// Dividir entre el tamaño para obtener el promedio
	for (size_t c = 0; c < num_clusters; c++) 
	if (tam_cluster[c] > 0) 
	for (size_t j = 0; j < num_caracteristicas; j++) 
	centroides[c][j] /= tam_cluster[c];
	return centroides;
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


int ProblemPar::CalcularInfeasibility(const tSolution<int> &solucion)
{
	int infeasibility = 0;
	
	// Recorremos la lista de restricciones para evaluar cada una de ellas
	for (size_t k = 0; k < l_restricciones.size(); k++)
	{
		unsigned i = l_restricciones[k].i;
		unsigned j = l_restricciones[k].j;
		int tipo = l_restricciones[k].tipo;
		
		if (tipo == 1 && solucion[i] != solucion[j])
			infeasibility++;
		else if (tipo == -1 && solucion[i] == solucion[j])
			infeasibility++;
	}
	return infeasibility;
}

double ProblemPar::CalcularDesviacion(const tSolution<int> &solucion)
{
	// Calcular los centroides de cada cluster
	std::vector<std::vector<double>> centroides = CalcularCentroides(solucion);
	double desviacion = 0.0;

	for (size_t c = 0; c < num_clusters; c++)
		desviacion += CalcularDistanciaIntraCluster(c, centroides[c], solucion);
	return desviacion / num_clusters;
}