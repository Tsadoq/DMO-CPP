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
double temperature_shock(double temperature);

double sa_random(Solution* solution, struct timeb start, int timelimit, int n_exams, int total_number_students, int n_timeslot, string current_instance,double t0){
    struct timeb now;
    double t=t0;
    double obj_new;
    vector<int> old_timeslot_solution;
    vector<int> best_timeslot_solution;
    vector<double> weight_for_exams=solution->update_weights(n_exams);
    solution->flag = solution->check_feasibility_weighted(solution->timeslot_per_exams,solution->all_exams);
    double obj_old = solution->objective_function_unfeasible(n_exams,total_number_students, solution->flag);
    cout<<"Initial Objective Function: "<<obj_old<<endl;
    //solution->write_output_file(current_instance, n_exams);
    int num_mutation=1;
    ftime(&now); 
    int flag;
    vector<double> conflitti;
    double prob_random;
    int index_exam;
    int color;
    double prob =0;
    int count_iter =0;
    double a=0.5;

    std::default_random_engine generator;
    std::uniform_real_distribution<double> distribution(0.0,1.0);
    double best_sol = obj_old;
    int old_conflitti_esame;
    int new_conflitti_esame;
    int segna_conflitti;

    while((int)((now.time-start.time))<timelimit){
        old_conflitti_esame=0;
        segna_conflitti = 0;
        count_iter++;
        conflitti=vector<double>();
        old_timeslot_solution = solution->timeslot_per_exams;
        solution->flag = solution->check_feasibility_weighted(solution->timeslot_per_exams,solution->all_exams);
        std::random_device rd; 
        std::mt19937 gen(rd()); 
       
        /*for(int i=0; i<solution->all_exams.size(); i++){
            prob_random.push_back((solution->all_exams[i]->flag_for_exams)/flag);
        }*/
        for(int i=0; i<solution->all_exams.size(); i++){
            conflitti.push_back(solution->all_exams[i]->flag_for_exams);  
        }
       
        std::discrete_distribution<> d(conflitti.begin(), conflitti.end());
        index_exam = d(gen);
        color = rand()%n_timeslot+1;
        old_conflitti_esame = solution->all_exams[index_exam]->flag_for_exams;
        solution->timeslot_per_exams[index_exam] = color;
        //----------------------------------------------------------------------------
        solution->update_timeslots(n_exams);
        solution->flag = solution->check_feasibility_weighted(solution->timeslot_per_exams,solution->all_exams);
        new_conflitti_esame = solution->all_exams[index_exam]->flag_for_exams; 
        if(new_conflitti_esame>old_conflitti_esame){
            segna_conflitti=new_conflitti_esame;
        }else{
            segna_conflitti=0;
        }
        //----------------------------------------------------------------------------
        weight_for_exams=solution->update_weights(n_exams);    
        obj_new=solution->objective_function_unfeasible(n_exams, total_number_students, segna_conflitti);
        cout<<"nuova fo "<<obj_new<<endl;
        if(obj_new > obj_old){
            prob=probability(obj_new, obj_old, t);
            prob_random = distribution(generator);
            if (prob_random > prob){
                //rifiuto, quindi risistemo la soluzione
                solution->timeslot_per_exams=old_timeslot_solution;
                solution->update_timeslots(n_exams);
                weight_for_exams=solution->update_weights(n_exams);
            }else{
                //solution->write_output_file(current_instance, n_exams);
                cout<<obj_new<<"puzzi "<<solution->flag<<endl;
                obj_old=obj_new;
            }
        }else{
            //solution->write_output_file(current_instance, n_exams);
            obj_old=obj_new;
            cout<<obj_new<<"tu di piu "<<solution->flag<<endl;
        }

        if (obj_new < best_sol && solution->flag==0){
            best_sol = obj_new;
            best_timeslot_solution=solution->timeslot_per_exams;
            // solution->write_output_file(current_instance, n_exams);
        }
       t = cooling(t);
       cout<<"temperatura: "<<t<<endl;
        /*if (count_iter>=a*n_exams){            
            t = cooling(t);
            a-=0.001;
            if (a<=0){
                a=0.5;
            }
            count_iter=0;
        }*/
        //botta di calore per togliermi dal local minimum
        /*if(t<t0*0.01 ){ // || (best_improvement-improvement)/best_improvement<0.1         
            t=temperature_shock(t0);
        }*/
     ftime(&now); 
    }
    solution->timeslot_per_exams = best_timeslot_solution;
    solution->update_timeslots(n_exams);
    solution->update_weights(n_exams);
    cout<<best_sol<<" "<<solution->flag<<endl;
    return best_sol;
}

double probability(double obj_new, double obj_old, double temperature)
{
    double e=2.71828183;
    double p = pow(e, -((obj_new-obj_old)/temperature)); // p = exp^(-(F(x_new)-F(x_old))/T)
    return p;
}

double cooling(double temperature)
{
    temperature = 0.8*temperature;
    //temperature=temperature/(1+500*temperature);
    //temperature=1/((1/temperature)+0.001/2);
    return  temperature;
}

double temperature_shock(double temperature){
    return 2*temperature;
}