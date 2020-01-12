# pragma once
#include <vector>
#include <iostream>



class Solution{
    public:
    // valori variabili all'interno dell'algoritmo
    std::vector <int> timeslot_per_exams;
    std::vector <Exam*> all_exams;
    std::vector <int> num_neighbours_for_exams;
    double double_obj=0;

    // valori fissi all'interno dell'algoritmo
    int n_exams=0;
    int total_number_students;
    std::vector<int> possible_timeslot;
    int n_timeslot;

    // aggiorno i valori che rimarranno fissi per tutto l'algoritmo
    void solution_update(std::vector<std::vector<int>> conflict_matrix, int n_exams, int total_number_students, int n_timeslot);
    // check della feasibility della soluzione
    int check_feasibility(std::vector<int> t, std::vector<Exam*> e);
    // aggiornamento timeslot per chiamata iniziale
    void update_timeslots();
    // scrittura su file
    void write_output_file(std::string current_instance);
    // aggiornamento dei pesi per chiamata iniziale
    void update_weights();
    // calcolo della funzione obiettivo per la chiamata iniziale
    double objective_function();
    // creo una copia della soluzione iniziale
    Solution* copy_solution();
    // aggiorno il timeslot di un esame nella soluzione e nei suoi conflittuali
    void update_single_exam(int exam, int new_timeslot);
    
};