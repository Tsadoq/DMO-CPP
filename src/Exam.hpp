# pragma once
#include <vector>
#include <iostream>

using namespace std;

class Exam{
public:
    // valori che rimangono fissi per sempre:
    // esami conflittuali
    vector<int> conflict_exams; 
    // tempi conflittuali
    vector<int> conflict_weights;
    // numero di esami conflittuali
    size_t num_conflict;
    // id esame
    int id_exam;

    // valori che cambiano nel tempo
    // timeslot degli esami conflittuali
    vector<int> conflict_times;
    // peso nella funzione obiettivo apportato dall'esame
    double weight_in_obj_fun;  
    // numero di timeslot disponibili  
    size_t available_times;
};
