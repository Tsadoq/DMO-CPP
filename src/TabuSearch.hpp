# pragma once
#include "TabuSearch.hpp"
#include <vector>
#include <iostream>
#include "Exam.hpp"
//#include "sa.cpp"
#include "Solution.hpp"

using namespace std;

class TabuSearch{
public:
    int listSize;
    vector<vector<int>> tabuSol;
    
    void tabuControl(Solution* candidateNewSol, int n_exams,  int n_timeslot);
    int tabuCheck(Solution* s);
};