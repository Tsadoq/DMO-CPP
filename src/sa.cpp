#include "header.hpp"
#include "TabuSearch.hpp"
#include "TabuSearch.cpp"
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

    std::default_random_engine generator;
    std::uniform_real_distribution<double> distribution(0.0,1.0);

    vector<double> weight_for_exams=solution->update_weights(n_exams);
    double obj_SA = solution->objective_function(n_exams,total_number_students);
    cout<<"Initial Objective Function: "<<obj_SA<<endl;

    vector<size_t> order_for_local;

    //riscrivo il file output -> da fare
    solution->write_output_file(current_instance, n_exams);
   
    vector<int> possible_timeslots;
    for (int i=0; i<n_timeslot;i++){
        possible_timeslots.push_back(i+1);
    }

    //--------------------LOCAL SEARCH---------------------------
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
    double perc=1.0;
    int count_iter=0;
    
    double best_sol = obj_old;
    double worst_sol = obj_old;

    ftime(&now); 
    Solution * best_solution=solution->copy_solution(n_exams);
  
    string file_out="temperaturaVALE1";
    ofstream output_file;
    output_file.open(file_out);

    double rel_t;
    obj_SA=obj_local;

    int uguali=0;
    int uguali_swap=0;
    vector <int> old_ts_pre_swap;
    
    while((int)((now.time-start.time))<timelimit){        
        rel_t=t/t0;
        old_timeslot_solution=solution->timeslot_per_exams; 
        
        uguali=0;

        //--------------------------------------- RESCHEDULING----------------
        
        int num_unsched;
        if (rel_t*0.2<0.1){
            num_unsched=round(n_exams*0.1);
        }else{
            num_unsched=round(n_exams*rel_t*0.2);
        }
        //cout<<"rel t: "<<rel_t<<" un "<<num_unsched<<endl;
        unres=false;
        int first=2;
        while(!unres){
            //cout<<!unres<<endl;
            if (first==0){               
                solution->timeslot_per_exams=old_timeslot_solution;
                solution->update_timeslots(n_exams);
                solution->update_weights(n_exams);
                //cout<<"ciao"<<endl;
            }
            un=unscheduling(solution, num_unsched);
            res=rescheduling(solution, n_timeslot, n_exams);
            unres=un||res;
            first=0;
            cout<<"jjjjjjjjjjj"<<un<<" "<<res<<" "<<unres<<endl;
        }
        cout<<"uscitaaaaaaaaaaaaaaaaaaaaaaaaaaa"<<endl;
        
        
        weight_for_exams=solution->update_weights(n_exams);

        /*vector<size_t> sorted_index = vector<size_t>(n_exams);
        vector<int> tmp = vector<int>(n_exams);
        for(int yy=0; yy<n_exams; yy++){
            if(solution->all_exams[yy]->timeslot == -1){
                tmp.push_back(solution->num_neighbours_for_exams[yy]);
            }
        }
        
        sorted_index = sort_indexes(tmp);*/
       /* sorted_index = sort_indexes(solution->num_neighbours_for_exams);
        int ee = alternativeColoring(solution,  n_timeslot,  n_exams, sorted_index);
        //neighbours_by_mutation_no_order(solution,num_unsched,possible_timeslots,n_exams);
        solution->update_weights(n_exams);
*/
       /* while(rescheduled==0 && counter_unsched < 40){
            //cout<<"ddddddd"<<endl;
            unscheduling(solution, num_unsched);
            //cout<<"Unscheduling done"<<endl;
            rescheduled = rescheduling(solution, n_timeslot, old_timeslot_solution);
            
            if (num_unsched != rescheduled){
                solution->timeslot_per_exams = old_timeslot_solution;
                solution->update_timeslots(solution->timeslot_per_exams.size());
                cout<<"Non sono riuscito a rimettere tutti gli esami, ma solo "<<rescheduled<<endl;
                rescheduled = 0;
            }
            if (num_unsched == rescheduled){
                //cout<<"I have rescheduled all exams!"<<endl;
            }
            counter_unsched++;
        }
        weight_for_exams=solution->update_weights(n_exams);  */  
        //obj_new=solution->objective_function(n_exams,total_number_students);
        
        
        for(int hh=0;hh<n_exams;hh++){
            if (old_timeslot_solution[hh]==solution->timeslot_per_exams[hh]){
                uguali++;
            }
        }

        //cout<<"ts uguali "<<uguali<<endl;
        //----------------------------------SWAPs-------------------------------
        vector<int> timeslot_pre_swap=solution->timeslot_per_exams;
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
        perc_improvement=0.3*rel_t; 
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
        //cout<<"------------------------------------end local-----------------------------------"<<endl;
        //cout<<"new"<<obj_new<<endl;

        if(obj_new > obj_SA){
            prob=(obj_new,obj_SA, t);
            prob_random = distribution(generator);
            if (prob_random > prob){
                //rifiuto, quindi risistemo la soluzione
                solution->timeslot_per_exams=old_timeslot_solution;
                solution->update_timeslots(n_exams);
                weight_for_exams=solution->update_weights(n_exams);
                //cout<<"rfiuto"<<endl;  
            }else{
                //solution->write_output_file(current_instance, n_exams);
                obj_SA=obj_new; 
                //cout<<"accetto peggioramento"<<endl;  
            }
        }else{
            //solution->write_output_file(current_instance, n_exams);
            obj_SA=obj_new;
            //cout<<"oooo"<<obj_SA<<endl;
            //cout<<"accetto bello"<<endl;  
   
        }

        if (obj_new <= best_sol){
            best_sol = obj_new;
            best_timeslot_solution=solution->timeslot_per_exams;
            best_solution->timeslot_per_exams=solution->timeslot_per_exams;
            best_solution->update_timeslots(n_exams);
            weight_for_exams=solution->update_weights(n_exams);
            best_solution->write_output_file(current_instance, n_exams);
        }
        if (obj_new > worst_sol){
            worst_sol = obj_new;
        }
        
        t = cooling(timelimit, now.time-start.time,t0);
        output_file<<"temperature "<<t<<" OF OLD "<<obj_SA<<"\n";

    ftime(&now); 
    } 
    output_file.close();               
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

