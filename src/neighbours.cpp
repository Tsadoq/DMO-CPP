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
    while(t2==t1 && abs(t2-t1)>5)
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
    //int flag = solution->check_feasibility(solution->timeslot_per_exams, solution->all_exams);
    //cout<<"The swapped solution is feasible? "<<flag<<endl;    
    return;

}

void neighbour_by_crossover(Solution* actual_sol,Solution* best_sol, int n_exams, int n_timeslot){
    vector<int> best_sol_ts=best_sol->timeslot_per_exams;
    vector<int> timeslot_crossover=vector<int>(n_exams, 0);
    int first_cut=round(n_exams/5.0);
    int second_cut=round(4.0*n_exams/5.0);
    for(int i=first_cut;i<second_cut;i++){
        timeslot_crossover[i]=best_sol->timeslot_per_exams[i];
    }
    int exam_no_ok = 0;

    for(int j=second_cut;j<n_exams;j++){
        // gli esami nella actual_sol non sono tra loro in conflitto, devo solo controllare che non siano in conflitto con la best_sol
        if(find(best_sol->all_exams[j]->conflict_times.begin(), best_sol->all_exams[j]->conflict_times.end(),
         actual_sol->timeslot_per_exams[j]) == best_sol->all_exams[j]->conflict_times.end()){
             timeslot_crossover[j]=actual_sol->timeslot_per_exams[j];
        } else {
            exam_no_ok++;
        }
    }
    for(int j=0;j<first_cut;j++){
        // gli esami nella actual_sol non sono tra loro in conflitto, devo solo controllare che non siano in conflitto con la best_sol
        if(find(best_sol->all_exams[j]->conflict_times.begin(), best_sol->all_exams[j]->conflict_times.end(),
         actual_sol->timeslot_per_exams[j]) == best_sol->all_exams[j]->conflict_times.end()){
             timeslot_crossover[j]=actual_sol->timeslot_per_exams[j];
        } else{
            exam_no_ok++;
        }
    }
    actual_sol->timeslot_per_exams=timeslot_crossover;
    actual_sol->update_timeslots(n_exams);
    int all_exam_ok=0;
    for (int k=0;k<n_exams;k++){
        if(actual_sol->timeslot_per_exams[k]==0){
            for (int t=1;t<=n_timeslot;t++){
                if (std::find(actual_sol->all_exams[k]->conflict_times.begin(), 
                actual_sol->all_exams[k]->conflict_times.end(), t) == actual_sol->all_exams[k]->conflict_times.end()){
                    actual_sol->timeslot_per_exams[k]=t;
                    all_exam_ok++;
                    actual_sol->update_timeslots(n_exams);
                    t=n_timeslot;
                }

            }
        }

    }
    if(all_exam_ok< exam_no_ok){
        actual_sol->timeslot_per_exams=best_sol_ts;
    }
    actual_sol->update_timeslots(n_exams);
}


