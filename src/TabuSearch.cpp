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

void TabuSearch::tabuControl(Solution* candidateNewSol, int n_exams,  int n_timeslot){
    int problem = tabuCheck(candidateNewSol);
    if ( problem== 0){
        if (tabuSol.size()>=10){
            tabuSol.erase(tabuSol.begin());    
        }
        tabuSol.push_back(candidateNewSol->timeslot_per_exams);
       
    }
    else{
        
        vector<double> weight_for_exams;
        vector<size_t> order_for_mutation=vector<size_t>(n_exams);
        weight_for_exams=candidateNewSol->update_weights(n_exams);
        order_for_mutation=sort_indexes(weight_for_exams);
        
        vector<int> possible_timeslots;
        for (int i=0; i<n_timeslot;i++){
            possible_timeslots.push_back(i+1);
        }
        int num_mutation=20;
        double perc=1;   
     
        while (problem != 0){
            vector<vector<int>>mutations_vector=neighbours_by_mutation(candidateNewSol, order_for_mutation,
                                                                 num_mutation, possible_timeslots,1,n_exams);
            problem = tabuCheck(candidateNewSol);
            weight_for_exams=candidateNewSol->update_weights(n_exams);
            order_for_mutation=sort_indexes(weight_for_exams);
        }
       
    }
}

int TabuSearch::tabuCheck(Solution* candidateNewSol){
    for (int i = 0; i< tabuSol.size(); i++){
        if (candidateNewSol->timeslot_per_exams == tabuSol[i]){
            return i+1; // così non viene confuso lo zero
        }
    }
    if (tabuSol.size()< 10){
        tabuSol.push_back(candidateNewSol->timeslot_per_exams);
    }
    return 0;
}