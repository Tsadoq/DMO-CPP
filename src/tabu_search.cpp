#include <iostream>
#include <vector>
#include <algorithm>
#include "Solution.hpp" 

Solution* tabu_search(Solution *sol, int **conflicts, int nexams, int tmax, int maxiter);

using namespace std;

Solution* tabu_search(Solution *sol, int n_exams, int dim, int max_iter)
{
    int i=0;
    Solution *best_sol = sol;
    Solution *best_candidate = sol;
    vector<vector<int>> tabulist;
    vector<Solution> sol_neighbourhood;
    tabulist.push_back(best_sol->timeslot_per_exams);

    while(i < max_iter){
        sol_neighbourhood = getNeighbors(best_candidate); //check how to retrieve neighbourhood
        for(auto &candidate : sol_neighbourhood){
            if((std::find(sol_neighbourhood.begin(), sol_neighbourhood.end(), candidate) == sol_neighbourhood.end()) &&
             (obj_funct(best_candidate) < obj_funct(best_sol))){
                 best_candidate = &(candidate);
             }
        }

        if (obj_funct(best_candidate) < obj_funct(best_sol)){
            best_sol = best_candidate;
        }

        tabulist.push_back(best_candidate->timeslot_per_exams); // add best sol in neighbourhood to tabu list
        // when tabu list is full, delete first element
        if(tabulist.size > dim){
            tabulist.erase(tabulist.begin());
        }

        i++;
    }

    return best_sol;
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