void neighbours_by_double_mutation(Solution* solution, int totTimeslots, int numMutation){
    // funzione che serve a modificare la soluzione tentando di spostare esami in timeslot a prima vista non disponibili, ma che diventano disponibili facilmente
    // cerco esami che hanno timeslot in cui hanno un solo conflitto
    int counter = 0;
    int counterMutation = 0;
    vector<int> times;
    vector<int> possible_times;
    vector<int> possible_timeslots;
    // per gli spostamenti del scondo esame considerato
    for (int i=0; i<totTimeslots;i++){
        possible_timeslots.push_back(i+1);
    }
    vector<vector<int>> tabulist; // lista che tiene conto degli esami gia' spostati cosi, in modo da evitare secondo scambio
    while (counterMutation < numMutation && counter < solution->timeslot_per_exams.size()){
        int i = rand() % solution->timeslot_per_exams.size(); // scelgo un esame a caso
        times = vector<int>(totTimeslots,0);
        for (int j = 0; j < solution->all_exams[i]->conflict_times.size(); j++){
            times[ solution->all_exams[i]->conflict_times[j]-1 ]++;
        }
        // in questo momento in times ci sono il numero di conflitti dell'esame i in ogni timeslot

        // scelgo a caso, se c'e' un timeslot tra gli unavaiable che me lo permette
        
        for (int j= 0; j < totTimeslots; j++){
            if (times[j]== 1){
                possible_times.push_back(j); // salvo le posizioni dei timeslot che hanno un solo conflitto con l'esame i
            }
        }
        // in possible times ci sono i timeslot che hanno 1 solo conflitto con l'esame i
        if (possible_times.size() > 0){
            cout<<"I found a timeslot where n of conflicts is 1"<<endl;
            int selected_time = rand() % possible_times.size();
            int conflict_index = -1;
            // se l'esame che e' in conlitto può spostarsi, allora sposto lui e posso inserire l'esame i nel timeslot selected time
            for (int j= 0; j <solution->all_exams[i]->conflict_times.size(); j++){
                if (solution->all_exams[i]->conflict_times[j]== selected_time){
                    conflict_index = j; // cerco la posizione dell'esame in conflitto 
                }
            }
            vector<int> possible_mutation_for_second_exam;
            vector<int> not_available_sec = solution->all_exams[conflict_index]->conflict_times;
            sort(not_available_sec.begin(), not_available_sec.end()); 
            // find available timeslot 
            set_difference(possible_timeslots.begin(), possible_timeslots.end(), not_available_sec.begin(),  
                            not_available_sec.end(),inserter(possible_mutation_for_second_exam, possible_mutation_for_second_exam.begin()));
            
            vector<int> toavoid = {i, conflict_index};
            vector<int> toavoidreverse = {conflict_index, i};
            if (possible_mutation_for_second_exam.size()> 0 && find(tabulist.begin(), tabulist.end(), toavoid) == tabulist.end() && find(tabulist.begin(), tabulist.end(), toavoidreverse) == tabulist.end() ){
                // se questo swap non era gia stato fatto
                cout<<"I'm double swapping exams: "<<i<<" and "<<conflict_index<<endl;
                tabulist.push_back(toavoid);
                solution->all_exams[i]->timeslot = possible_times[selected_time];
                solution-> all_exams[conflict_index]->timeslot = possible_mutation_for_second_exam[ rand() % possible_mutation_for_second_exam.size() ];
                counterMutation++;
                solution->update_timeslots(solution->all_exams.size());
                cout<<"number of mutation is "<<counterMutation<<endl;
            }
            possible_mutation_for_second_exam.clear();
            not_available_sec.clear();  
        }
        counter++;
        possible_times.clear();
    }
}
/*
int double_mutation(Solution* solution, int totTimeslots){
    // funzione che serve a modificare la soluzione tentando di spostare esami in timeslot a prima vista non disponibili, ma che diventano disponibili facilmente
    // cerco esami che hanno timeslot in cui hanno un solo conflitto
    vector<int> times;
    vector<int> possible_times;
    vector<int> possible_timeslots;
    // per gli spostamenti del scondo esame considerato
    for (int i=0; i<totTimeslots;i++){
        possible_timeslots.push_back(i+1);
    }
    int i = rand() % solution->timeslot_per_exams.size(); // scelgo un esame a caso
    times = vector<int>(totTimeslots,0);
    for (int j = 0; j < solution->all_exams[i]->conflict_times.size(); j++){
        times[ solution->all_exams[i]->conflict_times[j]-1 ]++;
    }
    for (int j= 0; j < totTimeslots; j++){
        if (times[j] == 1){
            possible_times.push_back(j+1); // salvo i timeslot che hanno un solo conflitto con l'esame i (da 1 a n_timeslots)
        }
    }
    // in possible times ci sono i timeslot che hanno 1 solo conflitto con l'esame i
    if (possible_times.size() > 0){
        int selected_time = rand() % possible_times.size();
        int conflict_index = -1;
        // se l'esame che e' in conlitto può spostarsi, allora sposto lui e posso inserire l'esame i nel timeslot selected time
        for (int j= 0; j <solution->all_exams[i]->conflict_times.size(); j++){
            if (solution->all_exams[i]->conflict_times[j]== possible_times[selected_time]){
                conflict_index = j; // cerco la posizione dell'esame in conflitto 
                break;
                }
        }
        vector<int> possible_mutation_for_second_exam;
        vector<int> not_available_sec = solution->all_exams[conflict_index]->conflict_times;
        sort(not_available_sec.begin(), not_available_sec.end());  
        set_difference(possible_timeslots.begin(), possible_timeslots.end(), not_available_sec.begin(),  
                            not_available_sec.end(),inserter(possible_mutation_for_second_exam, possible_mutation_for_second_exam.begin()));
        if (possible_mutation_for_second_exam.size()> 0 ){
            // se questo swap non era gia stato fatto
            int rand_ind = rand() % possible_mutation_for_second_exam.size();
            solution->all_exams[i]->timeslot = possible_times[selected_time];
            solution-> all_exams[conflict_index]->timeslot = possible_mutation_for_second_exam[rand_ind ];
            solution->update_timeslots(solution->all_exams.size());
            cout<<endl;
            return 1;
        }  
    }
    return 0;
}
*/

