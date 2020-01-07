# pragma once
#include <vector>
#include <iostream>

using namespace std;

class Solution{
    public:
    vector <int> timeslot_per_exams;
    vector <Exam*> all_exams;
    vector <int> num_neighbours_for_exams;
    int flag=1;
    double double_obj=0;
    void solution_update(vector<vector<int>> conflict_matrix, int n_exams);
    int check_feasibility(std::vector<int> t, std::vector<Exam*> e);
    void update_timeslots(int n_exams);
    vector<double> update_weights(int n_exams);
    double objective_function(int n_exams, int total_number_student);
    Solution* copy_solution(int n_exams);
    void write_output_file(string current_instance, int n_exams);
};