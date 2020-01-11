#include "header.hpp"
#include <time.h>
#include <sys/timeb.h>
#include <cmath>
#include <random>

using namespace std;

double probability(double obj_new, double obj_old, double temperature);
//double probability(double obj_new, double obj_old, double temperature, double maxtemperature);
//double cooling(double temperature, double coeff);
double cooling(double time_limit, double current_time,double t0);
int num_mutation_changer(int num_mutation_actual, double &perc, double improvement,double best_improvement,bool first,int n_exams);
double temperature_shock(double temperature);

Solution* sa(Solution* solution, struct timeb start, int timelimit, int n_exams, int total_number_students, int n_timeslot,string current_instance,double t0, double alpha, int mutations, double cooling_coefficient){
 
    struct timeb now;
    double prob = 0;
    double prob_random = 0;
    double obj_new;
    vector<int> old_timeslot_solution;
    vector<int> best_timeslot_solution;
    double t;
    double cooling_coeff = cooling_coefficient; //def = 0.8
    double perc_improvement;
    double obj_local=0;
    double obj_old;
    bool un;
    bool res;
    bool unres;
    double rel_t;
    vector<size_t> order_for_local;    
    vector <int> old_ts_pre_swap;
    double best_sol;
    double obj_SA;
    vector<int> timeslot_pre_swap;

    std::default_random_engine generator;
    std::uniform_real_distribution<double> distribution(0.0,1.0);

    vector<double> weight_for_exams=solution->update_weights(n_exams);
    obj_SA = solution->objective_function(n_exams,total_number_students);
    cout<<"Initial Objective Function: "<<obj_SA<<endl;

    

    solution->write_output_file(current_instance, n_exams);
   
    vector<int> possible_timeslots;
    for (int i=0; i<n_timeslot;i++){
        possible_timeslots.push_back(i+1);
    }

    //--------------------LOCAL SEARCH INIZIALE---------------------------
    order_for_local=sort_indexes(solution->num_neighbours_for_exams);

    localSearch(solution, n_exams, possible_timeslots,order_for_local);
    obj_local=solution->objective_function(n_exams,total_number_students);
    cout<<obj_SA<<"\n";
    obj_old=obj_SA;
    cout<<obj_SA<<"\n";
  
    perc_improvement=0.1;
    while((obj_old-obj_local)/obj_local>perc_improvement){
        obj_old=obj_local;
        localSearch(solution, n_exams, possible_timeslots,order_for_local);
        obj_local=solution->objective_function(n_exams,total_number_students);  
    }
    cout<<"SA "<<obj_SA<<"local "<<obj_local<<endl;

    // ------------------CALCOLO T0--------------------------------
    t0=(obj_SA-obj_local)/0.693747281;
    t=t0;
    cout<<"initial t0 "<<t0<<endl;
    // --------------------------------------------------

    best_sol = obj_old;

    ftime(&now); 
    Solution * best_solution=solution->copy_solution(n_exams);

    //-------------------------SCRITTURA SU FILE
  
    string file_out="temperaturaVALE1";
    ofstream output_file;
    output_file.open(file_out);

    
    obj_SA=obj_local;

    
    while((int)((now.time-start.time))<timelimit){        
        rel_t=t/t0;
        old_timeslot_solution=solution->timeslot_per_exams; 
        

        //--------------------------------------- RESCHEDULING----------------
        
        int num_unsched;
        if (rel_t*0.3<0.1){
            num_unsched=round(n_exams*0.1);
        }else{
            num_unsched=round(n_exams*rel_t*0.3);
        }
        
        unres=false;
        int first=2;
        while(!unres){
            if (first==0){               
                solution->timeslot_per_exams=old_timeslot_solution;
                solution->update_timeslots(n_exams);
                solution->update_weights(n_exams);
            }
            un=unscheduling(solution, num_unsched);
            res=rescheduling(solution, n_timeslot, n_exams);
            unres=un||res;
            first=0;
        }
        
        
        weight_for_exams=solution->update_weights(n_exams);
        
        //----------------------------------SWAPs-------------------------------
        timeslot_pre_swap=solution->timeslot_per_exams;
        double obj_pre_swap=solution->objective_function(n_exams, total_number_students);
        bool better;
        for(int k=0; k<n_timeslot-1; k++){
            for(int q=k+1; q<n_timeslot; q++){
                better = neighbours_by_swapping_single(solution, k, q, obj_pre_swap,total_number_students);
                if(better==false){
                    solution->timeslot_per_exams= timeslot_pre_swap;
                    solution->update_timeslots(n_exams);
                    solution->update_weights(n_exams);
                }else{
                    obj_pre_swap=solution->objective_function(n_exams, total_number_students);
                    timeslot_pre_swap=solution->timeslot_per_exams;
                }
            }
        }
        
        //--------------------------LOCAL SEARCH-------------------------------------------------
        perc_improvement=0.1*rel_t; 
        //bool first_local=true;      
        obj_old=solution->objective_function(n_exams,total_number_students);
        localSearch(solution, n_exams, possible_timeslots,order_for_local);
        obj_local=solution->objective_function(n_exams,total_number_students);


        while((obj_old-obj_local)/obj_local>perc_improvement){
      
            obj_old=obj_local;
            localSearch(solution, n_exams, possible_timeslots,order_for_local);
            obj_local=solution->objective_function(n_exams,total_number_students);
        
        }
        
        obj_new=obj_local;

        //-------------------------SA--------------------------------------------------------

        if(obj_new > obj_SA){
            prob=(obj_new,obj_SA, t);
            prob_random = distribution(generator);
            if (prob_random > prob){
                solution->timeslot_per_exams=old_timeslot_solution;
                solution->update_timeslots(n_exams);
                weight_for_exams=solution->update_weights(n_exams);
            }else{
                obj_SA=obj_new;  
            }
        }else{
            obj_SA=obj_new; 
   
        }

        if (obj_new <= best_sol){
            best_sol = obj_new;
            best_solution->timeslot_per_exams=solution->timeslot_per_exams;
            best_solution->update_timeslots(n_exams);
            weight_for_exams=solution->update_weights(n_exams);
            best_solution->write_output_file(current_instance, n_exams);
        }
        
        t = cooling(timelimit, now.time-start.time,t0);
        output_file<<"temperature "<<t<<" OF OLD "<<obj_SA<<"\n";

    ftime(&now); 
    } 
    //output_file.close();               
    cout<<"Best sol "<<best_sol<<endl;
    best_solution->double_obj=best_sol;
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
