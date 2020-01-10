# pragma once
#include "Solution.hpp"
#include "Exam.hpp"
#include <vector>
#include <iostream>
#include <math.h>
#include <fstream>
#include<algorithm>
using namespace std;



void Solution::solution_update(std::vector<vector<int>> conflict_matrix, int n_exams){
    // vector of exams pointer  
    // initialize attributes conflict_exams and conflict_weights for each exam
    int num_neighbour;
    for(int i=0;i<n_exams;i++){
        num_neighbour=0;
        Exam *exam = new Exam();
        exam->id_exam=i+1;
        for(int j=0;j<n_exams;j++){
            if (conflict_matrix[i][j]>0){
                num_neighbour++;
                exam->conflict_exams.push_back(j+1);
                exam->conflict_weights.push_back(conflict_matrix[i][j]);
            }
        }
        all_exams.push_back(exam);
        num_neighbours_for_exams.push_back(num_neighbour);
    }
}

int Solution::check_feasibility(std::vector<int> t, std::vector<Exam*> e){
    int flag = 0;
    for(int i=0;i<e.size();i++){
        for(int j=0; j<e[i]->conflict_exams.size(); j++){
            if((t[i]==e[i]->conflict_times[j]) || (t[i]<=0)){ 
                flag=1;
                return flag;
            }
        }
    }
    return flag;
}

void Solution::update_timeslots(int n_exams){    
    for(int i=0;i<n_exams;i++){   
        vector <int> conflict_times_new=vector<int>();
        // save timeslot for current exam 
        all_exams[i]->timeslot=timeslot_per_exams[i];
        for (auto j:all_exams[i]->conflict_exams){
            // save timeslot of conflicting exams
            conflict_times_new.push_back(timeslot_per_exams[j-1]);            
        }
        all_exams[i]->conflict_times=conflict_times_new;
    }
}

std::vector<double> Solution::update_weights(int n_exams){
    vector<double> weight_for_exams;
    // calculate total weight in objective function for each exam
    int current_exam_timeslot;
    int neighbour_exam_timeslot;
    double neighbour_exam_weight;
    double weight_for_exam;
    for(int i=0;i<n_exams;i++){
        weight_for_exam=0;
        current_exam_timeslot=all_exams[i]->timeslot;
        for (int j=0;j<all_exams[i]->conflict_exams.size();j++){
            // save timeslot of conflicting exams
            neighbour_exam_timeslot=all_exams[i]->conflict_times[j];
            neighbour_exam_weight=all_exams[i]->conflict_weights[j];
            if (abs(neighbour_exam_timeslot-current_exam_timeslot)<=5){
                weight_for_exam+=pow(2,5-abs(neighbour_exam_timeslot-current_exam_timeslot))*neighbour_exam_weight;
            }
        }
        all_exams[i]->weight_in_obj_fun=weight_for_exam;
        weight_for_exams.push_back(weight_for_exam);
    }
    return weight_for_exams;
}

double Solution::objective_function(int n_exams, int total_number_students){
    double obj_fun=0;
    for(int i=0;i<n_exams;i++){
        obj_fun+=all_exams[i]->weight_in_obj_fun;
    }
    obj_fun/=(2*total_number_students);
    return obj_fun;
}

Solution* Solution::copy_solution(int n_exams){
    vector <Exam*> copy_all_exams;
    // create a copy of all_exam, if we do a local search we will have #copies=#neighbours generated
    Exam* copy_exam;
    for(int i=0; i<n_exams;i++){
        copy_exam=new Exam();
        copy_exam->conflict_exams=all_exams[i]->conflict_exams;
        copy_exam->conflict_times=all_exams[i]->conflict_times;
        copy_exam->conflict_weights=all_exams[i]->conflict_weights;
        copy_exam->timeslot=all_exams[i]->timeslot;
        copy_exam->weight_in_obj_fun=all_exams[i]->weight_in_obj_fun;
        copy_exam->id_exam=all_exams[i]->id_exam;
        copy_all_exams.push_back(copy_exam);
    }
    Solution* copy_sol=new Solution();
    copy_sol->all_exams=copy_all_exams;
    copy_sol->timeslot_per_exams=timeslot_per_exams;
    copy_sol->num_neighbours_for_exams=num_neighbours_for_exams;
    return copy_sol;
}

void Solution::write_output_file(string current_instance, int n_exams){
    string file_out=current_instance;
    ofstream output_file;
    output_file.open (file_out);
    for(int i=0; i<n_exams;i++){    
        output_file << i+1 <<"\t"<< timeslot_per_exams[i]<<"\n"; 
    }
    output_file.close();
}

int Solution::change_exam(int pos_exam, int totTimeslots){
    int k;
    vector<int> possible_timeslots;
    vector<int> not_available;
    vector<int> possible_mutation;
    for (int i = 0; i <all_exams[pos_exam]->conflict_times.size(); i++){
        if (all_exams[pos_exam]->conflict_times[i] != -1){
            not_available.push_back(i);
        }    
    }
    // per gli spostamenti del scondo esame considerato
    for (int i=0; i<totTimeslots;i++){
        possible_timeslots.push_back(i+1);
    }
    sort(not_available.begin(), not_available.end());  
    set_difference(possible_timeslots.begin(), possible_timeslots.end(), not_available.begin(),  
                    not_available.end(),inserter(possible_mutation, possible_mutation.begin()));
    if (possible_mutation.size()> 0){
        int rand_index = rand() % possible_mutation.size();
        all_exams[pos_exam]->timeslot = possible_mutation[rand_index];
        timeslot_per_exams[pos_exam] = possible_mutation[rand_index];
        for (auto j : all_exams[pos_exam]->conflict_exams){ 
                k=0; 
                while(all_exams[j-1]->conflict_exams[k] != all_exams[pos_exam]->id_exam){ 
                    k++;
                } 
                all_exams[j-1]->conflict_times[k]=possible_mutation[rand_index];; 
            } 
        return 1;
    } else {
        return 0;
    }
}

int Solution::probability_creator(int n_timeslot, int pos_exam){
    int min = 200000;
    vector<int> probabilities;
    vector<int> notAvailable = all_exams[pos_exam]->conflict_times;
    vector<int> possible_mutation;
    notAvailable.push_back(timeslot_per_exams[pos_exam]);
    vector<int> possible_timeslots;
    // per gli spostamenti del scondo esame considerato
    for (int i=0; i<n_timeslot;i++){
        possible_timeslots.push_back(i+1);
    }
    sort(notAvailable.begin(), notAvailable.end());  
    set_difference(possible_timeslots.begin(), possible_timeslots.end(), notAvailable.begin(), notAvailable.end(),inserter(possible_mutation, possible_mutation.begin()));
    if (possible_mutation.size()> 0){
        vector<int> penalties = vector<int>(possible_mutation.size(),0);
        for (int i = 0; i <penalties.size(); i++){
            for (int j = 0; j<all_exams[pos_exam]->conflict_times.size(); j++){
                if ( abs( all_exams[pos_exam]->conflict_times[j] - possible_mutation[i]) <= 5) {
                    penalties[i] = penalties[i] + pow(2, 5-abs(all_exams[pos_exam]->conflict_times[j] - possible_mutation[i])) * all_exams[pos_exam]->conflict_weights[j];
                }
            }
            if (penalties[i]<= min){
                min = i;
            }
        }
        return possible_mutation[min];
    }
    else
        return 0;
 }