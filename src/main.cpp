#include "Exam.hpp"
#include "Solution.hpp"
#include "header.hpp"
#include "Exam.cpp"
#include "Solution.cpp"
#include "TabuSearch.hpp"
#include "TabuSearch.cpp"
#include "TSforInitialSolution.hpp"
#include "read_file_slo.cpp"
#include "read_file_exm.cpp"
#include "read_file_stu.cpp"
#include "sort_indexes.cpp"
#include "graph_coloring_greedy.cpp"
#include "neighbours.cpp"
#include "sa.cpp"
#include "TSforInitialSolution.cpp"
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
    char * writable_instance_exm = new char[instance_exm.size() + 1];
    std::copy(instance_exm.begin(), instance_exm.end(), writable_instance_exm);
    writable_instance_exm[instance_exm.size()] = '\0';

    // read number of timeslots
    n_timeslot = read_file_slo(instance_slo);
    fprintf(stdout, "Number of timeslot: %d\n", n_timeslot);
    Solution *initial_solution = new Solution();
    // read number of exams
    vector<double> weights=read_file_exm(writable_instance_exm,n_exams);    
    fprintf(stdout, "Number of exams: %d\n", n_exams);
    
    // create the conflict matrix: for each pair of exams the number of students willing to give both exams
    vector<vector<int>> conflict_matrix;
    int total_number_students=0;
    conflict_matrix=read_file_stu(writable_instance_stu,n_exams,total_number_students);
    cout<<"Number of students: "<<total_number_students<<endl;
    
    
    initial_solution->solution_update(conflict_matrix, n_exams);
    initial_solution->other_weights=weights;
    for (int i=0;i<n_exams;i++){
        initial_solution->other_weights[i]=initial_solution->num_neighbours_for_exams[i]/initial_solution->other_weights[i];
    }
    
    // sort exams by decreasing value of number of neighbours
    vector<size_t> sorted_index=vector<size_t>(n_exams);
    // it's a vector of indexes: values in [0,n_exams-1]       
    sorted_index=sort_indexes(initial_solution->other_weights);
   
    // apply greedy coloring trying to assign timeslots first to exams with higher degree
    
    graph_coloring_greedy(initial_solution, n_timeslot, sorted_index, n_exams); 
    initial_solution->update_timeslots(n_exams);
    int flag = initial_solution->check_feasibility(initial_solution->timeslot_per_exams, initial_solution->all_exams);
    TSforInitialSolution* TS=new TSforInitialSolution();
    TS->dim=n_timeslot;
    TS->maxIter=100000;
    int ts=TS->tabu_search(initial_solution,n_exams,n_timeslot,conflict_matrix);
    cout<<ts<<endl;
    //initial_solution->write_output_file(current_instance, n_exams);
    flag = initial_solution->check_feasibility(initial_solution->timeslot_per_exams, initial_solution->all_exams);

    /*int i=0;
    while(flag==1 && i<1000){
        unsigned seed = std::chrono::system_clock::now().time_since_epoch().count();    
        std::shuffle(sorted_index.begin(), sorted_index.end(),std::default_random_engine(seed) );
        graph_coloring_greedy(initial_solution, n_timeslot, sorted_index, n_exams); 
        initial_solution->update_timeslots(n_exams);
        i++;
    }*/

    // AGGIUNGERE TS O QUALCOS'ALTRO PER ASSICURARSI CHE LA SOLUZIONE INIZIALE SIA FEASIBLE
    // O GESTIRE LE PENALITA' NELLA OBJ FUNCTION PER CONVERGERE ALLA FEASIBILITY
    cout<<"Flag: "<<flag<<endl;
/*
    //PROVIAMO A MODIFICARE LA SOLUZIONE INIZIALE DI TANTsO PRIMA DI LANCIARE
    initial_solution->update_timeslots(n_exams);
    vector<double> weight_for_exams=initial_solution->update_weights(n_exams);
    vector<int> possible_timeslots;
    for (int i=0; i<n_timeslot;i++){
        possible_timeslots.push_back(i+1);
    }
*/
    sa(initial_solution, start, timelimit, n_exams, total_number_students, n_timeslot,"./instances/"+current_instance+".sol");

   
    return 0;
}

