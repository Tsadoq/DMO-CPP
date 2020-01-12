#include "header.hpp"
#include <iostream>
#include <vector>
#include <iterator>
#include <string> 
#include <numeric>  
#include <random>   
#include <algorithm> 


using namespace std;

void piazzo_random(Solution* initial_solution, vector<int> available_colors, vector<Exam*> &notInPosition, int TRY_PLACE);
bool swap_random(Solution* initial_solution, vector<int> available_colors);
bool prova_a_inserire(Solution* initial_solution,  vector<int> available_colors, int TRY_PLACE, vector<Exam*> &notInPosition, Exam* exam);

int alternativeColoring(Solution* initial_solution, int n_timeslot,  int n_exams,vector<size_t> sorted_index){
    srand(time(NULL));
    initial_solution->timeslot_per_exams=vector<int>(n_exams, -1);
    vector<int> seed_for_boxes{0,3,1,4,2,5};
    vector<vector<int>> boxes;
    vector<Exam*> notInPosition;
    for(int n=0;n<n_exams;n++){
        notInPosition.push_back(initial_solution->all_exams[sorted_index[n]]);
    }
    //vector<Exam*> yesInPosition;    
    //inizializzo i timeslot conflittuali
    //-------------------------------------------------------------------------------
    int dim=0;
    for(int i=0; i<n_exams; i++){
        dim=initial_solution->all_exams[i]->conflict_exams.size();
        initial_solution->all_exams[i]->conflict_times=vector<int>(dim,-1);
        initial_solution->all_exams[i]->timeslot=-1;
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
    int FAILURES = 2*n_timeslot;
    int NUM= 10;
    vector<int> available_colors;
    
    available_colors.insert(available_colors.end(), boxes[0].begin(), boxes[0].end());
    //cout<<"size di notinPos "<<notInPosition.size()<<endl;
   

    while(notInPosition.size()!=0){

        /*for(int tt=0;tt<notInPosition.size();tt++){
            cout<<notInPosition[tt]->id_exam<<" ";
        }
        cout<<endl;*/

        //cout<<"entro nel while"<<endl;
        piazzo_random(initial_solution, available_colors, notInPosition, TRY_PLACE);
        while(!swap_random( initial_solution, available_colors)){
            //cout<<"aaa"<<endl;
        }
        num++;
        if(num > NUM){
            if(index_boxes<6){
                //cout<<"aumento boxes"<<endl;
               index_boxes++;
               available_colors.insert(available_colors.end(), boxes[index_boxes].begin(), boxes[index_boxes].end());
               //cout<<"num colori "<<available_colors.size()<<endl;
            }else{
                failures++;
                //cout<<"ppppppppppppppp"<<endl;
                //cout<<"esami da piazzare pre failure"<<notInPosition.size()<<endl;
                if(failures == FAILURES){
                    //cout<<"restart"<<endl;
                    failures=0;
                    index_boxes=0;
                    notInPosition=vector<Exam*>();
                    for(int n=0;n<n_exams;n++){
                        notInPosition.push_back(initial_solution->all_exams[sorted_index[n]]);
                    }
                    available_colors = vector<int>();
                    available_colors.insert(available_colors.end(), boxes[0].begin(), boxes[0].end());
                    for(int i=0; i<n_exams; i++){
                        dim=initial_solution->all_exams[i]->conflict_exams.size();
                        initial_solution->all_exams[i]->conflict_times.assign(dim,-1);
                        initial_solution->all_exams[i]->timeslot=-1;
                    }
                    //cout<<"riga 105"<<endl;
                    initial_solution->timeslot_per_exams.assign(n_exams,-1);
                    //cout<<"esami da piazzare post failure "<<notInPosition.size()<<endl;
                }   
            }
            num=0;
        }
    }
    cout<<"finito"<<endl;
    int d=33;
    return d;
}
void piazzo_random(Solution* initial_solution, vector<int> available_colors, vector<Exam*> &notInPosition, int TRY_PLACE){
    Exam* exam;
    int i=0;
    while(i<notInPosition.size() && notInPosition.size()!=0){
        exam = notInPosition[i];
        //prova_a_inserire ritorna false se non ha inserito ritorna true se ha inserito
        if(!prova_a_inserire( initial_solution, available_colors, TRY_PLACE, notInPosition, exam)){
          //cout<<"entro in prova a inserire"<<endl;
          i++;
          break;
        }
    }
}

bool swap_random(Solution* initial_solution, vector<int> available_colors){
    int t1;
    int t2;
    int index_exam;
    vector<int> tmp;
    for(int i=0; i<initial_solution->all_exams.size(); i++){
        if(initial_solution->timeslot_per_exams[i]>0){
            tmp.push_back(i);
        }
    } 
    int index = rand() % tmp.size();
    index_exam = tmp[index];
    t1 = initial_solution->all_exams[index_exam]->timeslot;
    int index2 = rand() % available_colors.size();
    t2 = available_colors[index2]+1;
    int kk;

    if (std::find(initial_solution->all_exams[index_exam]->conflict_times.begin(), 
                initial_solution->all_exams[index_exam]->conflict_times.end(), t2) != initial_solution->all_exams[index_exam]->conflict_times.end()){
            //se ho trovato conflitto provo un'altra mossa random
            //cout<<"NON fatto swap"<<endl;
            return false;
    }
    initial_solution->timeslot_per_exams[index_exam]=t2;
    initial_solution->all_exams[index_exam]->timeslot=t2;
    for (auto jj : initial_solution->all_exams[index_exam]->conflict_exams){ 
        kk=0; 
        while(initial_solution->all_exams[jj-1]->conflict_exams[kk] != initial_solution->all_exams[index_exam]->id_exam){ 
            kk++;
        } 
        initial_solution->all_exams[jj-1]->conflict_times[kk]=t2; 
    }
    //cout<<"fatto swap"<<endl;
    
    return true;
}

bool prova_a_inserire(Solution* initial_solution,  vector<int> available_colors, int TRY_PLACE, vector<Exam*> &notInPosition, Exam* exam){
    int find_conflict;
    int kk;
    for(int k=0; k<TRY_PLACE; k++){
        //cout<<"esame PR "<<exam->id_exam<<endl;
        int index_time = rand() % available_colors.size(); // scelgo un tempo a caso
        int time = available_colors[index_time]+1;
        //cout<<"time PR "<<time<<endl;
        find_conflict=0;
        for(int j=0; j< initial_solution->all_exams[exam->id_exam-1]->conflict_times.size() && find_conflict==0; j++){
            if(initial_solution->all_exams[exam->id_exam-1]->conflict_times[j]==time){
                find_conflict=1; 
                //cout<<"non piazzo random riprova"<<endl;
                find_conflict=1;
            }
        }
    
        //if(find(initial_solution->all_exams[exam->id_exam-1]->conflict_times.begin(), 
        //initial_solution->all_exams[exam->id_exam-1]->conflict_times.end(), time)==initial_solution->all_exams[exam->id_exam-1]->conflict_times.end());
        if(find_conflict==0){
            //cout<<"piazzo random riprova"<<endl;
            //cout<<"ora tolgo "<<notInPosition.size();
            notInPosition.erase(notInPosition.begin());
            //cout<<" ora ho tolto "<<notInPosition.size()<<endl;
            initial_solution->timeslot_per_exams[exam->id_exam-1]=time;
            initial_solution->all_exams[exam->id_exam-1]->timeslot=time;
            for (auto jj : initial_solution->all_exams[exam->id_exam-1]->conflict_exams){ 
                kk=0; 
                while(initial_solution->all_exams[jj-1]->conflict_exams[kk] != initial_solution->all_exams[exam->id_exam-1]->id_exam){ 
                    kk++;
                } 
            initial_solution->all_exams[jj-1]->conflict_times[kk]=time; 
            }
            return true; 
        } 
    }
    return false;
            
}
 