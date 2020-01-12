#include "header.hpp"
#include <time.h>
#include <sys/timeb.h>
#include <cmath>
#include <random>

Solution* sa_v2(Solution* solution, struct timeb start, int timelimit,std::string current_instance, bool inception){
 
    struct timeb now;
    double prob = 0;
    double prob_random = 0;
    double obj_new;
    double t;
    double t0;
    double perc_improvement;
    double obj_local=0;
    double obj_old;
    bool un;
    bool res;
    bool unres;
    double rel_t;
    double best_sol;
    double obj_SA;
    int n_exams=solution->n_exams;
    int n_timeslot=solution->n_timeslot;
    // prealloco tutti i vettori
    std::vector<int> timeslot_pre_swap=std::vector<int>(n_exams);    
    std::vector<int> old_timeslot_solution=std::vector<int>(n_exams);
    std::vector<int> best_timeslot_solution=std::vector<int>(n_exams);    
    std::vector<size_t> order_for_local=std::vector<size_t>(n_exams);    
    std::vector <int> old_ts_pre_swap=std::vector<int>(n_exams);
    
    // per mutation_by_
    std::default_random_engine generator;
    std::uniform_real_distribution<double> distribution(0.0,1.0);

    //per mutation--------------------------------------------------
    std::vector<double> weight_for_exams = std::vector<double>();
    weight_for_exams.reserve(n_exams);
    //fine per mutation--------------------------------------------------

    solution->update_weights();

    //per mutation--------------------------------------------------
    for(auto u: solution->all_exams){
       weight_for_exams.push_back(u->weight_in_obj_fun);
    }
    //fine per mutation--------------------------------------------------
    
    obj_SA = solution->objective_function();
    if (inception)
        // std::cout<<"Initial Objective Function: "<<obj_SA<<std::endl;

    //solution->write_output_file(current_instance);

    //--------------------LOCAL SEARCH INIZIALE---------------------------
    order_for_local=sort_indexes(solution->num_neighbours_for_exams);
    localSearch(solution,order_for_local);
    obj_local=solution->objective_function();
    // std::cout<<obj_local<<std::endl;
    obj_old=obj_SA;
     
    perc_improvement=0.1;
    while((obj_old-obj_local)/obj_local>perc_improvement){
        obj_old=obj_local;
        localSearch(solution,order_for_local);
        obj_local=solution->objective_function();  
    }
    // std::cout<<"SA "<<obj_SA<<"local "<<obj_local<<std::endl;
    
    // ------------------CALCOLO T0--------------------------------
    t0=(obj_SA-obj_local)/0.3566749;
    t=t0;
    // std::cout<<"initial t0 "<<t0<<std::endl;
    // --------------------------------------------------

    best_sol = obj_old;
    ftime(&now); 
    Solution * best_solution=solution->copy_solution();

    //-------------------------SCRITTURA SU FILE
      
    int iter=0;
    obj_SA=obj_local;
    
    while((int)((now.time-start.time))<timelimit){        
        rel_t=t/t0;
        old_timeslot_solution=solution->timeslot_per_exams; 
        iter++;
        //--------------------------------------- RESCHEDULING----------------
        
        int num_unsched;
        if (inception){
            if (rel_t*0.3<0.1){
                num_unsched=round(n_exams*0.1);
            }else{
                num_unsched=round(n_exams*rel_t*0.3);
            }
        }
        else{
            if (rel_t*0.3<0.1){
                num_unsched=3;
            }else{
                num_unsched=4;
            }
        }
        
        
        unres=false;
        int first=2;
        while(!unres){
            if (first==0){        
                // DECIDERE CHE FARE QUA       
                solution->timeslot_per_exams=old_timeslot_solution;
                solution->update_timeslots();
                solution->update_weights();
            }
            un=unscheduling(solution, num_unsched);
            res=rescheduling(solution);
            unres=un||res;
            first=0;
        }
        
        solution->update_weights();
        
        //----------------------------------DETERMINISTIC SWAP-------------------------------
        if (inception){
            timeslot_pre_swap=solution->timeslot_per_exams;
            double obj_pre_swap=solution->objective_function();
            bool better;
            for(int k=0; k<n_timeslot-1; k++){
                for(int q=k+1; q<n_timeslot; q++){
                    better = neighbours_by_swapping_single(solution, k, q, obj_pre_swap);
                    if(better==false){
                        // DECIDERE CHE FARE QUA
                        solution->timeslot_per_exams= timeslot_pre_swap;
                        solution->update_timeslots();
                        solution->update_weights();
                    }else{
                        obj_pre_swap=solution->objective_function();
                        timeslot_pre_swap=solution->timeslot_per_exams;
                    }
                }
            }
            solution->update_weights();
        }
        
        //----------------------------------RANDOM SWAP-------------------------------
        /*
        neighbours_by_swapping(solution);
        solution->update_weights();
    */
        //--------------------------MUTATION-------------------------------------------------
        /*
        //da inizializzare in SA se usiamo questa funzione
        std::vector<size_t> order_for_mutation=std::vector<size_t>(n_exams);
        order_for_mutation=sort_indexes(weight_for_exams);
        int num_mutation = 1;
        double perc = 1;
        neighbours_by_mutation(solution, order_for_mutation, num_mutation, perc);
        solution->update_weights();
        */
        
        //--------------------------LOCAL SEARCH-------------------------------------------------
        perc_improvement=0.1*rel_t; 
        //bool first_local=true;      
        obj_old=solution->objective_function();
        localSearch(solution, order_for_local);
        obj_local=solution->objective_function();


        while((obj_old-obj_local)/obj_local>perc_improvement){
      
            obj_old=obj_local;
            if (inception){
                ftime(&now); 
                solution = sa_v2(solution, now, 1, current_instance, false);
            }
            else{
                localSearch(solution,order_for_local);
            }
            obj_local=solution->objective_function();
        
        }
        
        obj_new=obj_local;

        //-------------------------SA--------------------------------------------------------

        if(obj_new > obj_SA){
            prob=(sqrt(obj_new),sqrt(obj_SA), t);
            prob_random = distribution(generator);
            if (prob_random > prob){
                solution->timeslot_per_exams=old_timeslot_solution;
                solution->update_timeslots();
                solution->update_weights();
                solution->objective_function();
            }else{
                obj_SA=obj_new;  
            }
        }else{
            obj_SA=obj_new; 
   
        }

        if (obj_new < best_sol){
            best_sol = obj_new;
            best_solution->timeslot_per_exams=solution->timeslot_per_exams;
            best_solution->update_timeslots();
            solution->update_weights();
            best_solution->objective_function();
        }
        
        t = cooling(timelimit, now.time-start.time,t0);
        //output_file<<"temperature "<<t<<" OF OLD "<<obj_SA<<"\n";

    ftime(&now); 
    } 
    //output_file.close();               
    //std::cout<<"Best sol "<<best_sol<<std::endl;
    //std::cout<<iter<<std::endl;
    return best_solution;  
}


