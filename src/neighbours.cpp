#include "header.hpp" 
#include <string> 
#include <iostream> 
#include <numeric>  
#include <random>   
#include <algorithm> 
#include <vector> 
#include <chrono>
#include <iterator> 
#include "Exam.hpp" 
#include "Solution.hpp" 
 
using namespace std; 
 
vector<vector<int>> neighbours_by_mutation(Solution* solution, vector<size_t> order_for_mutation,int num_mutation, 
                    vector<int> possible_timeslots,double perc,int n_exams){ 
                         
    vector<vector<int>> mutations_vector;  
    vector<int> single_mutation=vector<int> (2);                                        
    Exam* exam_mutate; 
    int is_void=0; 
    vector<int> not_available_timeslots; 
    vector<int> available_timeslots; 
    int k; 
    int new_timeslot; 
    int randomIndex; 
    int size_random=(int) (perc*n_exams); 
    std::vector<int> indexes= vector<int>(size_random); 
    std::iota(indexes.begin(), indexes.end(), 0); 
    // obtain a time-based seed:
    unsigned seed = std::chrono::system_clock::now().time_since_epoch().count();    
    std::shuffle(indexes.begin(), indexes.end(),std::default_random_engine(seed) ); 
    //std::random_shuffle(indexes.begin(), indexes.end()); 
         
    for(int i=0;i<num_mutation+is_void && i<size_random;i++){ 
        // exam I'm trying to mutate 
        available_timeslots=vector<int> (); 
        exam_mutate=solution->all_exams[order_for_mutation[indexes[i]]];
        not_available_timeslots=exam_mutate->conflict_times; 
        not_available_timeslots.push_back(exam_mutate->timeslot); 
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
            // update timeslot in all exam in conflict with respect to exam_mutate 
            for (auto j : exam_mutate->conflict_exams){ 
                k=0; 
                while(solution->all_exams[j-1]->conflict_exams[k] != exam_mutate->id_exam){ 
                    k++;
                } 
                solution->all_exams[j-1]->conflict_times[k]=new_timeslot; 
            } 
            // insert exam I want to mutate 
            single_mutation[0]=exam_mutate->id_exam; 
            // insert time slot in which I want to schedule exam 
            single_mutation[1]=new_timeslot; 
            solution->timeslot_per_exams[exam_mutate->id_exam-1]=new_timeslot;       
            mutations_vector.push_back(single_mutation); 
        } 
    } 
    return mutations_vector; 
}


void neighbours_by_swapping(Solution* solution, int totTimeslots){
    vector<int> exams = solution->timeslot_per_exams;
    // seleziono due diversi timeslot a caso
    int t1 = 1 + rand() % totTimeslots;
    int t2 = 1 + rand() % totTimeslots;
    while(t2==t1)
        t2 = 1 + rand() % totTimeslots;
    //cout<<"timeslot to be swapped: "<<t1<<" and: "<<t2<<endl;
    // vettori di indici di esami che stanno nello stesso timeslot rispettivamente in t1 e in t2
    vector<int> e1;
    vector<int> e2;
    for (int i = 0; i<exams.size(); i++){
        if (exams[i] == t1){
            e1.push_back(i);
        }
        if (exams[i] == t2){
            e2.push_back(i);
        }
    }

    // cerco gli esami in e2 che sono in conflitto con gli esami in t1
    vector<int> toSwappedIn1;
    // cerco gli esami in e1 che sono in conflitto con gli esami in t2
    vector<int> toSwappedIn2;
    for (int i = 0; i< e1.size(); i++){
        // cerco un esame conflittuale piazzato in t2
        if ( std::find(solution->all_exams[e1[i]]->conflict_times.begin(), 
                solution->all_exams[e1[i]]->conflict_times.end(), t2) != solution->all_exams[e1[i]]->conflict_times.end()){
            // ho trovato un esame conflittuale in t2, quindi l'esame con indice e1[i] va inserito
            toSwappedIn2.push_back(e1[i]);
        }
    }

    for (int i = 0; i< e2.size(); i++){
        if (std::find(solution->all_exams[e2[i]]->conflict_times.begin(), 
                solution->all_exams[e2[i]]->conflict_times.end(), t1) != solution->all_exams[e2[i]]->conflict_times.end()){
            // ho trovato un esame conflittuale in t1, quindi l'esame con indice e2[i] va inserito
            toSwappedIn1.push_back(e2[i]);
        }
    }

    for (int i = 0; i < toSwappedIn1.size(); i++){
        solution->all_exams[toSwappedIn1[i]]->timeslot = t1;
        solution->timeslot_per_exams[toSwappedIn1[i]]=t1;
    }
    for (int i = 0; i < toSwappedIn2.size(); i++){
        solution->all_exams[toSwappedIn2[i]]->timeslot = t2;
        solution->timeslot_per_exams[toSwappedIn2[i]]=t2;
    }
    solution->update_timeslots(solution->all_exams.size());
    int flag = solution->check_feasibility(solution->timeslot_per_exams, solution->all_exams);
    //cout<<"The swapped solution is feasible? "<<flag<<endl;    
    return;

}