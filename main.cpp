#include "Exam.hpp"
#include "Solution.hpp"
#include "read_file_slo.hpp"
#include "read_file_exm.hpp"
#include "read_file_stu.hpp"
#include "sort_by_num_neighbours.hpp"
#include "graph_coloring_greedy.hpp"
#include "Exam.cpp"
#include "Solution.cpp"
#include "read_file_slo.cpp"
#include "read_file_exm.cpp"
#include "read_file_stu.cpp"
#include "sort_by_num_neighbours.cpp"
#include "graph_coloring_greedy.cpp"
#include <fstream>
#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ios>
#include <time.h> //per tempo per simulating annealing

//int read_file_stu(char *name_stu);
int main(int argc, char **argv) {
    int n_exams = 0;
    int n_timeslot = 0;
    string current_instance=argv[1];
    string instance_exm=".\\instances\\"+current_instance+".exm";
    string instance_slo=".\\instances\\"+current_instance+".slo";
    string instance_stu=".\\instances\\"+current_instance+".stu";
    char * writable_instance_stu = new char[instance_stu.size() + 1];
    std::copy(instance_stu.begin(), instance_stu.end(), writable_instance_stu);
    writable_instance_stu[instance_stu.size()] = '\0';

    // read number of timeslots
    n_timeslot = read_file_slo(instance_slo);
    fprintf(stdout, "Number of timeslot: %d\n", n_timeslot);

    // read number of exams
    n_exams=read_file_exm(instance_exm);
    fprintf(stdout, "Number of exams: %d\n", n_exams);

    // create the conflict matrix: for each pair of exams the number of students willing to give both exams
    vector<vector<int>> conflict_matrix;
    conflict_matrix=read_file_stu(writable_instance_stu,n_exams);
    
    // vector of exams pointer
    vector<Exam*> all_exams;
    // for each exam save the number of neighbour
    vector<int> num_neighbours_for_exams;
    // initialize attriburtes conflict_exams and conflict_weights for each exam
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
    
    // sort exams by decreasing value of number of neighbours
    vector<size_t> sorted_index=vector<size_t>(n_exams);
    // it's a vector of indexes: values in [0,n_exams-1]   
    sorted_index=sort_indexes(num_neighbours_for_exams);
    
    // apply greedy coloring trying to assign timeslots first to exams with higher degree
    vector<int> assigned_timeslots=vector<int>(n_exams);
    assigned_timeslots=graph_coloring_greedy(all_exams, n_timeslot, sorted_index, n_exams);

    ofstream output_file;
    output_file.open ("instance01.sol");
    for(int i=0; i<n_exams;i++){    
        output_file << i+1 <<"\t"<< assigned_timeslots[i]<<"\n"; 
    }
    output_file.close();

    return 0;
}