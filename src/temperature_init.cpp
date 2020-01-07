#include "header.hpp"

double temperature_init(Solution *solution,int n_exams, int total_number_students, int n_timeslot)
{
    double obj_new;

    vector<double> weight_for_exams = solution->update_weights(n_exams);
    double obj_old = solution->objective_function(n_exams, total_number_students);
    vector<size_t> order_for_mutation = vector<size_t>(n_exams);
    vector<int> possible_timeslots;
    for (int i = 0; i < n_timeslot; i++)
    {
        possible_timeslots.push_back(i + 1);
    }

    int num_mutation = 3;
    double perc = 0.2;
    int count_iter = 0;

    double min_pen = obj_old;
    double max_pen = obj_old;    

    while (count_iter < 2000)
    {
        count_iter++;
        order_for_mutation = sort_indexes(weight_for_exams);
        vector<vector<int>> mutations_vector = neighbours_by_mutation(solution, order_for_mutation, num_mutation, possible_timeslots, perc, n_exams);
        weight_for_exams = solution->update_weights(n_exams);
        neighbours_by_swapping(solution, n_timeslot);
        weight_for_exams = solution->update_weights(n_exams);
        obj_new = solution->objective_function(n_exams, total_number_students);
        if (max_pen < obj_new)          max_pen = obj_new;
        else if (min_pen > obj_new)     min_pen = obj_new;
    }

    int heating_coeff = 7;

    double temp = (max_pen * heating_coeff - min_pen) / 0.25;

    cout << "Temperature initialization finished: " << temp << endl;
    return temp;
}