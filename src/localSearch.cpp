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

void localSearch(Solution* solution, int n_exams, vector<int> possible_timeslots,vector<size_t> sorted_exams){
    //double starting_obj=solution->double_obj;
    vector<int> not_available_timeslots;
    vector<int> available_timeslots;
    double cost_in_obj;
    for(int i=0;i<n_exams;i++){
        available_timeslots=vector<int> (); 
        not_available_timeslots=solution->all_exams[sorted_exams[i]]->conflict_times; 
        not_available_timeslots.push_back(solution->all_exams[sorted_exams[i]]->timeslot); 
        // sort vector because set_difference works with sorted arrays 
        sort(not_available_timeslots.begin(), not_available_timeslots.end()); 
        // find available timeslot 
        set_difference(possible_timeslots.begin(), possible_timeslots.end(), not_available_timeslots.begin(),  
                        not_available_timeslots.end(),inserter(available_timeslots, available_timeslots.begin())); 
        for(auto time:available_timeslots){
            cost_in_obj=0;
            for(int j=0; j<solution->all_exams[sorted_exams[i]]->conflict_times.size();j++){
                if(abs(time-solution->all_exams[sorted_exams[i]]->conflict_times[j])<=5){
                    cost_in_obj=cost_in_obj+pow(2,5-abs(time-solution->all_exams[sorted_exams[i]]->conflict_times[j]))*solution->all_exams[sorted_exams[i]]->conflict_weights[j];
                }                
            }
            if (solution->all_exams[sorted_exams[i]]->weight_in_obj_fun>cost_in_obj){
                solution->all_exams[sorted_exams[i]]->weight_in_obj_fun=cost_in_obj;
                solution->timeslot_per_exams[sorted_exams[i]]=time;
            }
        }
        solution->update_timeslots(n_exams);
        solution->update_weights(n_exams);
    }
}