# pragma once
#include "Solution.hpp"
#include "Exam.hpp"
#include <vector>
#include <iostream>
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

int Solution::check_feasibility(std::vector<int> t, std::vector<Exam*> e) {
    int flag = 0;
    for(int i=0;i<e.size();i++){
        for(int j=0; j<e[i]->conflict_exams.size(); j++){
            if(t[i]==e[i]->conflict_times[j]){
                flag=1;
                return flag;
            }
        }
    }
    return flag;
}