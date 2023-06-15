#ifndef STUDENT_H
#define STUDENT_H

#include <string>
#include <map>
#include <vector>

class Student {
public:
    std::string name;
    std::map<std::string, std::vector<int>> grades;

    Student(const std::string& name);
};

#endif // STUDENT_H