/*double probability(double obj_new, double obj_old, double temperature, double maxtemperature)
{
    double e = 2.71828183;

    double bias = 0.5+(maxtemperature-temperature)/maxtemperature;
    double p = pow(e, -((obj_new*bias - obj_old) / temperature)); // p = exp^(-(F(x_new)-F(x_old))/T)

    return p;
}*/

/*double cooling(double temperature, double coeff)
{
    //temperature = 0.8*temperature;
    //temperature=temperature/(1+500*temperature);
    //temperature=1/((1/temperature)+0.001/2);
    return  temperature*coeff;
}*/

double cooling(double time_limit, double current_time,double t0)
{
    double coeff=(time_limit-current_time)/time_limit;
    //temperature = 0.8*temperature;
    //temperature=temperature/(1+500*temperature);
    //temperature=1/((1/temperature)+0.001/2);
    return  t0*coeff;
}

int num_mutation_changer(int num_mutation_actual, double &perc, double improvement,double best_improvement,bool big_change,int n_exams){
    int num_mutation_new;
    int available_num_mutation;
    if (big_change==true || improvement==0){
        perc=1.0;
        num_mutation_new=round(n_exams*0.03);
        //cout<<"true "<< num_mutation_new<<" "<<perc<<endl;
    }else{    
        perc = improvement/best_improvement;  
        //cout<<"improvement "<< improvement<<endl;       
        available_num_mutation=round(perc*n_exams);
        available_num_mutation=0.1*available_num_mutation+1;
        if (available_num_mutation-num_mutation_actual>0){            
            num_mutation_new =num_mutation_actual+ rand()%(available_num_mutation-num_mutation_actual)+1;
            //cout<<"false if "<< num_mutation_new<<" "<<perc<<endl;
        }else{
            num_mutation_new=rand()%available_num_mutation+1;
            //cout<<"false else "<< num_mutation_new<<" "<<perc<<endl;
        }        
        /*cout<< num_mutation_actual<<endl;*/
    }
    return num_mutation_new;
}

double temperature_shock(double temperature){
    return 0.7*temperature;
}