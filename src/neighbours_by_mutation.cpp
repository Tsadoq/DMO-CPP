#include "header.hpp"
#include<string>
#include <iostream>
#include <algorithm>
#include <vector>
#include <iterator>
#include "Exam.hpp"
#include "Solution.hpp"

using namespace std;

vector<vector<int>> neighbours_by_mutation(vector<Exam*> all_exams, vector<size_t> order_for_mutation,int num_mutation,vector<int> possible_timeslots){

    vector<vector<int>> mutations_vector; 
    vector<int> single_mutation=vector<int> (2);                                       
    Exam* exam_mutate;
    int is_void=0;
    vector<int> not_available_timeslots;
    vector<int> available_timeslots;
    int k;
    int new_timeslot;
    int randomIndex;
    for(int i=0;i<num_mutation+is_void;i++){
        // exam I'm trying to mutate
        available_timeslots=vector<int> ();
        exam_mutate=all_exams[order_for_mutation[i]];
        not_available_timeslots=exam_mutate->conflict_times;
        // sort vector because set_difference works with sorted arrays
        sort(not_available_timeslots.begin(), not_available_timeslots.end());
        // find available timeslot
        set_difference(possible_timeslots.begin(), possible_timeslots.end(), not_available_timeslots.begin(), 
                        not_available_timeslots.end(),inserter(available_timeslots, available_timeslots.begin()));
        if(available_timeslots.size()==0){
            is_void++;
        }else{
            randomIndex = rand() % available_timeslots.size();
            new_timeslot=available_timeslots[randomIndex];
            // update timeslot in exam I want to mutate
            exam_mutate->timeslot=new_timeslot;
            // update timeslot in all conflicting exam with respect to exam_mutate
            for (auto j : exam_mutate->conflict_exams){
                k=0;
                while(all_exams[j-1]->conflict_exams[k] != exam_mutate->id_exam){
                    k++;
                }
                all_exams[j-1]->conflict_times[k]=new_timeslot;
            }
            // insert exam I want to mutate
            single_mutation[0]=exam_mutate->id_exam;
            // insert time slot in which I want to schedule exam
            single_mutation[1]=new_timeslot;
        }
        mutations_vector.push_back(single_mutation);
    }
    return mutations_vector;
}