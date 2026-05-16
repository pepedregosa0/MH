/*
Archivo: operadores.h
Autor: josepc
Descripcion: Implementacion de los operadores de cruce y mutacion.
*/

#pragma once
#include <vector>
#include <mh.h>

using namespace std;

// Genera dos hijos (h1, h2) a partir de dos padres (p1, p2)
template <typename tDomain>
void CruceUniforme(const vector<tDomain>& p1, const vector<tDomain>& p2,
				   vector<tDomain>& h1, vector<tDomain>& h2);

// Genera dos hijos (h1, h2) a partir de dos padres (p1, p2) intercambiando
// un segmento fijo de longitud aleatoria y posición aleatoria resto con uniforme
template <typename tDomain>
void CruceSegmentoFijo(const vector<tDomain>& p1, const vector<tDomain>& p2,
					   vector<tDomain>& h1, vector<tDomain>& h2);

// Se generan dos numeros uno para la posicion a mutar y otro para decidir el valor
template <typename tDomain>
void MutacionUniforme(vector<tDomain>& solucion, double prob_mutacion, int k);

// Selecciona el índice del mejor individuo enfrentando a 'k_torneo' candidatos aleatorios
int SeleccionPorTorneo(const vector<double>& fitness_poblacion, int k_torneo = 3);
