# pragma once
#include "TabuSearch.hpp"
#include <vector>
#include <iostream>
#include "Solution.hpp"

using namespace std;

class TabuSearch{
public:
    int dim;
    int maxIter;
    vector<vector<int>> tabuList;

    TabuSearch(int dimension, int max_iter);
    Solution* tabu_search(Solution *sol, int n_exams);
};