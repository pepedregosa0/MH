/*
Archivo: memetic.h
Autor: josepc
Descripcion: Implementacion de la clase Memetic, que representa
algoritmos de busqueda memetica.
*/

#pragma once
#include <mh.h>
#include "random.hpp"
#include <vector>
#include <utility>
#include <algorithm> // Para swap

using namespace std;

template <typename tDomain> class Memetic : public MH<tDomain>
{

};
