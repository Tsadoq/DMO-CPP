#include "header.hpp"
#include <fstream>
#include <iostream>
#include <string>


vector<double> read_file_exm(char* file_name,int &n_exams){
    int exam_id; 
    int num_st;
    vector <double> student_per_exams=vector<double>();
    FILE *fp;

    fp = fopen(file_name, "r");
    //reading of .exm to know how many exams there are
    while(fscanf(fp, "%d\t%d", &exam_id,&num_st)!=EOF){
        n_exams++;
        student_per_exams.push_back(num_st);
    }
    fclose(fp);
    return student_per_exams;
}