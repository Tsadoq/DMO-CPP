# pragma once
#include <vector>
#include <iostream>

using namespace std;

class Solution{
    public:
    // valori variabili all'interno dell'algoritmo
    vector <int> timeslot_per_exams;
    vector <Exam*> all_exams;
    vector <int> num_neighbours_for_exams;
    double double_obj=0;

    // valori fissi all'interno dell'algoritmo
    int n_exams=0;
    int total_number_students;

    // aggiorno i valori che rimarranno fissi per tutto l'algoritmo
    void solution_update(vector<vector<int>> conflict_matrix, int n_exams, int total_number_students);
    int check_feasibility(std::vector<int> t, std::vector<Exam*> e);
    void update_timeslots(int n_exams);
    vector<double> update_weights(int n_exams);
    double objective_function(int n_exams, int total_number_student);
    Solution* copy_solution(int n_exams);
    void write_output_file(string current_instance, int n_exams);
};