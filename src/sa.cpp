#include "header.hpp"
#include <time.h>
#include <sys/timeb.h>
#include <cmath>
#include<random>

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
int num_mutation=30;
double perc=0.4;

 while((int)((now.time-start.time))<timelimit){
    ftime(&now);    
    order_for_mutation=sort_indexes(weight_for_exams);
    old_timeslot_solution=solution->timeslot_per_exams;  
    vector<vector<int>>mutations_vector=neighbours_by_mutation(solution, order_for_mutation, num_mutation, possible_timeslots,perc,n_exams);
    weight_for_exams=solution->update_weights(n_exams);    
    obj_new=solution->objective_function(n_exams,total_number_students);    
    if(obj_new > obj_old){
       prob = probability(obj_new, obj_old, t);
       prob_random = distribution(generator);
        if (prob_random > prob){
            //rifiuto, quindi risistemo la soluzione
            solution->timeslot_per_exams=old_timeslot_solution;
            solution->update_timeslots(n_exams);
            weight_for_exams=solution->update_weights(n_exams);
           
        }else{
        cout<<"Objective Function: "<<obj_new<<endl;
        solution->write_output_file("./instances/"+current_instance, n_exams);
        obj_old=obj_new;
       }

    }else{
        cout<<"Objective Function: "<<obj_new<<endl;
        solution->write_output_file("./instances/"+current_instance, n_exams);
        obj_old=obj_new;
    }

    t = cooling(t);
    //botta di calore per togliermi dal local minimum
    if(t<0.0001){ 
        t=200000; 
    }

 } 
}

double probability(double obj_new, double obj_old, double temperature)
{
    double e=2.71828183;
    double p = pow(e, -((obj_new-obj_old)/temperature)); // p = exp^(-(F(x_new)-F(x_old))/T)

    return p;
}

double cooling(double temperature)
{
    temperature = temperature/(1+500*temperature);
    return  temperature;
}

