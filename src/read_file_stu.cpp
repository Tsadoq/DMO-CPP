#include "header.hpp"
#include <fstream>
#include <iostream>
#include <string>
#include <algorithm>
#include <iterator>
#include <vector>



std::vector<std::vector<int>> read_file_stu(char* file_name,int n_exams, int &total_number_students){
    std::vector<std::vector<int>> conflict_matrix = std::vector<std::vector<int>> (n_exams,std::vector<int>(n_exams, 0));
    std::vector <int> exam_ids; 
    std::vector <int> exams_for_student;
    FILE *fp;
    char stud_id[10];
    std::string stud_id_string="";
    int exam_id;


    fp = fopen(file_name, "r");
    if(fp == NULL){
        std::cerr << file_name << " file not found" << std::endl;
        exit(EXIT_FAILURE); 
    }
    std::string current_student="";
    int count_exams_for_student=0;
    //reading of .exm to know how many exams there are
    while(fscanf(fp, "%s\t%d", stud_id, &exam_id)!=EOF){
        stud_id_string=stud_id;
        if (stud_id_string.compare(current_student)!=0){
            total_number_students++;
            exams_for_student.push_back(count_exams_for_student);
            count_exams_for_student=0;
            current_student.assign(stud_id_string);
        }
        //student_ids.push_back(stud_id);
        exam_ids.push_back(exam_id);
        count_exams_for_student++;
    }
    exams_for_student.push_back(count_exams_for_student);
    fclose(fp);

    
    int line_student=0;
    for (int i=0; i<exams_for_student.size(); i++){
        for(int j=0; j<exams_for_student[i]-1;j++){
            for(int k=j+1; k<exams_for_student[i]; k++){
                conflict_matrix[exam_ids[line_student+j]-1][exam_ids[line_student+k]-1]++;
                conflict_matrix[exam_ids[line_student+k]-1][exam_ids[line_student+j]-1]++;
            }
        }
        line_student+=exams_for_student[i];
    }

    return conflict_matrix;
}
