# pragma once
#include "Solution.hpp"
#include "Exam.hpp"
#include <vector>
#include <iostream>
#include <math.h>
#include <stdio.h>
#include <fstream>
#include <algorithm>
#include <bitset>
#include <numeric>


void Solution::solution_update(std::vector<std::vector<int>> conflict_matrix, int n_ex, int tot_num_students, int timeslot){
    
    // initialize fixed values
    total_number_students=tot_num_students;
    n_exams=n_ex;
    n_timeslot=timeslot;
    possible_timeslot=std::vector<int>(timeslot) ; // vector with 100 ints.
    std::iota (std::begin(possible_timeslot), std::end(possible_timeslot), 1);

    // initialize attributes conflict_exams and conflict_weights for each exam
    int num_neighbour;
    int position_in_conf;
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

    
    for(int i=0;i<n_exams;i++){
        for(auto conf_exams: all_exams[i]->conflict_exams){
            int pos=0;
            while(all_exams[conf_exams]->conflict_exams[pos] != all_exams[i]->id_exam){ 
                pos++;
            } 
            all_exams[i]->position_in_conf_exams.push_back(pos);
        }
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

    std::vector <int> conflict_times_new;

    for(int i=0;i< n_exams;i++){
        all_exams[i]->conflict_times.clear();
        // save timeslot for current exam 
 
        for (auto j:all_exams[i]->conflict_exams){
            // save timeslot of conflicting exams
            all_exams[i]->conflict_times.push_back(timeslot_per_exams[j]);        
            //std::cout<<all_exams[i]->id_exam<<" "<<timeslot_per_exams[j]<<std::endl; 
        }
    }
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void Solution::write_output_file(std::string current_instance){
    std::string file_out=current_instance;
    std::ofstream output_file;
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
    int diff;
    
    //std::cout<<"n_esami "<<n_exams<<std::endl;
    for(int i=0;i<n_exams;i++){
        weight_for_exam=0;
        current_exam_timeslot=timeslot_per_exams[i];
        for (int j=0;j<all_exams[i]->conflict_exams.size();j++){
            // save timeslot of conflicting exams               
            neighbour_exam_timeslot=all_exams[i]->conflict_times[j];
            neighbour_exam_weight=all_exams[i]->conflict_weights[j];
            diff=abs(neighbour_exam_timeslot-current_exam_timeslot);
            if (diff<=5){
                // bitshift operation
                diff=1<<5-diff;
                weight_for_exam += diff*neighbour_exam_weight;
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
    obj_fun/=(2*total_number_students);
    
    double_obj = obj_fun;

    return obj_fun;
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
Solution* Solution::copy_solution(){
    std::vector <Exam*> copy_all_exams=std::vector<Exam*>();
    // create a copy of all_exam, if we do a local search we will have #copies=#neighbours generated
    Exam* copy_exam;
    for(int i=0; i<n_exams;i++){
        copy_exam=new Exam();
        copy_exam->conflict_exams=all_exams[i]->conflict_exams;
        copy_exam->conflict_times=all_exams[i]->conflict_times;
        copy_exam->conflict_weights=all_exams[i]->conflict_weights;
        copy_exam->weight_in_obj_fun=all_exams[i]->weight_in_obj_fun;
        copy_exam->num_conflict= all_exams[i]->num_conflict;
        copy_exam->id_exam=all_exams[i]->id_exam;
        copy_exam->position_in_conf_exams=all_exams[i]->position_in_conf_exams;
        copy_all_exams.push_back(copy_exam);
    }
    Solution* copy_sol=new Solution();
    copy_sol->all_exams=copy_all_exams;
    copy_sol->timeslot_per_exams=timeslot_per_exams;
    copy_sol->num_neighbours_for_exams=num_neighbours_for_exams;
    copy_sol->n_exams=n_exams;
    copy_sol->n_timeslot=n_timeslot;
    copy_sol->possible_timeslot=possible_timeslot;
    copy_sol->total_number_students=total_number_students;
    
    return copy_sol;
}


//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void Solution::update_single_exam(int exam, int new_timeslot){

    timeslot_per_exams[exam]=new_timeslot;
    int pos_in_conf;
    int conf_exam;
    for(int i=0; i<all_exams[exam]->num_conflict;i++){
        pos_in_conf=all_exams[exam]->position_in_conf_exams[i];
        conf_exam=all_exams[exam]->conflict_exams[i];
        all_exams[conf_exam]->conflict_times[pos_in_conf]=new_timeslot;
    }
}