int double_mutation(Solution* solution, int totTimeslots){
    // funzione che serve a modificare la soluzione tentando di spostare esami in timeslot a prima vista non disponibili, ma che diventano disponibili facilmente
    // cerco esami che hanno timeslot in cui hanno un solo conflitto
    vector<int> times;
    vector<int> possible_times;
    vector<int> possible_timeslots;
    // per gli spostamenti del scondo esame considerato
    for (int i=0; i<totTimeslots;i++){
        possible_timeslots.push_back(i+1);
    }
    int i = rand() % solution->timeslot_per_exams.size(); // scelgo un esame a caso
    times = vector<int>(totTimeslots,0);
    for (int j = 0; j < solution->all_exams[i]->conflict_times.size(); j++){
        times[ solution->all_exams[i]->conflict_times[j]-1 ]++;
    }
    for (int j= 0; j < totTimeslots; j++){
        if (times[j] == 1){
            possible_times.push_back(j+1); // salvo i timeslot che hanno un solo conflitto con l'esame i (da 1 a n_timeslots)
        }
    }
    // in possible times ci sono i timeslot che hanno 1 solo conflitto con l'esame i
    if (possible_times.size() > 0){
        int selected_time = rand() % possible_times.size();
        int conflict_index = -1;
        // se l'esame che e' in conlitto può spostarsi, allora sposto lui e posso inserire l'esame i nel timeslot selected time
        for (int j= 0; j <solution->all_exams[i]->conflict_times.size(); j++){
            if (solution->all_exams[i]->conflict_times[j]== possible_times[selected_time]){
                conflict_index = j; // cerco la posizione dell'esame in conflitto 
                break;
                }
        }
        vector<int> possible_mutation_for_second_exam;
        vector<int> not_available_sec = solution->all_exams[conflict_index]->conflict_times;
        sort(not_available_sec.begin(), not_available_sec.end());  
        set_difference(possible_timeslots.begin(), possible_timeslots.end(), not_available_sec.begin(),  
                            not_available_sec.end(),inserter(possible_mutation_for_second_exam, possible_mutation_for_second_exam.begin()));
        if (possible_mutation_for_second_exam.size()> 0 ){
            // se questo swap non era gia stato fatto
            int rand_ind = rand() % possible_mutation_for_second_exam.size();
            solution->all_exams[i]->timeslot = possible_times[selected_time];
            solution-> all_exams[conflict_index]->timeslot = possible_mutation_for_second_exam[rand_ind ];
            solution->update_timeslots(solution->all_exams.size());
            cout<<endl;
            return 1;
        }  
    }
    return 0;
}

