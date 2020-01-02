#include "header.hpp"
#include <time.h>
#include <sys/timeb.h>
#include <cmath>
#include <random>
#include "Solution.hpp"

using namespace std;

class sa
{

public:
    int temperature;
    int plateau;
    Solution *initial_sol;
    Solution *current_sol;
    int running_time;
    int warmup_time;
    int n_exams;
    int total_number_students;
    int n_timeslot;
    int num_mutation;
    string current_instance;

    sa(Solution *initial_solution, int total_time, int num_exams, int tot_number_students, int num_timeslot, string this_instance, int n_mutation)
    {

        initial_sol = initial_solution;
        warmup_time = total_time / 10;
        running_time = total_time - running_time;
        n_exams = num_exams;
        total_number_students = tot_number_students;
        n_timeslot = num_timeslot;
        current_instance = this_instance;
        num_mutation = n_mutation;
    }

    int heatup()
    {
        struct timeb start;
        ftime(&start);
        struct timeb now;
        vector<double> weight_for_exams = initial_sol->update_weights(n_exams);
        double obj_old = initial_sol->objective_function(n_exams, total_number_students);
        double max_penalty = obj_old;
        double min_penalty = obj_old;
        double current_penalty = obj_old;
        vector<size_t> order_for_mutation = vector<size_t>(n_exams);
        vector<int> possible_timeslots;
        Solution* copy_sol;
        double obj_new;

        while ((int)((now.time - start.time)) < warmup_time)
        {
            
            ftime(&now);
            order_for_mutation = sort_indexes(weight_for_exams);
            copy_sol = initial_sol->copy_solution(n_exams);
            vector<vector<int>> mutations_vector = neighbours_by_mutation(copy_sol, order_for_mutation, num_mutation, possible_timeslots);
            weight_for_exams = copy_sol->update_weights(n_exams);
            obj_new = copy_sol->objective_function(n_exams, total_number_students);
            if (obj_new > max_penalty){max_penalty = obj_new;}
            else {
                if()
            }
        }

        // qui dobbiamo:
        // - runnare per un tempo warmup_time
        // - generare nuove soluzioni (senza farci problemi sugli esami con priorità più o meno alta)
        // - - se la soluzione è migliorante la teniamo e
        // - - (anche se non è migliorante) salviamo la penalty se maggiore di max_penalty o minore di min_penalty
        // - - sempre se non è migliorante la teniamo con una probabilità di 0.5;
        // - finite le iterazioni usiamo max e min penalty per calcolare il delta e delta per calcolare la temperatura

        return 0;
    };
    void run(){
        // sa normale con un running time uguale a running_time;
    };
};