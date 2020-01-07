#include "header.hpp"
#include "TabuSearch.hpp"
#include "TabuSearch.cpp"
#include <time.h>
#include <sys/timeb.h>
#include <cmath>
#include <random>

using namespace std;
double sa_random(Solution* solution, struct timeb start, int timelimit, int n_exams, int total_number_students, int n_timeslot, string current_instance,double t0){
    struct timeb now;
    double t=t0;
    double obj_new;
    vector<int> old_timeslot_solution;
    vector<int> best_timeslot_solution;
    vector<double> weight_for_exams=solution->update_weights(n_exams);
    double obj_old = solution->objective_function(n_exams,total_number_students);
    cout<<"Initial Objective Function: "<<obj_old<<endl;
    //solution->write_output_file(current_instance, n_exams);
    int num_mutation=3;
    ftime(&now); 
    while((int)((now.time-start.time))<timelimit){

    }
}
