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
    //double t0 = 30;
    double obj_new;
    vector<int> old_timeslot_solution;
    vector<int> best_timeslot_solution;
    double a=alpha; //def = 0.5
    double t;
    double cooling_coeff = cooling_coefficient; //def = 0.8
    double perc_improvement;
    double obj_local=0;
    double old_SA;

    

    std::default_random_engine generator;
    std::uniform_real_distribution<double> distribution(0.0,1.0);

    vector<double> weight_for_exams=solution->update_weights(n_exams);
    double obj_old = solution->objective_function(n_exams,total_number_students);
    cout<<"Initial Objective Function: "<<obj_old<<endl;

    vector<size_t> order_for_local;

    //riscrivo il file output -> da fare
    solution->write_output_file(current_instance, n_exams);
    vector<size_t> order_for_mutation=vector<size_t>(n_exams);
    vector<int> possible_timeslots;
    for (int i=0; i<n_timeslot;i++){
        possible_timeslots.push_back(i+1);
    }

    //--------------------LOCAL SEARCH---------------------------
    order_for_local=sort_indexes(solution->num_neighbours_for_exams);

    old_SA=solution->objective_function(n_exams,total_number_students);
    localSearch(solution, n_exams, possible_timeslots,order_for_local);
    obj_local=solution->objective_function(n_exams,total_number_students);
    obj_old=old_SA;
    /*cout<<obj_old<<" old obj"<<endl;
        cout<<obj_local<<" local obj"<<endl;*/
    perc_improvement=0.1;
    while((obj_old-obj_local)/obj_local>perc_improvement){
        //cout<<obj_local<<"dentro"<<endl;
        obj_old=obj_local;
        localSearch(solution, n_exams, possible_timeslots,order_for_local);
        obj_local=solution->objective_function(n_exams,total_number_students);
        /*cout<<obj_old<<" old obj"<<endl;
        cout<<obj_local<<" local obj"<<endl;*/
    }

    
    cout<<"first local"<<obj_local<<endl;
