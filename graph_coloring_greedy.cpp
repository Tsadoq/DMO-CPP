#include "graph_coloring_greedy.hpp"
#include <iostream>
#include <vector>

using namespace std;

vector<int> graph_coloring_greedy(vector<Exam*> all_exams, int n_timeslot, vector<size_t> sorted_index, int n_exams){

    // initialize vector of timeslot for each exams to -1
    vector <int> timeslot_per_exams=vector<int>(n_exams,-1);
    // put the the exam with highest number of conflits in first timeslot
    timeslot_per_exams[sorted_index[0]]=1;
    // index of the exam I'm considering: to get the id I must add 1
    int current_exam_idx;
    vector<int> current_exam_conflict;
    vector<int> available_timeslot;
    int select_timeslot;
    for (auto i: sorted_index) {
        // for each exam I want to see which are the available timeslot
        available_timeslot=vector<int>(n_timeslot,0);
        current_exam_idx=sorted_index[i];
        // read id of conflicting exam: to get index I must subtract 1
        current_exam_conflict=all_exams[current_exam_idx]->conflict_exams;
        for(auto j:current_exam_conflict){
            if(timeslot_per_exams[j-1]>0){
                // if a conflicting exam is yet scheduled in a time slot I sign it as unavailable
                available_timeslot[timeslot_per_exams[j-1]-1]=1;
            }
        }
        select_timeslot=0;
        while(available_timeslot[select_timeslot]==1 & select_timeslot<n_timeslot){
            select_timeslot++;
        }
        if (select_timeslot==n_timeslot){
            // I can't assign a timeslot avoiding conflicts only using greedy coloring
            // I'll handle it later: mark it with -2
            timeslot_per_exams[current_exam_idx]=-2;
        }else{
            // I put the exam in the first time slot available
            timeslot_per_exams[current_exam_idx]=select_timeslot+1;
        }        
    }
    return timeslot_per_exams;
}