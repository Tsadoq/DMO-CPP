# pragma once
#include <vector>
#include <iostream>
#include "Solution.hpp"

using namespace std;

class TSforInitialSolution{
public:
    int dim;
    int maxIter;
    //vector<vector<int>> tabuList;
    void tabu_search(Solution *sol, int n_exams,int n_timeslot);
};