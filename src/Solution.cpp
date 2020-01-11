# pragma once
#include "Solution.hpp"
#include "Exam.hpp"
#include <vector>
#include <iostream>
#include <math.h>
#include <stdio.h>
#include <fstream>
#include<algorithm>



void Solution::solution_update(std::vector<vector<int>> conflict_matrix, int n_ex, int tot_num_students){
    
    // initialize fixed values
    total_number_students=tot_num_students;
    n_exams=n_ex;

    // initialize attributes conflict_exams and conflict_weights for each exam
    int num_neighbour;
    num_neighbours_for_exams.reserve(n_exams);
    for(int i=0;i<n_exams;i++){
        num_neighbour=0;
        Exam* exam = new Exam();
        // riservo memoria ai vettori
        exam->conflict_exams.reserve(n_exams);
        exam->conflict_weights.reserve(n_exams);
        for(int j=0;j<n_exams;j++){
            if (conflict_matrix[i][j]>0){
                num_neighbour++;
                // aggiungo la posizione dell'esame conflittuale
                exam->conflict_exams.push_back(j);
                // aggingo il peso dell'esame conflittuale
                exam->conflict_weights.push_back(conflict_matrix[i][j]);
            }
        }
        exam->id_exam=i;
        exam->num_conflict=num_neighbour;
        all_exams.push_back(exam);
        num_neighbours_for_exams.push_back(num_neighbour);
    }
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

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

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void Solution::update_timeslots(){    
    for(int i=0;i<n_exams;i++){   
        vector <int> conflict_times_new=vector<int>(all_exams[i]->num_conflict);
        // save timeslot for current exam 
        for (auto j:all_exams[i]->conflict_exams){
            // save timeslot of conflicting exams
            all_exams[i]->conflict_times.push_back(timeslot_per_exams[j]);            
        }
    }
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void Solution::write_output_file(string current_instance){
    string file_out=current_instance;
    ofstream output_file;
    output_file.open (file_out);
    for(int i=0; i<n_exams;i++){    
        output_file << i+1 <<"\t"<< timeslot_per_exams[i]<<"\n"; 
    }
    output_file.close();
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void Solution::update_weights(){
    // calculate total weight in objective function for each exam
    int current_exam_timeslot;
    int neighbour_exam_timeslot;
    double neighbour_exam_weight;
    double weight_for_exam;

    for(int i=0;i<n_exams;i++){
        weight_for_exam=0;
        current_exam_timeslot=timeslot_per_exams[i];
        for (int j=0;j<all_exams[i]->conflict_exams.size();j++){
            // save timeslot of conflicting exams
            if(j>i){                
                neighbour_exam_timeslot=all_exams[i]->conflict_times[j];
                neighbour_exam_weight=all_exams[i]->conflict_weights[j];
                if (abs(neighbour_exam_timeslot-current_exam_timeslot)<=5){
                    // bitshift operation
                    weight_for_exam += (1<<(5-abs(neighbour_exam_timeslot-current_exam_timeslot)))*neighbour_exam_weight;
                }
            }
        }
        all_exams[i]->weight_in_obj_fun=weight_for_exam;
    }
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

double Solution::objective_function(){
    double obj_fun=0;
    for(int i=0;i<n_exams;i++){
        obj_fun+=all_exams[i]->weight_in_obj_fun;
    }
    obj_fun/=total_number_students;
    return obj_fun;
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
Solution* Solution::copy_solution(){
    vector <Exam*> copy_all_exams=vector<Exam*>();
    // create a copy of all_exam, if we do a local search we will have #copies=#neighbours generated
    Exam* copy_exam;
    for(int i=0; i<n_exams;i++){
        copy_exam=new Exam();
        copy_exam->conflict_exams=all_exams[i]->conflict_exams;
        copy_exam->conflict_times=all_exams[i]->conflict_times;
        copy_exam->conflict_weights=all_exams[i]->conflict_weights;
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


