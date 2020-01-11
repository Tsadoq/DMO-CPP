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

void localSearch(Solution* solution, std::vector<int> possible_timeslots,std::vector<size_t> sorted_exams,int n_timeslot){
    //double starting_obj=solution->double_obj;
    std::vector<int> not_available_timeslots;
    not_available_timeslots.reserve(n_timeslot);
    std::vector<int> available_timeslots;
    available_timeslots.reserve(n_timeslot);
    double cost_in_obj;
    int n_exams=solution->n_exams;
    Exam* sort_exam;
    
    for(int i=0;i<n_exams;i++){
        available_timeslots.clear(); 
        not_available_timeslots.clear();
        sort_exam=solution->all_exams[sorted_exams[i]];
        not_available_timeslots=sort_exam->conflict_times; 
        not_available_timeslots.push_back(solution->timeslot_per_exams[sorted_exams[i]]); 
        // sort vector because set_difference works with sorted arrays 
        sort(not_available_timeslots.begin(), not_available_timeslots.end()); 
        // find available timeslot 
        set_difference(possible_timeslots.begin(), possible_timeslots.end(), not_available_timeslots.begin(),  
                        not_available_timeslots.end(),inserter(available_timeslots, available_timeslots.begin())); 
        for(auto time:available_timeslots){
            cost_in_obj=0;
            for(int j=0; j<sort_exam->conflict_times.size();j++){
                if(abs(time-sort_exam->conflict_times[j])<=5){
                    if(i>j){
                        cost_in_obj+=(1<<(5-abs(time-sort_exam->conflict_times[j])))*sort_exam->conflict_weights[j];                        
                    }                    
                }                
            }
            if (sort_exam->weight_in_obj_fun>cost_in_obj){
                sort_exam->weight_in_obj_fun=cost_in_obj;
                solution->timeslot_per_exams[sorted_exams[i]]=time;
            }
        }
        solution->update_timeslots();
        solution->update_weights();
    }
}