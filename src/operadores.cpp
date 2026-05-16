/*
Archivo: operadores.cpp
Autor: josepc
Descripcion: Implementacion de los operadores de cruce.
*/
#include "operadores.h"

template <typename tDomain>
void CruceUniforme(const vector<tDomain>& p1, const vector<tDomain>& p2, vector<tDomain>& h1, vector<tDomain>& h2)
{
	int N = p1.size();
	h1.resize(N);
	h2.resize(N);
	
	for(int i = 0; i < N; i++)
	{
		// Probabilidad del 50% de intercambiar
		if(Random::get<double>(0.0, 1.0) < 0.5)
		{
			h1[i] = p2[i];
			h2[i] = p1[i];
		}
		else
		{
			h1[i] = p1[i];
			h2[i] = p2[i];
		}
	}
}

template <typename tDomain>
void CruceSegmentoFijo(const vector<tDomain>& p1, const vector<tDomain>& p2, vector<tDomain>& h1, vector<tDomain>& h2)
{
	int N = p1.size();
	h1.resize(N);
	h2.resize(N);
	
	// Generamos posición inicial y tamaño del segmento
	int pos = Random::get<int>(0, N - 1);
	int tam = Random::get<int>(1, N / 3); 
	
	// Marcar qué posiciones pertenecen al segmento
	vector<bool> en_segmento(N, false);
	for(int k = 0; k < tam; k++)
	{
		int i = (pos + k) % N; 
		en_segmento[i] = true;
	}
	
	// Construimos los hijos
	for(int i = 0; i < N; i++)
	{
		if(en_segmento[i])
		{
			// DENTRO del segmento se copian de los padres respectivos sin modificar
			h1[i] = p1[i];
			h2[i] = p2[i];
		}
		else
		{
			// FUERA del segmento cruce uniforme (50% de probabilidad de intercambiar)
			if(Random::get<double>(0.0, 1.0) < 0.5)
			{
				h1[i] = p2[i]; // Se intercambian
				h2[i] = p1[i];
			}
			else
			{
				h1[i] = p1[i]; // No se intercambian
				h2[i] = p2[i];
			}
		}
	}
}

template <typename tDomain>
void MutacionUniforme(vector<tDomain>& solucion, double prob_mutacion, int k)
{
	// Evaluamos si el cromosoma entero va a mutar
	if (Random::get<double>(0.0, 1.0) < prob_mutacion)
	{
		int N = solucion.size();
		
		// Elegimos un gen aleatorio para mutar
		int gen = Random::get<int>(0, N - 1);
		
		// Elegimos a qué nuevo clúster lo mandamos
		int valor_actual = solucion[gen];
		int nuevo_valor = Random::get<int>(0, k - 1);
		
		// El guion exige que el nuevo valor sea obligatoriamente distinto al actual
		while (nuevo_valor == valor_actual) 
		{
			nuevo_valor = Random::get<int>(0, k - 1);
		}
		
		// Aplicamos el cambio
		solucion[gen] = nuevo_valor;
	}
}

// Selecciona el índice del mejor individuo enfrentando a 'k_torneo' candidatos aleatorios
int SeleccionPorTorneo(const vector<double>& fitness_poblacion, int k_torneo)
{
	int tam_poblacion = fitness_poblacion.size();
	
	// Elegimos al primer participante al azar
	int mejor_indice = Random::get<int>(0, tam_poblacion - 1);
	
	// Lo enfrentamos contra otros participantes aleatorios
	for (int i = 1; i < k_torneo; i++)
	{
		int contendiente = Random::get<int>(0, tam_poblacion - 1);
		
		// Nos quedamos con el de menor fitness (minimizamos distancia + restricciones)
		if (fitness_poblacion[contendiente] < fitness_poblacion[mejor_indice])
			mejor_indice = contendiente;
	}
	
	return mejor_indice;
}
