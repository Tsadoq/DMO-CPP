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
#include "sa_v1.cpp"
#include "sa_v2.cpp"
#include "sa_functions.cpp"
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
    int numproc = 4;
    int timelimit = 0;
    std::string current_instance=argv[1];

    if(argc >= 4){
        if (argc == 5){
            numproc = atoi(argv[4]);
        }
        std::string time_flag = argv[2];
        if (time_flag != "-t") {
            std::cerr << "Second argument must flag -t" << std::endl;
            return 1;
        }

        std::string time = argv[3];
        try {
            timelimit = std::stoi(time);
            std::cout << "TIMELIMIT SET" <<std::endl;
        } catch (const std::exception& e) { 
            std::cerr << "Third argument must be an integer" << std::endl;
            return 1;
        }
    } else {
        std::cerr << "Wrong number of arguments.\nProgram must be used as such: ./main instance_name -t (int)timelimit" << std::endl;
        return 1;
    }

    ftime(&start);
    std::cout<<"Starting"<<std::endl;

    // std::string current_instance=argv[1];
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
    // int timelimit = atoi(argv[2]);
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
    
    std::cout<<"prima"<<std::endl;
    initial_solution->solution_update(conflict_matrix,n_exams, total_number_students,n_timeslot);
    std::cout<<"dopo"<<std::endl;

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

        //-------------------------------- VARIABLE VALUES INITIALIZATION --------------------------------------------
        // sort exams by decreasing value of number of neighbours
        std::vector<std::vector<size_t>> sorted_index=std::vector<std::vector<size_t>>(n_exams);
        // it's a vector of indexes: values in [0,n_exams-1]       
        sorted_index[id]=sort_indexes(array_sol[id]->num_neighbours_for_exams);
        
        //-----------------------------------------------------------------------------------------
        // apply a variant of greedy coloring trying to assign timeslots first to exams with higher degree
        alternativeColoring(array_sol[id],sorted_index[id]);
        //-----------------------------------------------------------------------------------------

        array_sol[id]->update_timeslots();
        int flag =  array_sol[id]->check_feasibility( array_sol[id]->timeslot_per_exams,  array_sol[id]->all_exams);
        std::cout<<"feasibility "<<flag<<std::endl;

        std::string str_id = std::to_string(id);
        
        //double best_sol;
        double cooling_coeff=0.9;
        best_sol[id] = sa_v1(array_sol[id], start, timelimit,"./instances/"+current_instance+"_"+str_id+"_"+".sol",cooling_coeff);

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

