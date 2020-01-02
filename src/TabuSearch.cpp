#pragma once
#include "TabuSearch.hpp"
#include <vector>
#include <iostream>
#include "Exam.hpp"
#include "header.hpp"
//#include "sa.cpp"
#include "Solution.hpp"
#include "Solution.cpp"


using namespace std;

/*TabuSearch::TabuSearch()
{
}

TabuSearch::~TabuSearch()
{
}*/

Solution* TabuSearch::tabuControl(Solution* actual, Solution* candidateNewSol){
    int problem = tabuCheck(candidateNewSol);
    if ( problem== 0){
        if (tabuSol.size()<10){
        } else{
            tabuSol.erase(tabuSol.begin());    
        }
        tabuSol.push_back(candidateNewSol);
        return candidateNewSol;
    }
    else{
        cout<<"Ho beccato un fottuto ciclo, ora ci esco"<<endl;
        int n_exams = actual->all_exams.size();
        Solution* copy_sol=actual->copy_solution(n_exams); 
        vector<size_t> order_for_mutation=vector<size_t>(n_exams);
        vector<int> possible_timeslots;
        for (int i=0; i<13;i++){ // to be adjusted, it's i <n_timeslot
            possible_timeslots.push_back(i+1);
        }
        int num_mutation=20;
        double perc=1;   
        while (problem != 0){
            vector<vector<int>>mutations_vector=neighbours_by_mutation(copy_sol, order_for_mutation,
                                                                 num_mutation, possible_timeslots,1,actual-> all_exams.size());
            problem = tabuCheck(copy_sol);
        }
        return copy_sol;
    }
}

int TabuSearch::tabuCheck(Solution* s){
    for (int i = 0; i< tabuSol.size(); i++){
        if (s->timeslot_per_exams == tabuSol[i]->timeslot_per_exams){
            return i+1; // così non viene confuso lo zero
        }
    }
    if (tabuSol.size()< 10){
        tabuSol.push_back(s);
    }
    return 0;
}