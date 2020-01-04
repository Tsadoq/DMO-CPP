#include "TabuSearch.hpp"
#include <iostream>
#include <vector>
#include <algorithm>
#include "Solution.hpp" 


TabuSearch::TabuSearch(int dimension, int max_iter):
dim(0), maxIter(0)
{
    dim = dimension;
    maxIter = max_iter;
}

Solution* TabuSearch::tabu_search(Solution *sol, int n_exams)
{
    int i=0;
    vector<int> best_sol = sol->timeslot_per_exams;
    vector<int> best_candidate = sol->timeslot_per_exams;
    vector<vector<int>> sol_neighbourhood;
    tabuList.push_back(best_sol);

    while(i < maxIter){
        sol_neighbourhood = getNeighbors(best_candidate); // neighbourhood vector<vector<int>>
        for(auto &candidate : sol_neighbourhood){
            if((std::find(sol_neighbourhood.begin(), sol_neighbourhood.end(), candidate) == sol_neighbourhood.end()) &&
             (
                 conflicts(best_candidate) < conflicts(best_sol))
                 ){
                 best_candidate = &(candidate);
             }
        }

        if (conflicts(best_candidate) < conflicts(best_sol)){
            best_sol = best_candidate;
        }

        tabuList.push_back(best_candidate); // add best sol in neighbourhood to tabu list
        // when tabu list is full, delete first element
        if(tabuList.size() > dim){
            tabuList.erase(tabuList.begin());
        }

        i++;
    }

    // Maybe generate Solution given best_sol
    return new Solution;//best_sol;
}


// PSEUDOCODE
// best_sol = s0;
// best_candidate = s0;
// tabuList = [];
// tabuList.add(s0);
// while (i < max_iter){
//     sol_neighborhood = getNeighbors(best_candidate);
//     for (candidate in sol_neighborhood){
//         if ( (not tabuList.contains(sCandidate)) and (obj_funct(sCandidate) < obj_funct(bestCandidate)) )
//             bestCandidate ← sCandidate;
//     }   
    
//     if (obj_funct(best_candidate) < obj_funct(best_sol))
//         best_sol = best_candidate;

//     tabuList.add(bestCandidate);
//     if (tabuList.size > maxTabuSize)
//         tabuList.removeFirst();
    
// }

// return best_sol;