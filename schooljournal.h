#ifndef SCHOOLJOURNAL_H
#define SCHOOLJOURNAL_H

#include "student.h"
#include <vector>

class SchoolJournal {
public:
    std::vector<Student> students;
    std::vector<std::string> subjects;
};

#endif // SCHOOLJOURNAL_H
