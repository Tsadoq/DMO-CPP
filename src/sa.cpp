#include "header.hpp"
#include "TabuSearch.hpp"
#include "TabuSearch.cpp"
#include <time.h>
#include <sys/timeb.h>
#include <cmath>
#include <random>

using namespace std;

double probability(double obj_new, double obj_old, double temperature, double maxtemperature);
double cooling(double temperature);
int num_mutation_changer(int num_mutation_actual, int iteration, double &perc, double improvement,double best_improvement,bool first,int n_exams);
double temperature_shock(double temperature);
void sa(Solution* solution, struct timeb start, int timelimit, int n_exams, int total_number_students, int n_timeslot,string current_instance){
 
 struct timeb now;
 double prob = 0;
 double prob_random = 0;
 double t0 = 30;
 double obj_new;
vector<int> old_timeslot_solution;
double a=0.5;

double t=t0;

std::default_random_engine generator;
std::uniform_real_distribution<double> distribution(0.0,1.0);

vector<double> weight_for_exams=solution->update_weights(n_exams);
double obj_old = solution->objective_function(n_exams,total_number_students);
cout<<"Initial Objective Function: "<<obj_old<<endl;

//riscrivo il file output -> da fare
solution->write_output_file(current_instance, n_exams);
vector<size_t> order_for_mutation=vector<size_t>(n_exams);
vector<int> possible_timeslots;
for (int i=0; i<n_timeslot;i++){
    possible_timeslots.push_back(i+1);
}
//PER ORA SONO FISSE -> MODIFICARE IN SA
int num_mutation=0;
double perc=1.0;
/*double n_mut=1.0;
double n_reset=5;*/
int count_iter=0;
double improvement=0;
double best_improvement=0;
double best_sol = obj_old;
double worst_sol = obj_old;
bool first=true;
//TabuSearch* tabu;
int num_shock=0;
ftime(&now); 
    while((int)((now.time-start.time))<timelimit){
        count_iter++;
        //TabuSearch tabu;
        order_for_mutation=sort_indexes(weight_for_exams);

        old_timeslot_solution=solution->timeslot_per_exams;  
        num_mutation = num_mutation_changer(num_mutation, count_iter, perc, improvement,best_improvement,first,n_exams);
        first=false;
        vector<vector<int>>mutations_vector=neighbours_by_mutation(solution, order_for_mutation, num_mutation, possible_timeslots,perc,n_exams);
        weight_for_exams=solution->update_weights(n_exams);    
        obj_new=solution->objective_function(n_exams,total_number_students); 
        /*cout<<"Obj function pre swapping: "<<obj_new<<endl;
        cout<<"Trying to do swapping"<<endl;
        neighbours_by_swapping(solution, n_timeslot);
        weight_for_exams=solution->update_weights(n_exams);    
        obj_new=solution->objective_function(n_exams,total_number_students); 
        cout<<"Swapping done with new obj"<<obj_new<<endl;

        mutations_vector=neighbours_by_mutation(solution, order_for_mutation, num_mutation, possible_timeslots,perc,n_exams);
        weight_for_exams=solution->update_weights(n_exams);    
        obj_new=solution->objective_function(n_exams,total_number_students);
        cout<<"Trying to do swapping after mutation, new obj pre swap: "<<obj_new<<endl;
        neighbours_by_swapping(solution, n_timeslot);
        weight_for_exams=solution->update_weights(n_exams);    
        obj_new=solution->objective_function(n_exams,total_number_students); 
        cout<<"Swapping done with new obj"<<obj_new<<endl;

        mutations_vector=neighbours_by_mutation(solution, order_for_mutation, num_mutation, possible_timeslots,perc,n_exams);
        weight_for_exams=solution->update_weights(n_exams);    
        obj_new=solution->objective_function(n_exams,total_number_students);
        cout<<"Trying to do swapping after mutation, new obj pre swap: "<<obj_new<<endl;
        neighbours_by_swapping(solution, n_timeslot);
        weight_for_exams=solution->update_weights(n_exams);    
        obj_new=solution->objective_function(n_exams,total_number_students); 
        cout<<"Swapping done with new obj"<<obj_new<<endl;

        mutations_vector=neighbours_by_mutation(solution, order_for_mutation, num_mutation, possible_timeslots,perc,n_exams);
        weight_for_exams=solution->update_weights(n_exams);    
        obj_new=solution->objective_function(n_exams,total_number_students);
        cout<<"Trying to do swapping after mutation, new obj pre swap: "<<obj_new<<endl;
        neighbours_by_swapping(solution, n_timeslot);
        weight_for_exams=solution->update_weights(n_exams);    
        obj_new=solution->objective_function(n_exams,total_number_students); 
        cout<<"Swapping done with new obj"<<obj_new<<endl;*/

        weight_for_exams=solution->update_weights(n_exams);    
        obj_new=solution->objective_function(n_exams,total_number_students);    

        //TABU SEARCH
        //tabu.tabuControl(solution,  n_exams, n_timeslot);
        //weight_for_exams=solution->update_weights(n_exams);    
        //obj_new=solution->objective_function(n_exams,total_number_students);    
        cout<<"Arrivo qui?"<<endl;

        if(obj_new > obj_old){
        //prob = probability(obj_new, obj_old, t);
        prob=probability(obj_new, obj_old, t, t0);
        prob_random = distribution(generator);
            if (prob_random > prob){

                //rifiuto, quindi risistemo la soluzione
                solution->timeslot_per_exams=old_timeslot_solution;
                solution->update_timeslots(n_exams);
                weight_for_exams=solution->update_weights(n_exams);
            
            }else{
            solution->write_output_file(current_instance, n_exams);
            //cout<<"Objective Function: "<<obj_new<<endl;
            obj_old=obj_new;
        }

        }else{
            //cout<<"Objective Function: "<<obj_new<<endl;
            //solution->write_output_file("./instances/"+current_instance, n_exams);
            obj_old=obj_new;

        }
        
        if (obj_new < best_sol){
            best_sol = obj_new;
            first=true;
        }
        if (obj_new > worst_sol){
            worst_sol = obj_new;
        }
        best_improvement=worst_sol-best_sol;
        improvement=obj_new-best_sol;

        if (count_iter>=a*n_exams){            
            t = cooling(t);
            a-=0.001;
            if (a<=0){
                a=0.5;
            }
            count_iter=0;
        }
        //botta di calore per togliermi dal local minimum
        if(t<t0*0.001 ){ // || (best_improvement-improvement)/best_improvement<0.1         
            temperature_shock(t0);
            cout<<"shock"<<endl;
        }
    ftime(&now); 
    cout<<"E qui?"<<endl;

    }
    cout<<"Best sol "<<best_sol<<endl;
}

