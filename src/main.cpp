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
//#include "sa.cpp"
#include "sa_random.cpp"
#include"TSforInitialSolution.cpp"
#include"temperature_init.cpp"
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
#include <ctime>
#include <cstdlib>

#include <omp.h>

// GUIDA per settare vscode e c++ da linux
// https://code.visualstudio.com/docs/cpp/config-wsl

//int read_file_stu(char *name_stu);
int main(int argc, char **argv) {
    srand(time(NULL));
    struct timeb start;
    int n_exams = 0;
    int n_timeslot = 0;
    int numproc = atoi(argv[3]);
    
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
    
    //vector<int> old_timeslot_solution=initial_solution->timeslot_per_exams;

    // -----------------------------------------------------------------
    //TEMPERATURA INIZIALE
    // -----------------------------------------------------------------
    double t0 = 30;//temperature_init(initial_solution,n_exams,total_number_students,n_timeslot);
    cout<<"Temp init: "<<t0<<endl;

    //initial_solution->timeslot_per_exams=old_timeslot_solution;
    //initial_solution->update_timeslots(n_exams);
    //vector<double>weight_for_exams=initial_solution->update_weights(n_exams);
    // -----------------------------------------------------------------

    omp_set_dynamic(0);     // Explicitly disable dynamic teams
    omp_set_num_threads(numproc); // Use 4 threads for all consecutive parallel regions
 
    int mysize;
    mysize = omp_get_num_threads();
    cout << "  Number of processors available = " << omp_get_num_procs() << "\n";
    cout << "  Number of threads =              " << omp_get_max_threads() << "\n";

    // qui dentro tutti i metodi...
    

    // // iniziallizzo parametri
    // for(int i=0; i<numproc; i++){
        
    //     array[i].set_TS()


    //     if (i==0): //solo TS
    //         array[i].set_TS()
    //     if (i==1): //solo SA
    //     if (i==2): //solo TS+SA
    //     ...
    // }
    vector<Solution*> array_sol = vector<Solution*>(numproc);
    int id=0;
    int sa_flag = 0; //sa normale mentre pari a 1 sa modificato per random totale
     //if (1==1)

    // lancio in parallelo
    # pragma omp parallel private ( id ) //shared(initial_solution)
    {
        id = omp_get_thread_num();
        if(id==0){
            Solution* tmp=new Solution(); 
            sa_flag = 1;
            array_sol[0] = tmp;
            array_sol[0]->timeslot_per_exams= vector<int>(n_exams,-1);
            array_sol[0]->solution_update(conflict_matrix, n_exams);
            for(int i=0;i<n_exams;i++){
                array_sol[0]->timeslot_per_exams[i]=rand() % n_timeslot +1;        
            }             
            array_sol[0]->update_timeslots(n_exams);
            double f = sa_random(array_sol[0], start,  timelimit,  n_exams,  total_number_students,  n_timeslot,  current_instance, t0);
        }
        /*
        if(id==0){
            Solution* tmp=new Solution(); 
            array_sol[0] = tmp;
            array_sol[0]->timeslot_per_exams= vector<int>(n_exams,-1);
            array_sol[0]->solution_update(conflict_matrix, n_exams);
            for(int i=0;i<n_exams;i++){
                array_sol[0]->timeslot_per_exams[i]=rand() % n_timeslot +1;        
            }             
            
            array_sol[0]->update_timeslots(n_exams);
            for(int i=0; i<n_exams; i++){
                for(auto j:  array_sol[0]->all_exams[i]->conflict_times){
                    if(j== array_sol[0]->timeslot_per_exams[i]){
                        array_sol[0]->timeslot_per_exams[i]=-2;
                    }
                }
            }
            int flag = array_sol[0]->check_feasibility(array_sol[0]->timeslot_per_exams, array_sol[0]->all_exams);
            TSforInitialSolution* TS=new TSforInitialSolution();
            TS->dim=n_timeslot;
            TS->maxIter=10000000;
            int ts=TS->tabu_search(array_sol[0],n_exams,n_timeslot,conflict_matrix);
            cout<<ts<<endl;
            //initial_solution->write_output_file(current_instance, n_exams);
            flag = array_sol[0]->check_feasibility(array_sol[0]->timeslot_per_exams, array_sol[0]->all_exams);
            // AGGIUNGERE TS O QUALCOS'ALTRO PER ASSICURARSI CHE LA SOLUZIONE INIZIALE SIA FEASIBLE
            // O GESTIRE LE PENALITA' NELLA OBJ FUNCTION PER CONVERGERE ALLA FEASIBILITY
            cout<<"Flag: "<<flag<<endl;
        }
        if(id==1){
            Solution* tmp=new Solution(); 
            array_sol[1] = tmp;   
            array_sol[1]->timeslot_per_exams= vector<int>(n_exams,-1);
            array_sol[1]->solution_update(conflict_matrix, n_exams);
            //fprintf(stdout, "%d\n", initial_solution->num_neighbours_for_exams[1]);
            // sort exams by decreasing value of number of neighbours
            vector<size_t> sorted_index=vector<size_t>(n_exams);
            // it's a vector of indexes: values in [0,n_exams-1]       
            sorted_index=sort_indexes(array_sol[1]->num_neighbours_for_exams);
            // apply greedy coloring trying to assign timeslots first to exams with higher degree
            array_sol[1]->timeslot_per_exams= vector<int>(n_exams,-1);

            graph_coloring_greedy(array_sol[1], n_timeslot, sorted_index, n_exams); 
            array_sol[1]->update_timeslots(n_exams);
            int flag = array_sol[1]->check_feasibility(array_sol[1]->timeslot_per_exams, array_sol[id]->all_exams);
            TSforInitialSolution* TS=new TSforInitialSolution();
            TS->dim=n_timeslot;
            TS->maxIter=10000000;
            int ts=TS->tabu_search(array_sol[1],n_exams,n_timeslot,conflict_matrix);
            cout<<ts<<endl;
            //initial_solution->write_output_file(current_instance, n_exams);
            flag = array_sol[1]->check_feasibility(array_sol[1]->timeslot_per_exams, array_sol[id]->all_exams);
            // AGGIUNGERE TS O QUALCOS'ALTRO PER ASSICURARSI CHE LA SOLUZIONE INIZIALE SIA FEASIBLE
            // O GESTIRE LE PENALITA' NELLA OBJ FUNCTION PER CONVERGERE ALLA FEASIBILITY
            cout<<"Flag: "<<flag<<endl;
        }*/
        
        //Solution *initial_solution = new Solution();
        //initial_solution = array[id];
        
        /*int i=0;
        while(flag==1 && i<1000){
            unsigned seed = std::chrono::system_clock::now().time_since_epoch().count();    
            std::shuffle(sorted_index.begin(), sorted_index.end(),std::default_random_engine(seed) );
            graph_coloring_greedy(initial_solution, n_timeslot, sorted_index, n_exams); 
            initial_solution->update_timeslots(n_exams);
            i++;
        }*/

        
        
        //PROVIAMO A MODIFICARE LA SOLUZIONE INIZIALE DI TANTO PRIMA DI LANCIARE
        /*initial_solution->update_timeslots(n_exams);
        vector<double> weight_for_exams=initial_solution->update_weights(n_exams);
        vector<int> possible_timeslots;
        for (int i=0; i<n_timeslot;i++){
            possible_timeslots.push_back(i+1);
        }*/
    
        /*
        string str_id = to_string(id);
        
        double best_sol;
        best_sol = sa(array_sol[id], start, timelimit, n_exams, total_number_students, n_timeslot,"./instances/"+current_instance+"_"+str_id+"_"+".sol",t0);

        array_sol[id]->double_obj=best_sol;
        */
    }
 
    // seleziono miglior soluzione tra quelle feasible che ho trovato

    double min = array_sol[0]->double_obj;
    int index_best = 0;
    for(int i=0; i<numproc; i++){
       // prendo il minimo
       Solution *tmp = array_sol[i];
       double tmp_obj = tmp->double_obj;
       if (tmp_obj < min){
           index_best = i;
       }
       
    }

    // -----------------------------------------------------------------
    
    array_sol[index_best]->write_output_file("./instances/"+current_instance+".sol", n_exams);

   
    return 0;
}

