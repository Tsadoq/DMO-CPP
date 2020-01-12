#include "header.hpp"
#include <time.h>
#include <sys/timeb.h>
#include <cmath>
#include <random>


double probability(double obj_new, double obj_old, double temperature);
double cooling(double time_limit, double current_time,double t0);
int num_mutation_changer(int num_mutation_actual, double &perc, double improvement,double best_improvement,bool first,int n_exams);
double temperature_shock(double temperature);



Solution* func_local_search(Solution* solution, double perc_improvement)
{   
    double obj_SA = solution->double_obj;
    int n_exams=solution->n_exams;
    double obj_local=0;
    std::vector<size_t> order_for_local=std::vector<size_t>(n_exams);    

    order_for_local=sort_indexes(solution->num_neighbours_for_exams);
    localSearch(solution,order_for_local);
    obj_local=solution->objective_function();
    double obj_old=obj_SA;
    
    while((obj_old-obj_local)/obj_local>perc_improvement){
        obj_old=obj_local;
        localSearch(solution,order_for_local);
        obj_local=solution->objective_function();
        solution->double_obj = obj_local;
    }
    
    return solution;
}


Solution* func_rescheduling(Solution* solution, std::vector<int> old_timeslot_solution, double rel_t, int inception)
{   
    int n_exams=solution->n_exams;

    double obj_local=0;
    
    bool un;
    bool res;
    bool unres;

    
    double obj_SA = solution->double_obj;

    
    int num_unsched;

    switch (inception)
    {
    case 0:
        if (rel_t*0.3<0.1){
            num_unsched=round(n_exams*0.1);
        }else{
            num_unsched=round(n_exams*rel_t*0.3);
        }
        break;
    case 1:
       
        if (rel_t*0.3<0.1){
            num_unsched=round(n_exams*0.1);
        }else{
            num_unsched=round(n_exams*rel_t*0.3);
        }
    case 2:
        if (rel_t*0.3<0.1){
            num_unsched=3;
        }else{
            num_unsched=4;
        }
    default:
        if (rel_t*0.3<0.1){
            num_unsched=round(n_exams*0.1);
        }else{
            num_unsched=round(n_exams*rel_t*0.3);
        }
    }

    

    // ------------------------------------------------------------
    
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

    return solution;
}

Solution* func_mutation(Solution* solution)
{   
    int n_exams=solution->n_exams;
    std::vector<double> weight_for_exams = std::vector<double>();

    //per mutation--------------------------------------------------
    for(auto u: solution->all_exams){
       weight_for_exams.push_back(u->weight_in_obj_fun);
    }

    weight_for_exams.reserve(n_exams);
    //fine per mutation--------------------------------------------------

    //da inizializzare in SA se usiamo questa funzione
    std::vector<size_t> order_for_mutation=std::vector<size_t>(n_exams);
    order_for_mutation=sort_indexes(weight_for_exams);
    int num_mutation = 1;
    double perc = 1;
    neighbours_by_mutation(solution, order_for_mutation, num_mutation, perc);
    solution->update_weights();
    
    return solution;
}


Solution* func_swap_deterministic(Solution* solution, std::vector<int> timeslot_pre_swap, int inception)
{   
    int iii;
    
    switch (inception)
    {
        case 0:
        {
            iii =0;
        }
        case 1:
        {
            double n_timeslot = solution->n_timeslot;
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
        } break;
        case 2:
        {
            iii = 0;
            break;
        }
    }
    

    return solution;
}



Solution* func_swap_random(Solution* solution)
{   
    neighbours_by_swapping(solution);
    solution->update_weights();

    return solution;
}




Solution* func_alberto(Solution* solution, std::vector<int> timeslot_pre_swap, 
        std::vector<int> old_timeslot_solution, double rel_t, double perc_improvement, 
        int inception, struct timeb now, std::string current_instance)
{   

    solution = func_rescheduling(solution, old_timeslot_solution, rel_t, inception);
    solution = func_swap_deterministic(solution, timeslot_pre_swap, inception);
    solution = func_local_search(solution, perc_improvement);
    
    if (inception==1){
        ftime(&now);
        inception=2;
        solution = sa(solution, now, 1, current_instance, inception);
        // devo resettare inception da qualche parte?????? dopo sa interno???  DOMANDA!!!!!!!!!
        inception = 0; // controola qua
    }
    
    if (inception==2){
        solution = func_local_search(solution, perc_improvement);
    }

    return solution;
}



int choose_function(double rel_t, int iter, double perc_improvement, int inception)
{   
    // CAMBIA PARAMETRO SE AGGIUNGI O TOGLI FUNZIONI!!!
    int num_func = 6;
    int idx_func;
    
    // 0    -> rescheduling
    // 1    -> swap deterministic
    // 2    -> swap random
    // 3    -> mutation
    // 4    -> local search greedy
    // 5    -> func alberto


    if (perc_improvement == 0){
        idx_func = 3;
    } else {
        // solo alberto
        idx_func = num_func-1;
    }


    // quando uso func_alberto ho rel_t = -nan
    std::cout << "rel_t:\t" << rel_t << std::endl;

    // idx_func = rand() % num_func;

    std::cout << "chosen:\t" << idx_func << std::endl;
    
    return idx_func;
}


