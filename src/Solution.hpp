# pragma once
#include <vector>
#include <iostream>

using namespace std;

class Solution{
    public:
    vector <int> timeslot_per_exams;
    vector <Exam*> all_exams;
    vector <int> num_neighbours_for_exams;

    void solution_update(vector<vector<int>>, int);
    int check_feasibility(std::vector<int>, std::vector<Exam*>);
    void update_timeslots(int n_exams);
    vector<double> update_weights(int n_exams);
    double objective_function(int n_exams, int total_number_student);
    Solution* copy_solution(int n_exams);
    void write_output_file(string current_instance, int n_exams);
    vector<vector<int>> getNeighbourhood(int size);
    void new_solution(vector<int> new_timeslots, int n_exams);
};