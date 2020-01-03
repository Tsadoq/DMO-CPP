#include "header.hpp"
#include "TabuSearch.hpp"
#include "TabuSearch.cpp"
#include <time.h>
#include <sys/timeb.h>
#include <cmath>
#include <random>

using namespace std;

double probability(double obj_new, double obj_old, double temperature);
double cooling(double temperature);

void sa(Solution* solution, struct timeb start, int timelimit, int n_exams, int total_number_students, int n_timeslot,string current_instance){
 struct timeb now;
 double prob = 0;
 double prob_random = 0;
 double t = 20000;
 double obj_new;
 vector<int> old_timeslot_solution;
 int a=1;


Solution* copy_sol;

std::default_random_engine generator;
std::uniform_real_distribution<double> distribution(0.0,1.0);

vector<double> weight_for_exams=solution->update_weights(n_exams);
double obj_old = solution->objective_function(n_exams,total_number_students);
cout<<"Initial Objective Function: "<<obj_old<<endl;

//riscrivo il file output -> da fare
solution->write_output_file("./instances/"+current_instance, n_exams);
vector<size_t> order_for_mutation=vector<size_t>(n_exams);
vector<int> possible_timeslots;
for (int i=0; i<n_timeslot;i++){
    possible_timeslots.push_back(i+1);
}
//PER ORA SONO FISSE -> MODIFICARE IN SA
int num_mutation;
double perc=0.7;
double n_mut=1.0;
double n_reset=5;
int count_iter=0;

double best_sol = obj_old;
//TabuSearch* tabu;
int num_shock=0;
ftime(&now); 
    while((int)((now.time-start.time))<timelimit){
        count_iter++;
        //TabuSearch tabu;
        order_for_mutation=sort_indexes(weight_for_exams);

        old_timeslot_solution=solution->timeslot_per_exams;  
        num_mutation=(int) n_mut;
        //cout<<num_mutation<<endl;
        vector<vector<int>>mutations_vector=neighbours_by_mutation(solution, order_for_mutation, num_mutation, possible_timeslots,perc,n_exams);
        weight_for_exams=solution->update_weights(n_exams);    
        obj_new=solution->objective_function(n_exams,total_number_students);    

        //TABU SEARCH
        //tabu.tabuControl(solution,  n_exams, n_timeslot);
        //weight_for_exams=solution->update_weights(n_exams);    
        //obj_new=solution->objective_function(n_exams,total_number_students);    

        if(obj_new > obj_old){
        prob = probability(obj_new, obj_old, t);
        prob_random = distribution(generator);
            if (prob_random > prob){

                //rifiuto, quindi risistemo la soluzione
                solution->timeslot_per_exams=old_timeslot_solution;
                solution->update_timeslots(n_exams);
                weight_for_exams=solution->update_weights(n_exams);
            
            }else{
            solution->write_output_file("./instances/"+current_instance, n_exams);
            //cout<<"Objective Function: "<<obj_new<<endl;
            obj_old=obj_new;
        }

        }else{
            //cout<<"Objective Function: "<<obj_new<<endl;
            //solution->write_output_file("./instances/"+current_instance, n_exams);
            obj_old=obj_new;

        }
        if (count_iter>=a*n_exams){            
            t = cooling(t);
            cout<<t<<endl;
            a++;
            count_iter=0;
        }
        //botta di calore per togliermi dal local minimum
        if(t<0.00001){ 
            num_shock++;            
            t=20000;
            if (num_shock>n_reset){
                num_shock=0;
                t=t*2.5;
                n_reset+=0.5;
                n_mut+=0.5;
            } 
        }
        if (obj_new < best_sol){
            best_sol = obj_new;
        }
    ftime(&now); 
    }
    cout<<num_mutation<<endl;
    cout<<"Best sol "<<best_sol<<endl;
}

double probability(double obj_new, double obj_old, double temperature)
{
    double e=2.71828183;
    double p = pow(e, -((obj_new-obj_old)/temperature)); // p = exp^(-(F(x_new)-F(x_old))/T)

    return p;
}

double cooling(double temperature)
{
    //temperature = 0.99995*temperature;
    temperature=temperature/(1+500*temperature);
    //temperature=1/((1/temperature)+0.001/4);
    return  temperature;
}