bool unscheduling(Solution* sol, int num_unsched){
    vector<int> positions;
    int h;
    if (num_unsched==0){
        return true;
    }
    for (int i= 0; i<sol->all_exams.size(); i++){
        positions.push_back(i);
    }
    unsigned seed = std::chrono::system_clock::now().time_since_epoch().count();    
    std::shuffle(positions.begin(), positions.end(),std::default_random_engine(seed) );
    for (int j= 0; j < num_unsched; j++){
        sol->timeslot_per_exams[positions[j]] = -1; 
        sol->all_exams[positions[j]]->timeslot = -1;
         for (auto k : sol->all_exams[positions[j]]->conflict_exams){ 
                h=0; 
                while(sol->all_exams[k-1]->conflict_exams[h] != sol->all_exams[positions[j]]->id_exam){ 
                    h++;
                } 
                sol->all_exams[k-1]->conflict_times[h]= -1; 
            }
    }
    return false;
}

bool rescheduling(Solution* sol, int totTimeslots, int n_exams){

    int fail=0;
    int max_fail=n_exams/3;
    vector<Exam*> unsched_exams=vector<Exam*>();
    int k;
    int counter = 0; 

    for (int i =0; i< sol->all_exams.size(); i++){
        if (sol->timeslot_per_exams[i] == -1){
            unsched_exams.push_back(sol->all_exams[i]);
        }
        sol->all_exams[i]->num_conflict = sol->all_exams[i]->conflict_exams.size();
    }   

    vector<int> possible_timeslots=vector<int>();
    for (int i=0; i<totTimeslots;i++){  // possible timeslot rimane uguale per tutti, e il vettore [1, ntimeslot]
            possible_timeslots.push_back(i+1);
    }

    vector<int> not_available;
    vector<int> possible_mutation;
    vector<size_t> order_not_schedule;
    vector<int> not_available_time;
    vector<int> available_time;



    int pos;
    int kk;
    int rand_index;
    int pos_conf;
    
    while (unsched_exams.size()!=0){
        if (fail>=max_fail){
            return false;
        }

        not_available=vector<int>();
        possible_mutation=vector<int>();
        not_available_time=vector<int>();
        available_time=vector<int>();
        order_not_schedule=vector<size_t>();

        for(auto un_ex: unsched_exams){
            not_available_time=un_ex->conflict_times;
            std::sort(not_available_time.begin(), not_available_time.end());  
            std::set_difference(possible_timeslots.begin(), possible_timeslots.end(), not_available_time.begin(),  
                    not_available_time.end(),inserter(available_time, available_time.begin()));
                    un_ex->available_times=available_time.size();
            available_time=vector<int>();
            not_available_time=vector<int>();
        }

        std::sort(unsched_exams.begin(), unsched_exams.end(), [](Exam* a, Exam* b) {
            if ( a->available_times!= b->available_times) return a->available_times < b->available_times;
                return a->num_conflict > b->num_conflict;
        });
        pos = unsched_exams[0]->id_exam-1;
        // vado a vedere quali timeslot sono disponibili
        for (int j = 0; j <sol->all_exams[pos]->conflict_times.size(); j++){
            if (sol->all_exams[pos]->conflict_times[j] != -1){
                not_available.push_back(sol->all_exams[pos]->conflict_times[j]);  // mi segno i timeslot non disponibili
            }    
        }
        std::sort(not_available.begin(), not_available.end());  
        std::set_difference(possible_timeslots.begin(), possible_timeslots.end(), not_available.begin(),  
                    not_available.end(),inserter(possible_mutation, possible_mutation.begin()));
        if (possible_mutation.size()> 0){
            rand_index = rand() % possible_mutation.size();
            sol->all_exams[pos]->timeslot = possible_mutation[rand_index];
            sol->timeslot_per_exams[pos] = possible_mutation[rand_index];
            for (auto h : sol->all_exams[pos]->conflict_exams){ 
                k=0; 
                while(sol->all_exams[h-1]->conflict_exams[k] != sol->all_exams[pos]->id_exam){ 
                    k++;
                } 
                sol->all_exams[h-1]->conflict_times[k]=possible_mutation[rand_index];
            } 
            unsched_exams.erase(unsched_exams.begin());
        }else{
            fail++;
            for (int j = 0; j <sol->all_exams[pos]->conflict_times.size(); j++){
                if(sol->all_exams[pos]->conflict_times[j]!=-1){                    
                    sol->all_exams[pos]->conflict_times[j]=-1;
                    pos_conf=sol->all_exams[pos]->conflict_exams[j]-1;
                    unsched_exams.push_back(sol->all_exams[pos_conf]);
                    sol->timeslot_per_exams[pos_conf]=-1;
                    sol->all_exams[pos_conf]->timeslot=-1;
                    for(auto conf_exam : sol->all_exams[pos_conf]->conflict_exams){
                        kk=0; 
                        while(sol->all_exams[conf_exam-1]->conflict_exams[kk] != pos_conf+1){ 
                            kk++;
                        } 
                        sol->all_exams[conf_exam-1]->conflict_times[kk]=-1;                        
                    }
                }
            }    
        }
    }
    return true;
}

