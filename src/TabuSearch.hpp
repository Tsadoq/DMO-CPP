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
    vector<Solution*> tabuSol;
    
    Solution* tabuControl(Solution* actual, Solution* candidateNewSol);
    int tabuCheck(Solution* s);
};