/*
    while((obj_old-obj_local)/obj_local>=perc_improvement){
        localSearch(solution, n_exams, possible_timeslots,order_for_local);
        obj_local=solution->objective_function(n_exams,total_number_students);
        obj_old=obj_local;
    }
    
    cout<<"first local search "<<solution->objective_function(n_exams,total_number_students)<<endl;
    */


    // ------------------CALCOLO T0--------------------------------
    t0=(old_SA-obj_local)/0.693747281;
    t=t0;
    cout<<"initial t0 "<<t0<<endl;


    //obj_old=obj_local;
    //cout<<obj_old<<"wwwwwww"<<endl;
    //int num_mutation=floor(n_exams/40);
    int num_mutation=2; //def = 3
    double perc=1.0;
    int count_iter=0;
    // improvement current solution wrt worst solution
    double improvement=0;
    // improvement best solution wrt worst solution
    double best_improvement=0;
    double best_sol = obj_old;
    double worst_sol = obj_old;
    // set perc=1.0 and num_mutation=1 when a new solution is the best one or we are at the first iteration
    bool big_change=true;
    // number of temperature shock done
    int num_shock=0;
    ftime(&now); 
    Solution * best_solution=solution->copy_solution(n_exams);
    int count_local_minima=0;
    int count_swap=0;
    int change=0;
    int refuse=0;
    string file_out="temperaturaVALE1";
    ofstream output_file;
    output_file.open(file_out);
    double rel_t;
    old_SA=obj_local;
    
    while((int)((now.time-start.time))<timelimit){        
        //change=0;
        //count_iter++;
        //count_swap++;
        rel_t=t/t0;
        // sort exam wrt weigths in obj function
        //order_for_mutation=sort_indexes(weight_for_exams);
        // save old solution
        old_timeslot_solution=solution->timeslot_per_exams; 
        // find num_mutation and perc wrt the improvement in the current solution
        //num_mutation = num_mutation_changer(num_mutation, perc, improvement,best_improvement,big_change,n_exams);
        //big_change=false;
        // create a neighbour
        //vector<vector<int>>mutations_vector=neighbours_by_mutation(solution, order_for_mutation, round(n_exams*0.2), possible_timeslots,perc,n_exams);
        
        //--------------------------------------- RESCHEDULING----------------
        int rescheduled = 0;
        int counter_unsched = 0;
        int num_unsched;
        if (rel_t*0.3<0.1){
            num_unsched=round(n_exams*0.1);
        }else{
            num_unsched=rel_t*0.3;
        }

        while(num_unsched > rescheduled && counter_unsched < 40){
            unscheduling(solution, num_unsched);
            //cout<<"Unscheduling done"<<endl;
            rescheduled = rescheduling(solution, n_timeslot);
            if (num_unsched != rescheduled){
                solution->timeslot_per_exams = old_timeslot_solution;
                solution->update_timeslots(solution->timeslot_per_exams.size());
                //cout<<"Non sono riuscito a rimettere tutti gli esami, ma solo "<<rescheduled<<endl;
                rescheduled = 0;
            }
            if (num_unsched != rescheduled){
                //cout<<"I have rescheduled all exams!"<<endl;
            }
            counter_unsched++;
        }
        weight_for_exams=solution->update_weights(n_exams);    
        //obj_new=solution->objective_function(n_exams,total_number_students);

        //----------------------------------MUTATION-------------------------------
        //vector<vector<int>>mutations_vector=neighbours_by_mutation(solution, order_for_mutation, round(n_exams*0.3), possible_timeslots,1.0,n_exams);
        
        // update the weigths in the obj function after the mutation
        //weight_for_exams=solution->update_weights(n_exams);    
        //obj_new=solution->objective_function(n_exams,total_number_students);
        //cout<<"Mutation feas? "<< solution->check_feasibility(solution->timeslot_per_exams, solution->all_exams) <<endl; 
        //cout<<"Obj function pre swapping: "<<obj_new<<endl;
        //cout<<"Trying to do swapping"<<endl;   
        neighbours_by_swapping(solution, n_timeslot);
        weight_for_exams=solution->update_weights(n_exams); 
        

        //--------------------------LOCAL SEARCH-------------------------------------------------
        perc_improvement=0.3*rel_t; 
        //bool first_local=true;      
        obj_old=solution->objective_function(n_exams,total_number_students);
        localSearch(solution, n_exams, possible_timeslots,order_for_local);
        obj_local=solution->objective_function(n_exams,total_number_students);
        /*cout<<obj_old<<" old obj"<<endl;
        cout<<obj_local<<" local obj"<<endl;*/

        while((obj_old-obj_local)/obj_local>perc_improvement){
            //cout<<obj_local<<"dentro"<<endl;
            obj_old=obj_local;
            localSearch(solution, n_exams, possible_timeslots,order_for_local);
            obj_local=solution->objective_function(n_exams,total_number_students);
            /*cout<<obj_old<<" old obj"<<endl;
            cout<<obj_local<<" local obj"<<endl;*/
        }
        //cout<<(obj_old-obj_local)/obj_local<<" "<< 0.3*rel_t<<endl;     
        obj_new=obj_local;
        //cout<<"------------------------------------end local-----------------------------------"<<endl;
        
       /* neighbourhood_by_obj_fun(rand()%n_exams, solution, n_timeslot, possible_timeslots, n_exams);
        weight_for_exams=solution->update_weights(n_exams); 
        obj_new=solution->objective_function(n_exams,total_number_students); */

       /*for(int ii=0;ii<n_exams;ii++){
            if(old_timeslot_solution[ii]!=solution->timeslot_per_exams[ii]){
                change++;
            }
        }*/

        //cout<<change<<endl;
        //cout<<solution->check_feasibility(solution->timeslot_per_exams, solution->all_exams)<<endl;
        //cout<<"Swapping done with new obj"<<obj_new<<endl; 
        
        if(obj_new > old_SA){
            prob=(obj_new,old_SA, t);
            prob_random = distribution(generator);
            if (prob_random > prob){
                //rifiuto, quindi risistemo la soluzione
                solution->timeslot_per_exams=old_timeslot_solution;
                solution->update_timeslots(n_exams);
                weight_for_exams=solution->update_weights(n_exams);
                refuse++;
            
            }else{
                //solution->write_output_file(current_instance, n_exams);
                old_SA=obj_new;
                refuse=0;
            }
        }else{
            //solution->write_output_file(current_instance, n_exams);
            old_SA=obj_new;
            refuse=0;
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
        best_improvement=worst_sol-best_sol;
        improvement=obj_new-best_sol;

        /*if(improvement/best_improvement<0.1){
            cooling_coeff=0.9997;
        }*/

        t = cooling(timelimit, now.time-start.time,t0);
        output_file<<"temperature "<<t<<" OF OLD "<<obj_old<<"\n";

        /*if (count_iter>=160){            
            t = cooling(t, cooling_coeff);
            a-=0.001;
            if (a<=0){
                a=0.5;
            }
            count_iter=0;
        }*/

        /*if(improvement/best_sol>0.15){
            count_local_minima++;
            big_change=true;
        }*/

        

        //botta di calore per togliermi dal local minimum
        /*if(refuse==7){ // || (best_improvement-improvement)/best_improvement<0.1 
            refuse=0;
            //cout<<"shock"<<endl;        
            count_local_minima=0;
            t=temperature_shock(t0);
            cooling_coeff = cooling_coefficient;
            //neighbour_by_crossover(solution, best_solution, n_exams, n_timeslot);            
            neighbours_by_swapping(solution, n_timeslot);
            weight_for_exams=solution->update_weights(n_exams);
            int rescheduled = 0;
            int counter_unsched = 0;
            int num_unsched = 10;
            neighbourhood_by_obj_fun(rand()%n_exams, solution, n_timeslot, possible_timeslots, n_exams);
            weight_for_exams=solution->update_weights(n_exams); 
            obj_new=solution->objective_function(n_exams,total_number_students); 
            
            //cout<<"The solution out of the new function is feasible: "<<solution->check_feasibility(solution->timeslot_per_exams, solution->all_exams);
        } */
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