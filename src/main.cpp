#include "Exam.hpp"
#include "Solution.hpp"
#include "header.hpp"
#include "Exam.cpp"
#include "Solution.cpp"
#include "TabuSearch.hpp"
#include "TabuSearch.cpp"
#include "read_file_slo.cpp"
#include "read_file_exm.cpp"
#include "read_file_stu.cpp"
#include "sort_indexes.cpp"
#include "graph_coloring_greedy.cpp"
#include "neighbours_by_mutation.cpp"
#include "sa.cpp"
#include <fstream>
#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ios>
#include <time.h> //per tempo per simulating annealing
#include <math.h>
#include <time.h>
#include <sys/timeb.h>

//int read_file_stu(char *name_stu);
int main(int argc, char **argv) {
    struct timeb start;
    int n_exams = 0;
    int n_timeslot = 0;
    
    ftime(&start);

    string current_instance=argv[1];
    string instance_exm="./instances/"+current_instance+".exm";
    string instance_slo="./instances/"+current_instance+".slo";
    string instance_stu="./instances/"+current_instance+".stu";
    int timelimit = atoi(argv[2]);
    cout<<"timelimit: "<<timelimit<<endl;
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
        
    Solution *initial_solution = new Solution();
    
    initial_solution->solution_update(conflict_matrix, n_exams);
    //fprintf(stdout, "%d\n", initial_solution->num_neighbours_for_exams[1]);
    
    // sort exams by decreasing value of number of neighbours
    vector<size_t> sorted_index=vector<size_t>(n_exams);
    // it's a vector of indexes: values in [0,n_exams-1]       
    sorted_index=sort_indexes(initial_solution->num_neighbours_for_exams);
   
    // apply greedy coloring trying to assign timeslots first to exams with higher degree
    
    graph_coloring_greedy(initial_solution, n_timeslot, sorted_index, n_exams); 
    // AGGIUNGERE TS O QUALCOS'ALTRO PER ASSICURARSI CHE LA SOLUZIONE INIZIALE SIA FEASIBLE
    // O GESTIRE LE PENALITA' NELLA OBJ FUNCTION PER CONVERGERE ALLA FEASIBILITY  
    initial_solution->update_timeslots(n_exams);
    int flag = initial_solution->check_feasibility(initial_solution->timeslot_per_exams, initial_solution->all_exams);
/*
    //PROVIAMO A MODIFICARE LA SOLUZIONE INIZIALE DI TANTO PRIMA DI LANCIARE
    initial_solution->update_timeslots(n_exams);
    vector<double> weight_for_exams=initial_solution->update_weights(n_exams);
    vector<int> possible_timeslots;
    for (int i=0; i<n_timeslot;i++){
        possible_timeslots.push_back(i+1);
    }
    vector<size_t>order_for_mutation=sort_indexes(weight_for_exams);
    //int flag = initial_solution->check_feasibility(initial_solution->timeslot_per_exams, initial_solution->all_exams);
    vector<vector<int>>mutations_vector=neighbours_by_mutation(initial_solution, order_for_mutation, (int)(2*n_exams/3), possible_timeslots,1.0,n_exams);
    weight_for_exams=initial_solution->update_weights(n_exams);    
    double obj_new=initial_solution->objective_function(n_exams,total_number_students); 
    cout<<"new initial sol: "<<obj_new<<endl;
  */  
    // prova push Chiara
    sa(initial_solution, start, timelimit, n_exams, total_number_students, n_timeslot,current_instance);

    /*vector<int> x;
    x.push_back(10);
    x.push_back(20);

    vector<int> y;
    y.push_back(10);
    y.push_back(20);

    if (x == y){
        cout<<"Hai sbagliato"<<endl;
    } else{
        cout<<"Hai fatto bene"<<endl;
    }*/


    /*
    // calculate total weight in objective function for each exam
    vector<double> weight_for_exams=initial_solution->update_weights(n_exams);

    // calculate objective function
    double obj_fun=initial_solution->objective_function(n_exams,total_number_students);
    cout<<"Objective function: "<<obj_fun<<"\n";

    // sort exams by decreasing value of penalty in objective function
    vector<size_t> order_for_mutation=vector<size_t>(n_exams);
    // it's a vector of indexes: values in [0,n_exams-1]   
    order_for_mutation=sort_indexes(weight_for_exams);

    vector<int> possible_timeslots;
    for (int i=0; i<n_timeslot;i++){
        possible_timeslots.push_back(i+1);
    }

    int num_mutation=10;

    // create a copy of initial_solution, if we do a local search we will have #copies=#neighbours 
    Solution* copy_sol=initial_solution->copy_solution(n_exams);   

    // I modify the copy of solution, so if I don't accept the new solution I have no problems with the old one
    //vector<vector<int>>mutations_vector=neighbours_by_mutation(copy_sol, order_for_mutation, num_mutation, possible_timeslots);

    // update weights in the new solution
    //weight_for_exams=copy_sol->update_weights(n_exams);
    
    //obj_fun=copy_sol->objective_function(n_exams,total_number_students);
    //cout<<"New objective function: "<<obj_fun<<"\n";

    // write solution on file
    //copy_sol->write_output_file("./instances/"+current_instance, n_exams);
    */
   
    return 0;
}

