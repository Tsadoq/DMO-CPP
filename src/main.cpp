#include "Exam.hpp"
#include "Solution.hpp"
#include "header.hpp"

#include "Exam.cpp"
#include "Solution.cpp"
#include "read_file_slo.cpp"
#include "read_file_exm.cpp"
#include "read_file_stu.cpp"
#include "sort_indexes.cpp"
#include "localSearch.cpp"
#include "neighbours.cpp"
#include "sa.cpp"
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


//int read_file_stu(char *name_stu);
int main(int argc, char **argv) {
    srand(time(NULL));
    struct timeb start;
    int n_exams = 0;
    int n_timeslot = 0;
    int numproc = atoi(argv[3]);


    ftime(&start);
    std::cout<<"Starting"<<std::endl;

    std::string current_instance=argv[1];
    std::string instance_exm;
    std::string instance_slo;
    std::string instance_stu;
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
    
    //--------------------------------------- READ FILES------------------------------------------------------
    int timelimit = atoi(argv[2]);
    std::cout<<"timelimit: "<<timelimit<<std::endl;
    char * writable_instance_stu = new char[instance_stu.size() + 1];
    std::copy(instance_stu.begin(), instance_stu.end(), writable_instance_stu);
    writable_instance_stu[instance_stu.size()] = '\0';

    // read number of timeslots
    n_timeslot = read_file_slo(instance_slo);
    fprintf(stdout, "Number of timeslot: %d\n", n_timeslot);
    
    // read number of exams
    n_exams=read_file_exm(instance_exm);
    fprintf(stdout, "Number of exams: %d\n", n_exams);
    

    //--------------------------------- FIXED VALUES CONSTRUCTION--------------------------------------------

    // create the conflict matrix: for each pair of exams the number of students willing to give both exams
    std::vector<std::vector<int>> conflict_matrix;
    int total_number_students=0;
    conflict_matrix=read_file_stu(writable_instance_stu,n_exams,total_number_students);

    std::cout<<"Number of students: "<<total_number_students<<std::endl;
    Solution* initial_solution=new Solution();    
    
    initial_solution->solution_update(conflict_matrix,n_exams, total_number_students);

    //-------------------------------- VARIABLE VALUES INITIALIZATION --------------------------------------------
     // sort exams by decreasing value of number of neighbours
    std::vector<size_t> sorted_index=std::vector<size_t>(n_exams);
    // it's a vector of indexes: values in [0,n_exams-1]       
    sorted_index=sort_indexes(initial_solution->num_neighbours_for_exams);
    
    //-----------------------------------------------------------------------------------------
    // apply a variant of greedy coloring trying to assign timeslots first to exams with higher degree
    alternativeColoring(initial_solution,  n_timeslot, n_exams,sorted_index);
    //-----------------------------------------------------------------------------------------
    initial_solution->update_timeslots();
    int flag = initial_solution->check_feasibility(initial_solution->timeslot_per_exams, initial_solution->all_exams);
    std::cout<<"feasibility "<<flag<<std::endl;

    //---------------------------------- MULTI-THREAD -----------------------------------------------------------
 
    omp_set_dynamic(0);     // Explicitly disable dynamic teams
    omp_set_num_threads(numproc); // Use 4 threads for all consecutive parallel regions
 
    int mysize;
    mysize = omp_get_num_threads();
    std::cout << "  Number of processors available = " << omp_get_num_procs() << "\n";
    std::cout << "  Number of threads =              " << omp_get_max_threads() << "\n";

    std::vector<Solution*> array_sol= std::vector<Solution*>(numproc);
    std::vector<Solution*> best_sol=std::vector<Solution*>(numproc);
    int id=0;

    // lancio in parallelo
    # pragma omp parallel default(shared) private(id)
    {
        id = (int)omp_get_thread_num();

        Solution* tmp= new Solution();
        tmp = initial_solution->copy_solution();
        array_sol[id] = tmp;
           

        std::string str_id = std::to_string(id);
        
        //double best_sol;
        best_sol[id] = sa(array_sol[id], start, timelimit, n_exams, n_timeslot,"./instances/"+current_instance+"_"+str_id+"_"+".sol");

        //array_sol[id]->double_obj=best_sol;
        

    }
    // seleziono miglior soluzione tra quelle feasible che ho trovato
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
       std::cout << "Solution of core " << i+1 << " has a score of " << best_sol[i]->double_obj << std::endl;
       avg+=best_sol[i]->double_obj;
    }

    // -----------------------------------------------------------------
    
    best_sol[index_best]->write_output_file("./instances/"+current_instance+".sol");
    std::cout<<"Best solution:\t\t"<<best_sol[index_best]->double_obj<<std::endl;
    std::cout<<"Average solution:\t"<<avg/counter<<std::endl;
   
    return 0;
}

