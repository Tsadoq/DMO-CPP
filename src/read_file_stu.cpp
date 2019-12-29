#include "header.hpp"
#include <fstream>
#include <iostream>
#include <string>
#include <algorithm>
#include <iterator>
#include <vector>

using namespace std;

vector<vector<int>> read_file_stu(char* file_name,int n_exams){
    //vector<string> student_ids; 
    //int conflict_matrix;
    vector<vector<int>> conflict_matrix = vector<vector<int>> (n_exams,vector<int>(n_exams, 0));
    vector <int> exam_ids; 
    vector <int> exams_for_student;
    FILE *fp;
    char stud_id[10];
    string stud_id_string="";
    int exam_id; 

    fp = fopen(file_name, "r");
    
    string current_student="";
    int count_exams_for_student=0;
    // ASK TADEI IF IN ALL INSTANCES ROWS ARE SORTED BY STUDENTS ID
    //reading of .exm to know how many exams there are
    while(fscanf(fp, "%s\t%d", stud_id, &exam_id)!=EOF){
        stud_id_string=stud_id;
        if (stud_id_string.compare(current_student)!=0){
            exams_for_student.push_back(count_exams_for_student);
            count_exams_for_student=0;
            current_student.assign(stud_id_string);
        }
        //student_ids.push_back(stud_id);
        exam_ids.push_back(exam_id);
        count_exams_for_student++;
    }
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
