# pragma once
#include <vector>
#include <iostream>



class Exam{
public:
    // valori che rimangono fissi per sempre:
    // esami conflittuali
    std::vector<int> conflict_exams; 
    // tempi conflittuali
    std::vector<int> conflict_weights;
    // numero di esami conflittuali
    size_t num_conflict;
    // id esame
    int id_exam;
    std::vector<int> position_in_conf_exams;

    // valori che cambiano nel tempo
    // timeslot degli esami conflittuali
    std::vector<int> conflict_times;
    // peso nella funzione obiettivo apportato dall'esame
    double weight_in_obj_fun;  
    // numero di timeslot disponibili  
    size_t available_times;
};
