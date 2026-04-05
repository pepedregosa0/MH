#pragma once
#include <mh.h>

using namespace std;

/**
 * Description of the Greedy Search metaheuristic
 *
 * @see MH
 * @see Problem
 */
// Instanciamos la plantilla con el tipo que nos interese
using MHInt = MH<int>;
using ProblemInt = Problem<int>;
using ResultMHInt = ResultMH<int>;

class GreedySearch : public MHInt 
{
private:
	std::vector<int> GenerarRSI(int n);
public:
	GreedySearch() : MH() {}
	virtual ~GreedySearch() {}
	// Implement the MH interface methods
	/**
	 * Run the Greedy Search algorithm to find the optimal solution.
	 */
	virtual ResultMH<int> optimize(Problem<int> &problem, int maxevals);

};
