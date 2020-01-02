#include "header.hpp"
#include "TabuSearch.hpp"
//#include "TabuSearch.cpp"
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
int num_mutation=5;
double perc=0.5;

double best_sol = obj_old;
TabuSearch* tabu;

while((int)((now.time-start.time))<timelimit){
    ftime(&now);    
    TabuSearch tabu;
    order_for_mutation=sort_indexes(weight_for_exams);
    copy_sol=solution->copy_solution(n_exams);    
    vector<vector<int>>mutations_vector=neighbours_by_mutation(copy_sol, order_for_mutation, num_mutation, possible_timeslots,perc,n_exams);
    /*int feasible=copy_sol->check_feasibility;
    cout<<"Feasibility: "<<feasible;*/
    Solution* copy_sol2;
    copy_sol2 = tabu.tabuControl(solution, copy_sol);
    weight_for_exams=copy_sol2->update_weights(n_exams);    
    obj_new=copy_sol2->objective_function(n_exams,total_number_students);    
    if(obj_new > obj_old){
       prob = probability(obj_new, obj_old, t);
       prob_random = distribution(generator);
        if (prob_random > prob){
           //non aggiorno il valore della funzione obiettivo
           cout<<"Algorithm is not updating!"<<endl;
        }else{
        cout<<"Objective Function: "<<obj_new<<endl;
        copy_sol2->write_output_file("./instances/"+current_instance, n_exams);
        obj_old=obj_new;
        solution=copy_sol; 
       }

    }else{
        cout<<"Objective Function: "<<obj_new<<endl;
        copy_sol2->write_output_file("./instances/"+current_instance, n_exams);
        obj_old=obj_new;
        solution=copy_sol2; 
    }

    t = cooling(t);
    //botta di calore per togliermi dal local minimum
    /*if(t<0.0001){ 
        t=200000; 
    }*/
    if (obj_new < best_sol){
        best_sol = obj_new;
    }

 }
 cout<<"Best solution found is: "<<best_sol<<endl;
}

double probability(double obj_new, double obj_old, double temperature)
{
    double e=2.71828183;
    double p = pow(e, -((obj_new-obj_old)/temperature)); // p = exp^(-(F(x_new)-F(x_old))/T)

    return p;
}

double cooling(double temperature)
{
    temperature = 0.99995*temperature;
    return  temperature;
}