/*double probability(double obj_new, double obj_old, double temperature)
{
    double e=2.71828183;
    double p = pow(e, -((obj_new-obj_old)/temperature)); // p = exp^(-(F(x_new)-F(x_old))/T)
    return p;
}*/

double probability(double obj_new, double obj_old, double temperature, double maxtemperature)
{
    double e = 2.71828183;

    double bias = 0.5+(maxtemperature-temperature)/maxtemperature;
    double p = pow(e, -((obj_new*bias - obj_old) / temperature)); // p = exp^(-(F(x_new)-F(x_old))/T)

    return p;
}

double cooling(double temperature)
{
    temperature = 0.8*temperature;
    //temperature=temperature/(1+500*temperature);
    //temperature=1/((1/temperature)+0.001/2);
    return  temperature;
}

// passare per riferimento perc, modificare num_mutation
int num_mutation_changer(int num_mutation_actual, int iteration, double &perc, double improvement,double best_improvement,bool first,int n_exams){
    int num_mutation_now;
    int available_num_mutation;
    if (first==true){
        perc=1;
        num_mutation_now=1;
    }else{    
        perc = (best_improvement-improvement)/best_improvement;           
        available_num_mutation=ceil(perc*n_exams*0.05);
        num_mutation_now = rand()%available_num_mutation+1;
        //cout<< num_mutation_now<<endl;
        /*cout<< num_mutation_actual<<endl;*/
    }
    return num_mutation_now;
}

double temperature_shock(double temperature){
    return 2*temperature;
}