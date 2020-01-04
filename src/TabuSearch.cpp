#include "TabuSearch.hpp"
#include <iostream>
#include <vector>
#include <algorithm>
#include "Solution.hpp" 

using namespace std;

TabuSearch::TabuSearch(int dimension, int max_iter):
dim(0), maxIter(0)
{
    dim = dimension;
    maxIter = max_iter;
}

Solution* TabuSearch::tabu_search(Solution *sol, int n_exams)
{
    int i=0, confl = -1, best_candidate_conflicts = -1, candidate_conflicts = -1;
    vector<int> best_sol = sol->timeslot_per_exams;
    vector<int> best_candidate = sol->timeslot_per_exams;
    Solution* current_sol = sol;
    vector<vector<int>> sol_neighbourhood;
    tabuList.push_back(best_sol);

    while(i < maxIter || confl > 0 ){
        sol_neighbourhood = current_sol->getNeighbourhood(50); // neighbourhood vector<vector<int>>
        for(auto &candidate : sol_neighbourhood){
            candidate_conflicts = conflicts(candidate)
            if(candidate_conflicts != 0 
            && (std::find(sol_neighbourhood.begin(), sol_neighbourhood.end(), candidate) == sol_neighbourhood.end()) 
            && (candidate_conflicts < conflicts(best_candidate))){
                 best_candidate = candidate;
             }
        }
        best_candidate_conflicts = conflicts(best_candidate);
        if (best_candidate_conflicts < confl){
            best_sol = best_candidate;
            confl = best_candidate_conflicts;
        }

        tabuList.push_back(best_candidate); // add best sol in neighbourhood to tabu list
        // when tabu list is full, delete first element
        if(tabuList.size() > dim){
            tabuList.erase(tabuList.begin());
        }
        current_sol->new_solution(best_sol, n_exams);
        i++;
    }

    return current_sol;;
}

int TabuSearch::conflicts(vector<int> solution){
    return std::count(solution.begin(), solution.end(), -2);
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