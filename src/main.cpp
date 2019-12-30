#include "Exam.hpp"
#include "Solution.hpp"
#include "header.hpp"
#include "Exam.cpp"
#include "Solution.cpp"
#include "read_file_slo.cpp"
#include "read_file_exm.cpp"
#include "read_file_stu.cpp"
#include "sort_indexes.cpp"
#include "graph_coloring_greedy.cpp"
#include "neighbours_by_mutation.cpp"
#include <fstream>
#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ios>
#include <time.h> //per tempo per simulating annealing
#include <math.h>

//int read_file_stu(char *name_stu);
int main(int argc, char **argv) {
    int n_exams = 0;
    int n_timeslot = 0;
    string current_instance=argv[1];
    string instance_exm="./instances/"+current_instance+".exm";
    string instance_slo="./instances/"+current_instance+".slo";
    string instance_stu="./instances/"+current_instance+".stu";
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
    int total_number_students=0;
    conflict_matrix=read_file_stu(writable_instance_stu,n_exams,total_number_students);
    fprintf(stdout, "conflict_matrix\n");
    
    Solution *initial_solution = new Solution();
    
    initial_solution->solution_update(conflict_matrix, n_exams);
    //fprintf(stdout, "%d\n", initial_solution->num_neighbours_for_exams[1]);
    
    // sort exams by decreasing value of number of neighbours
    vector<size_t> sorted_index=vector<size_t>(n_exams);
    // it's a vector of indexes: values in [0,n_exams-1]   
    
    sorted_index=sort_indexes(initial_solution->num_neighbours_for_exams);
   
    // apply greedy coloring trying to assign timeslots first to exams with higher degree

    //assigned_timeslots=graph_coloring_greedy(initial_solution, n_timeslot, sorted_index, n_exams); 
    graph_coloring_greedy(initial_solution, n_timeslot, sorted_index, n_exams); 

    // AGGIUNGERE TS O QUALCOS'ALTRO PER ASSICURARSI CHE LA SOLUZIONE INIZIALE SIA FEASIBLE
    // O GESTIRE LE PENALITA' NELLA OBJ FUNCTION PER CONVERGERE ALLA FEASIBILITY   
/*
    // inizialize all attributes for each exam
    for(int i=0;i<n_exams;i++){   
        // save timeslot for current exam 
        initial_solution->all_exams[i]->timeslot=assigned_timeslots[i];
        for (auto j:initial_solution->all_exams[i]->conflict_exams){
            // save timeslot of conflicting exams
            initial_solution->all_exams[i]->conflict_times.push_back(assigned_timeslots[j-1]);
        }
    }


    // create initial solution
    Solution *initial_solution = new Solution();

    int flag = initial_solution->check_feasibility(assigned_timeslots, all_exams);
    fprintf(stdout, "%d\n", flag);

    initial_solution->timeslot_per_exams = assigned_timeslots;
    initial_solution->total_exams = all_exams;

    vector<double> weight_for_exams;
    // calculate total weight in objective function for each exam
    int current_exam_timeslot;
    int neighbour_exam_timeslot;
    double neighbour_exam_weight;
    int weight_for_exam;
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
    
    // prova per vedere se ho fatto i conti giusti-> poi mettere in funzione a sè
    double obj_fun=0;
    for(int i=0;i<n_exams;i++){
        obj_fun+=all_exams[i]->weight_in_obj_fun;
    }
    obj_fun/=(2*total_number_students);
    cout<<"Objective function is: "<<obj_fun<<"\n";

    // sort exams by decreasing value of penalty in objective function
    vector<size_t> order_for_mutation=vector<size_t>(n_exams);
    // it's a vector of indexes: values in [0,n_exams-1]   
    order_for_mutation=sort_indexes(weight_for_exams);

    vector<int> possible_timeslots;
    for (int i=0; i<n_timeslot;i++){
        possible_timeslots.push_back(i+1);
    }

    int num_mutation=10;
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

    // I modify a copy of exams, so if I don't accept the new solution I have no problems with the old one
    vector<vector<int>>mutations_vector=neighbours_by_mutation(copy_all_exams, order_for_mutation, num_mutation, possible_timeslots);

    for (int i=0; i<num_mutation;i++){
        cout<<"Mutation: "<<"exam "<< mutations_vector[i][0]<<" timeslot "<<mutations_vector[i][1]<<"\n";
        // provo a vedere se è feasible con il checker -> OK è feasible
        assigned_timeslots[mutations_vector[i][0]-1]=mutations_vector[i][1];
    }
*/
  /*  string file_out=current_instance+".sol";
    ofstream output_file;
    output_file.open (file_out);
    for(int i=0; i<n_exams;i++){    
        output_file << i+1 <<"\t"<< initial_solution->timeslot_per_exams[i]<<"\n"; 
    }
    output_file.close();
*/
    return 0;
}