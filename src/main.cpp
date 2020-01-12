#include "Exam.hpp"
#include "Solution.hpp"
#include "header.hpp"
#include "Exam.cpp"
#include "Solution.cpp"
#include "TabuSearch.hpp"
#include "TabuSearch.cpp"
#include"TSforInitialSolution.hpp"
#include "read_file_slo.cpp"
#include "read_file_exm.cpp"
#include "read_file_stu.cpp"
#include "sort_indexes.cpp"
#include "graph_coloring_greedy.cpp"
#include "neighbours.cpp"
#include "sa.cpp"
#include "TSforInitialSolution.cpp"
#include "temperature_init.cpp"
#include "alternativeColoring.cpp"
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

#include <omp.h>

// GUIDA per settare vscode e c++ da linux
// https://code.visualstudio.com/docs/cpp/config-wsl

using namespace std;

//int read_file_stu(char *name_stu);
int main(int argc, char **argv) {
    srand(time(NULL));
    struct timeb start;
    int n_exams = 0;
    int n_timeslot = 0;
    int num_max_mutations = 10;
    int num_before_swap = 10;
    int numproc = atoi(argv[3]);
    int t0 = 100;

    double cooling = 0.9999;
    if(argc > 4){
        t0 = atof(argv[4]);
        num_max_mutations = atoi(argv[5]);
        num_before_swap = atoi(argv[6]);
    }

    ftime(&start);
    cout<<"Starting"<<endl;

    string current_instance=argv[1];
    string instance_exm;
    string instance_slo;
    string instance_stu;
    bool path_to_use=false;
    
    if(path_to_use){
        instance_exm="/home/parallels/DMO-CPP/src/instances/"+current_instance+".exm";
        instance_slo="/home/parallels/DMO-CPP/src/instances/"+current_instance+".slo";
        instance_stu="/home/parallels/DMO-CPP/src/instances/"+current_instance+".stu";
    }
    else{
        instance_exm="./instances/"+current_instance+".exm";
        instance_slo="./instances/"+current_instance+".slo";
        instance_stu="./instances/"+current_instance+".stu";
    }
    
    
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

    cout<<"Number of students: "<<total_number_students<<endl;
    Solution* initial_solution=new Solution();    
    
    initial_solution->solution_update(conflict_matrix, n_exams);
    //fprintf(stdout, "%d\n", initial_solution->num_neighbours_for_exams[1]);
     // sort exams by decreasing value of number of neighbours
    vector<size_t> sorted_index=vector<size_t>(n_exams);
    // it's a vector of indexes: values in [0,n_exams-1]       
    sorted_index=sort_indexes(initial_solution->num_neighbours_for_exams);
    // apply greedy coloring trying to assign timeslots first to exams with higher degree
    //-----------------------------------------------------------------------------------------
    int ee=alternativeColoring(initial_solution,  n_timeslot, n_exams, sorted_index);
    cout<<"ho finito coloring "<<ee<<endl;
    //-----------------------------------------------------------------------------------------
    //graph_coloring_greedy(initial_solution, n_timeslot, sorted_index, n_exams); 
    initial_solution->update_timeslots(n_exams);
    int flag = initial_solution->check_feasibility(initial_solution->timeslot_per_exams, initial_solution->all_exams);
    //vector<double>weight_for_exams=initial_solution->update_weights(n_exams);
    cout<<"feasibility "<<flag<<endl;
    initial_solution->write_output_file("./instances/"+current_instance+".sol", n_exams);
   
    omp_set_dynamic(0);     // Explicitly disable dynamic teams
    omp_set_num_threads(numproc); // Use 4 threads for all consecutive parallel regions
 
    int mysize;
    mysize = omp_get_num_threads();
    cout << "  Number of processors available = " << omp_get_num_procs() << "\n";
    cout << "  Number of threads =              " << omp_get_max_threads() << "\n";

    vector<Solution*> array_sol;
    vector<Solution*> best_sol=vector<Solution*>(numproc);
    int id=0;
    // lancio in parallelo
    # pragma omp parallel private ( id ) shared(initial_solution)
    {
        id = omp_get_thread_num();
        //Solution *initial_solution = new Solution();

        //initial_solution = array[id];
        for(int i=0; i<numproc; i++){
            Solution* tmp= initial_solution->copy_solution(n_exams);
            array_sol.push_back(tmp);
        }
        string str_id = to_string(id);
        //double best_sol;
        best_sol[id] = sa(array_sol[id], start, timelimit, n_exams, total_number_students, n_timeslot,"./instances/"+current_instance+"_"+str_id+"_"+".sol",t0, num_max_mutations, cooling, num_before_swap);
        //array_sol[id]->double_obj=best_sol;
    }
    
    double min = 100 * best_sol[0]->double_obj;
    int index_best = 0;
    double avg =0;
    int counter =0;
    for(int i=0; i<numproc; i++){
       // prendo il minimo
       Solution *tmp = array_sol[i];
       //double tmp_obj = tmp->double_obj;
       counter++;
       if (best_sol[i]->double_obj < min){
           index_best = i;
           min=best_sol[i]->double_obj;
       }
       cout << "Solution of core " << i+1 << " has a score of " << best_sol[i]->double_obj << endl;
       avg+=best_sol[i]->double_obj;
    }

    // -----------------------------------------------------------------
    
    best_sol[index_best]->write_output_file("./instances/"+current_instance+".sol", n_exams);
    cout<<"Best solution:\t\t"<<best_sol[index_best]->double_obj<<endl;
    cout<<"Average solution:\t"<<avg/counter<<endl;
 
    return 0;
}

