# pragma once
#include <vector>
#include <iostream>
#include "Solution.hpp"

using namespace std;

class TSforInitialSolution{
public:
    int dim;
    int maxIter;
    int tabu_search(Solution *sol, int n_exams,int n_timeslot, vector<vector<int>> conflict_matrix);
};