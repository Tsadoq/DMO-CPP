#include "header.hpp"
#include <iostream>
#include <vector>
#include <iterator>
#include <string> 
#include <numeric>  
#include <random>   
#include <algorithm> 




void piazzo_random(Solution* initial_solution, std::vector<int> available_colors, std::vector<Exam*> &notInPosition, int TRY_PLACE);
bool swap_random(Solution* initial_solution, std::vector<int> available_colors);
bool prova_a_inserire(Solution* initial_solution,  std::vector<int> available_colors, int TRY_PLACE, std::vector<Exam*> &notInPosition, Exam* exam);

void alternativeColoring(Solution* initial_solution,std::vector<size_t> sorted_index){
    srand(time(NULL));
    int n_exams=initial_solution->n_exams;
    int n_timeslot=initial_solution->n_timeslot;
    initial_solution->timeslot_per_exams=std::vector<int>(n_exams, -1);
    std::vector<int> seed_for_boxes{0,3,1,4,2,5};
    std::vector<std::vector<int>> boxes;
    std::vector<Exam*> notInPosition;
    notInPosition.reserve(n_exams);

    for(int n=0;n<n_exams;n++){
        notInPosition.push_back(initial_solution->all_exams[sorted_index[n]]);
    }
    //vector<Exam*> yesInPosition;    
    //inizializzo i timeslot conflittuali
    //-------------------------------------------------------------------------------
    
    for(int i=0; i<n_exams; i++){
        int dim=initial_solution->all_exams[i]->num_conflict;
        initial_solution->all_exams[i]->conflict_times=std::vector<int>(dim,-1);
    }
    //-------------------------------------------------------------------------------
    //inizializzo i boxes
    //-------------------------------------------------------------------------------
    int seed;
    std::vector<int> tmp;
    for(int i=0; i<seed_for_boxes.size(); i++){
        seed= seed_for_boxes[i];
        tmp=std::vector<int>();
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
    int FAILURES = 2*n_timeslot;
    int NUM= 10;
    std::vector<int> available_colors;
    
    available_colors.reserve(n_timeslot);
    available_colors.insert(available_colors.end(), boxes[0].begin(), boxes[0].end());  

    while(notInPosition.size()!=0){
        piazzo_random(initial_solution, available_colors, notInPosition, TRY_PLACE);
        while(!swap_random( initial_solution, available_colors)){          
        }
        num++;
        if(num > NUM){
            if(index_boxes<5){
               index_boxes++;
               available_colors.insert(available_colors.end(), boxes[index_boxes].begin(), boxes[index_boxes].end());
            }else{
                failures++;
                if(failures == FAILURES){
                    failures=0;
                    index_boxes=0;
                    notInPosition.clear();
                    for(int n=0;n<n_exams;n++){
                        notInPosition.push_back(initial_solution->all_exams[sorted_index[n]]);
                    }
                    available_colors.clear();
                    available_colors.insert(available_colors.end(), boxes[0].begin(), boxes[0].end());
                    for(int i=0; i<n_exams; i++){
                        int dim=initial_solution->all_exams[i]->num_conflict;
                        initial_solution->all_exams[i]->conflict_times=std::vector<int>(dim,-1);
                    }
                    initial_solution->timeslot_per_exams.assign(n_exams,-1);
                }   
            }
            num=0;
        }
    }
}

void piazzo_random(Solution* initial_solution, std::vector<int> available_colors, std::vector<Exam*> &notInPosition, int TRY_PLACE){
    Exam* exam=new Exam();
    int i=0;
    while(i<notInPosition.size() && notInPosition.size()!=0){
        exam = notInPosition[i];
        //prova_a_inserire ritorna false se non ha inserito ritorna true se ha inserito
        if(!prova_a_inserire( initial_solution, available_colors, TRY_PLACE, notInPosition, exam)){
          i++;
          break;
        }
    }
}

bool swap_random(Solution* initial_solution, std::vector<int> available_colors){
    int t1;
    int t2;
    int index_exam;
    std::vector<int> tmp;

    Exam* exam=new Exam();
    int n_exams=initial_solution->n_exams;

    tmp.reserve(n_exams);

    for(int i=0; i<n_exams; i++){
        if(initial_solution->timeslot_per_exams[i]>0){
            tmp.push_back(i);
        }
    } 
    int index = rand() % tmp.size();
    index_exam = tmp[index];
    t1 = initial_solution->timeslot_per_exams[index_exam];
    int index2 = rand() % available_colors.size();
    t2 = available_colors[index2]+1;
    int kk;
    exam=initial_solution->all_exams[index_exam];

    if (std::find(exam->conflict_times.begin(), exam->conflict_times.end(), t2) != exam->conflict_times.end()){
            //se ho trovato conflitto provo un'altra mossa random
            return false;
    }

    initial_solution->update_single_exam(index_exam,t2);       
    return true;
}

bool prova_a_inserire(Solution* initial_solution, std::vector<int> available_colors, int TRY_PLACE, std::vector<Exam*> &notInPosition, Exam* exam){
    int find_conflict;
    int kk;
    for(int k=0; k<TRY_PLACE; k++){
        int index_time = rand() % available_colors.size(); // scelgo un tempo a caso
        int time = available_colors[index_time]+1;
        find_conflict=0;
        for(int j=0; j< exam->conflict_times.size() && find_conflict==0; j++){
            if(exam->conflict_times[j]==time){
                find_conflict=1; 
            }
        }
        if(find_conflict==0){
            notInPosition.erase(notInPosition.begin());
            initial_solution->update_single_exam(exam->id_exam,time); 
            return true; 
        } 
    }
    return false;
            
}