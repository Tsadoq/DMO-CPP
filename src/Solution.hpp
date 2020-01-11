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
    // check della feasibility della soluzione
    int check_feasibility(std::vector<int> t, std::vector<Exam*> e);
    // aggiornamento timeslot per chiamata iniziale
    void update_timeslots();
    // scrittura su file
    void write_output_file(string current_instance);
    // aggiornamento dei pesi per chiamata iniziale
    void update_weights();
    // calcolo della funzione obiettivo per la chiamata iniziale
    double objective_function();
    // creo una copia della soluzione iniziale
    Solution* copy_solution();
    
};