int directional_mutation(Solution* sol, int totTimeslots){
    int rand_ind;
    int counter = 0;
    int k;
    int flag = 0;
    while (flag == 0 && counter < 10){
        rand_ind = rand() % sol->all_exams.size();
        flag = sol->probability_creator(totTimeslots, rand_ind); // trovo il timeslot in cui posso muovermi
        counter++;
    }
    if (counter == 10)
        return 0;
    // In questo momento in flag c'e' il timeslot che diminuisce la penalita' dell'esame in questione
    sol->timeslot_per_exams[rand_ind] = flag;  // cambio nel vettore 
    sol->all_exams[rand_ind]->timeslot = flag;  // e all'interno dell'esame
    for (auto j : sol->all_exams[rand_ind]->conflict_exams){ 
            k=0; 
            while(sol->all_exams[j-1]->conflict_exams[k] != sol->all_exams[rand_ind]->id_exam){ 
                k++;
            } 
            sol->all_exams[j-1]->conflict_times[k]= flag;  // modifico nei vicini
    } 
    return 1;

}

double directional_mutation_final(Solution* sol, int totTimeslots, double best_obj, int num_stud){
    int flag;
    double delta;
    int k;
    int i;
    vector<double> weight_for_exams=sol->update_weights(sol->all_exams.size());
    vector<size_t> order_for_mutation = sort_indexes(weight_for_exams);
    for (int h = 0; h < order_for_mutation.size(); h++){
        i = order_for_mutation[h];
        flag = sol->probability_creator(totTimeslots, i); // trovo il timeslot in cui posso muovermi
        if (flag != 0){
            delta = 0;
            // Devo calcolare in maniera furba la nuova penalita', tengo traccia solo degli esami che possono portare a cambiamenti
            for (int j = 0; j< sol->all_exams[i]->conflict_times.size(); j++){
                // calcolo per ogni conflitto dell'esame i come cambia la penalita contro l'esame j
                delta = delta + 2*( pow(2, 5-abs(sol->all_exams[i]->conflict_times[j] - flag)) - pow(2, 5-abs(sol->all_exams[i]->conflict_times[j] - sol->all_exams[i]->timeslot)) )* sol->all_exams[i]->conflict_weights[j];
                // in pratica, per ogni conflitto vado a vedere come sarebbe la penalita rispetto a come e adesso, il risultato moltiplicato per due perche cosi tengo conto anche del cambiamento tra j e i
            }
            cout<<"Delta is "<<delta<<endl;
            if (delta < 0){
                cout<<"Hey, I am improving"<<endl;
                // mi conviene fare questo cambiamento
                sol->all_exams[i]->timeslot = flag;
                sol->timeslot_per_exams[i] = flag;
                for (auto j : sol->all_exams[i]->conflict_exams){ 
                    k=0; 
                    while(sol->all_exams[j-1]->conflict_exams[k] != sol->all_exams[i]->id_exam){ 
                        k++;
                    } 
                    sol->all_exams[j-1]->conflict_times[k]= flag;  // modifico nei vicini
                } 
                // NB: non serve piu' cambiare i weights, aggiorno solo la fo
                best_obj = best_obj + 2*delta/num_stud; 
            }
            // Altrimenti non fa nulla e passa all'esame successivo
        }
    }
    cout<<"I have been there"<<endl;
    return best_obj;
}


