#include "header.hpp"
#include <iostream>
#include <vector>
#include <iterator>

using namespace std;

void piazzo_random(Solution* initial_solution, vector<int> available_colors, vector<Exam*> notInPosition,  int TRY_PLACE);

void alternativeColoring(Solution* initial_solution, int n_timeslot, vector<size_t> sorted_index, int n_exams){

    vector<int> seed_for_boxes{0,3,1,4,2,5};
    vector<vector<int>> boxes;
    vector<Exam*> notInPosition = initial_solution->all_exams;
    vector<Exam*> yesInPosition;    
    //inizializzo i timeslot conflittuali
    //-------------------------------------------------------------------------------
    int dim=0;
    for(int i=0; i<n_exams; i++){
        dim=initial_solution->all_exams[i]->conflict_exams.size();
        initial_solution->all_exams[i]->conflict_times=vector<int>(dim,-1);
    }
    //-------------------------------------------------------------------------------
    //inizializzo i boxes
    //-------------------------------------------------------------------------------
    int seed;
    vector<int> tmp;
    for(int i=0; i<seed_for_boxes.size(); i++){
        seed= seed_for_boxes[i];
        tmp=vector<int>();
        while(seed<n_timeslot){
            tmp.push_back(seed);
            seed= seed+6;
        }
        boxes.push_back(tmp);
    }
    
    //-------------------------------------------------------------------------------
    //coloramento
    int index_boxes = 0;
    int failures = 0;
    int num = 0;
    int TRY_PLACE = 2*n_timeslot;
    vector<int> available_colors;
    
    available_colors.insert(available_colors.end(), boxes[0].begin(), boxes[0].end());
    
    while(notInPosition.size()==0){
        piazzo_random(initial_solution, available_colors, notInPosition, TRY_PLACE);
        while()
    }



}
void piazzo_random(Solution* initial_solution, vector<int> available_colors, vector<Exam*> notInPosition, int TRY_PLACE){
    Exam* exam;
    int i=0;
    int find_conflict;
    while(i<notInPosition.size() && notInPosition.size()>0){
        exam = notInPosition[i];
        find_conflict = 0;
        for(int k=0; k<TRY_PLACE; k++){
            int index_time = rand() % available_colors.size(); // scelgo un esame a caso
            int time = available_colors[index_time]+1;
            for(int j=0; j< initial_solution->all_exams[exam->id_exam-1]->conflict_times.size(); j++){
                if(initial_solution->all_exams[exam->id_exam-1]->conflict_times[j]==time){
                    find_conflict=1;
                }
            }
            //if(find(initial_solution->all_exams[exam->id_exam-1]->conflict_times.begin(), 
            //initial_solution->all_exams[exam->id_exam-1]->conflict_times.end(), time)==initial_solution->all_exams[exam->id_exam-1]->conflict_times.end());
            if(find_conflict==0){
                initial_solution->all_exams[exam->id_exam-1]->timeslot=time;
                for (auto jj : initial_solution->all_exams[exam->id_exam-1]->conflict_exams){ 
                    int kk=0; 
                    while(initial_solution->all_exams[jj-1]->conflict_exams[kk] != initial_solution->all_exams[exam->id_exam-1]->id_exam){ 
                        kk++;
                    } 
                initial_solution->all_exams[jj-1]->conflict_times[kk]=time; 
                }
            
            }
        }
        
    }
}