Solution* get_new_solution(int idx , Solution* solution, std::vector<int> timeslot_pre_swap,
         std::vector<int> old_timeslot_solution, double rel_t, double perc_improvement, 
         int inception, struct timeb now, std::string current_instance)
{   

    switch (idx)
        {
        case 0:     
            solution = func_rescheduling(solution, old_timeslot_solution, rel_t, inception);
            break;
        case 1:
            solution = func_swap_deterministic(solution, timeslot_pre_swap, inception);
            break;
        case 2:
            solution = func_swap_random(solution);
            break;
        case 3:
            solution = func_mutation(solution);
            break;
        case 4:
            solution = func_local_search(solution, perc_improvement);
            break;
        case 5:
            {
            // la prima volta inception = 0 (guarda main.cpp)
            inception += 1;
            solution = func_alberto(solution, timeslot_pre_swap, old_timeslot_solution, rel_t,perc_improvement, 
            inception, now, current_instance);
            } break;
        default:
            break;
        }

    return solution;
}



Solution* sa(Solution* solution, struct timeb start, int timelimit,std::string current_instance, int inception){
 
    struct timeb now;
    double prob = 0;
    double prob_random = 0;
    double obj_new;
    double t;
    double t0;
    double perc_improvement;
    double obj_local=0;
    double obj_old;

    double rel_t;
    double best_sol;
    double obj_SA;
    int n_exams=solution->n_exams;
    int n_timeslot=solution->n_timeslot;
    
    // prealloco tutti i vettori
    std::vector<int> timeslot_pre_swap=std::vector<int>(n_exams);    
    std::vector<int> old_timeslot_solution=std::vector<int>(n_exams);
    std::vector<int> best_timeslot_solution=std::vector<int>(n_exams);       
    std::vector <int> old_ts_pre_swap=std::vector<int>(n_exams);
    
    // per mutation_by_
    std::default_random_engine generator;
    std::uniform_real_distribution<double> distribution(0.0,1.0);

    // TODO: what if inception==1?

    solution->update_weights();
    obj_SA = solution->objective_function();
    std::cout<<"Initial Objective Function: "<<obj_SA<<std::endl;

    //--------------------LOCAL SEARCH INIZIALE---------------------------
    perc_improvement = 0.1;
    solution = func_local_search(solution, perc_improvement);
    obj_local = solution->double_obj;
    
    // ------------------CALCOLO T0--------------------------------
    
    t0=(obj_SA-obj_local)/0.693747281;
    t=t0;
    std::cout<<"initial t0 "<<t0<<std::endl;
    // --------------------------------------------------

    best_sol = obj_SA;
    Solution * best_solution=solution->copy_solution();
      
    int iter=0;
    obj_SA=obj_local;

    ftime(&now);
    while((int)((now.time-start.time))<timelimit){      

        if (t0!=0){
            rel_t=t/t0;
        } else {

            // perchè t0=(obj_SA-obj_local)/0.693747281;
            // ma obj_SA=obj_locals
            rel_t = 0;
        }
        
        iter++;
        perc_improvement = 0.1*rel_t;

        old_timeslot_solution=solution->timeslot_per_exams; 

        int idx = choose_function(rel_t, iter, perc_improvement, inception);
        
        solution = get_new_solution(idx , solution, timeslot_pre_swap, old_timeslot_solution, rel_t, perc_improvement, inception, now, current_instance);
    
        //-------------------------SA--------------------------------------------------------

        obj_new = solution->double_obj;


        if(obj_new > obj_SA){
            prob=(obj_new,obj_SA, t);
            prob_random = distribution(generator);
            if (prob_random > prob){
                // DECIDERE CHE FARE QUA
                solution->timeslot_per_exams=old_timeslot_solution;
                solution->update_timeslots();
                solution->update_weights();
            }else{
                obj_SA=obj_new;  
            }
        }else{
            obj_SA=obj_new; 
   
        }

        if (obj_new <= best_sol){
            best_sol = obj_new;
            best_solution->timeslot_per_exams=solution->timeslot_per_exams;
            best_solution->update_timeslots();
            solution->update_weights();
            best_solution->write_output_file(current_instance);
        }
        
        t = cooling(timelimit, now.time-start.time,t0);
        //output_file<<"temperature "<<t<<" OF OLD "<<obj_SA<<"\n";

    ftime(&now); 
    } 
    //output_file.close();               
    //std::cout<<"Best sol "<<best_sol<<std::endl;
    best_solution->double_obj=best_sol;
    //std::cout<<iter<<std::endl;
    return best_solution;  
}

double probability(double obj_new, double obj_old, double temperature)
{
    double e=2.71828183;
    double p = pow(e, -((obj_new-obj_old)/temperature)); // p = exp^(-(F(x_new)-F(x_old))/T)
    return p;
}

double cooling(double time_limit, double current_time,double t0)
{
    double coeff=(time_limit-current_time)/time_limit;
    return  t0*coeff;
}
