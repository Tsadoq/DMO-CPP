#include "header.hpp" 
#include <string> 
#include <iostream> 
#include <numeric>  
#include <random>   
#include <algorithm> 
#include <vector> 
#include <chrono>
#include <iterator> 
#include "Exam.hpp" 
#include "Solution.hpp" 

void localSearch(Solution* solution, std::vector<size_t> sorted_exams){
    std::vector<int> possible_timeslots=solution->possible_timeslot;
    int n_timeslot=solution->n_timeslot;
    std::vector<int> not_available_timeslots;
    not_available_timeslots.reserve(n_timeslot);
    std::vector<int> available_timeslots;
    available_timeslots.reserve(n_timeslot);
    double cost_in_obj;
    int n_exams=solution->n_exams;
    Exam* sort_exam;
    int diff;
    int best_time;
    int old_time;
    
    for(int i=0;i<n_exams;i++){
        available_timeslots.clear(); 
        not_available_timeslots.clear();
        sort_exam=solution->all_exams[sorted_exams[i]];
        best_time=solution->timeslot_per_exams[sorted_exams[i]];
        old_time=best_time;
        not_available_timeslots=sort_exam->conflict_times; // PROBLEMA
        not_available_timeslots.push_back(solution->timeslot_per_exams[sorted_exams[i]]);
        
        // sort vector because set_difference works with sorted arrays 
        sort(not_available_timeslots.begin(), not_available_timeslots.end()); 
        // find available timeslot 
        
        set_difference(possible_timeslots.begin(), possible_timeslots.end(),
                         not_available_timeslots.begin(),not_available_timeslots.end(),
                         inserter(available_timeslots, available_timeslots.begin())   ); 

        for(auto time: available_timeslots){
            cost_in_obj=0;
           
            for(int j=0; j<sort_exam->conflict_times.size();j++){
                diff = abs(time-sort_exam->conflict_times[j]);
                if(diff<=5){
                    cost_in_obj+=(1<<(5-diff))*sort_exam->conflict_weights[j];   
                }              
            }
            
            if (sort_exam->weight_in_obj_fun>cost_in_obj){
                sort_exam->weight_in_obj_fun=cost_in_obj;
                best_time=time;
            }
        }
        solution->update_weights_conflicting(sorted_exams[i],best_time,old_time);
        solution->update_single_exam(sorted_exams[i],best_time);
        //----------------------------------------------
    }
}