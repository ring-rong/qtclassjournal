#include "mainwindow.h"
#include <QApplication>
#include <QToolBar>
#include <QAction>
#include <QFileDialog>
#include <QMessageBox>
#include <QHeaderView>
#include <QInputDialog>
#include <fstream>
#include <sstream>
#include <numeric>
#include <QColor>

MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent) {
    // Создаем таблицу и панель инструментов
    table = new QTableWidget(this);
    setCentralWidget(table);

    QToolBar* toolBar = addToolBar("Toolbar");
    toolBar->addAction("Add Student", this, &MainWindow::addStudent);
    toolBar->addAction("Add Lesson", this, &MainWindow::addSubject);
    toolBar->addAction("Add Grade", this, &MainWindow::addGrade);
    toolBar->addAction("Open File", this, &MainWindow::openFile);
    toolBar->addAction("Save File", this, &MainWindow::saveFile);
    toolBar->addAction("Clear Data", this, &MainWindow::clearData);
    toolBar->addAction("Clear Grades", this, &MainWindow::clearGrades);
    subjectBox = new QComboBox(this);
    toolBar->addWidget(subjectBox);

    updateTable();
    updateSubjectBox();
}

void MainWindow::clearData() {
    journal.students.clear();
    journal.subjects.clear();
    updateTable();
    updateSubjectBox();
}

void MainWindow::clearGrades() {
    for (auto& student : journal.students) {
        student.grades.clear();
    }
    updateTable();
}


void MainWindow::addStudent() {
    if (journal.subjects.empty()) {
        QMessageBox::critical(this, "Error", "Please add a subject before adding a student");
        return;
    }
    bool ok;
    QString name = QInputDialog::getText(this, "Add Student", "Student name:", QLineEdit::Normal, "", &ok);
    if (ok && !name.isEmpty()) {
        journal.students.push_back(Student(name.toStdString()));
        updateTable();
    }
}

void MainWindow::addSubject() {
    bool ok;
    QString subject = QInputDialog::getText(this, "Add Lesson", "Lesson name:", QLineEdit::Normal, "", &ok);
    if (ok && !subject.isEmpty()) {
        journal.subjects.push_back(subject.toStdString());
        updateTable();
        updateSubjectBox();
    }
}

void MainWindow::addGrade() {
    if (journal.students.empty() || journal.subjects.empty()) {
        QMessageBox::critical(this, "Error", "Please add a student and a subject before adding a grade");
        return;
    }
    bool ok;
    QString studentName = QInputDialog::getItem(this, "Add Grade", "Student:", getStudentNames(), 0, false, &ok);
    if (ok && !studentName.isEmpty()) {
        int grade = QInputDialog::getInt(this, "Add Grade", "Grade:", 1, 1, 5, 1, &ok);
        if (ok) {
            std::string subject = subjectBox->currentText().toStdString();
            for (auto& student : journal.students) {
                if (student.name == studentName.toStdString()) {
                    student.grades[subject].push_back(grade);
                    updateTable();
                    break;
                }
            }
        }
    }
}



void MainWindow::updateCellColor(int row, int column, double grade) {
    QColor color;
    if (grade < 3) {
        color = Qt::red;
    } else if (grade < 4) {
        color = QColor(255, 165, 0); // Orange
    } else if (grade < 5) {
        color = QColor(173, 255, 47); // GreenYellow
    } else {
        color = Qt::green;
    }

    QTableWidgetItem *item = table->item(row, column);
    if (!item) {
        item = new QTableWidgetItem();
        table->setItem(row, column, item);
    }
    item->setBackground(color);
}



void MainWindow::openFile() {
    QString fileName = QFileDialog::getOpenFileName(this, "Open CSV", "", "CSV files (*.csv)");
    if (!fileName.isEmpty()) {
        std::ifstream file(fileName.toStdString());
        if (file.is_open()) {
            // Очистка текущих данных
            journal.students.clear();
            journal.subjects.clear();

            std::string line;
            // Чтение первой строки файла (заголовки)
            if (std::getline(file, line)) {
                std::stringstream ss(line);
                std::string subject;
                // Пропуск первого заголовка ("Name")
                std::getline(ss, subject, ',');
                // Чтение остальных заголовков (предметы)
                while (std::getline(ss, subject, ',')) {
                    journal.subjects.push_back(subject);
                }
            }
            // Чтение остальных строк файла (студенты и оценки)
            while (std::getline(file, line)) {
                std::stringstream ss(line); // исправлено здесь
                std::string name;
                std::getline(ss, name, ',');
                Student student(name);
                std::string gradeStr;
                for (const auto& subject : journal.subjects) {
                    if (std::getline(ss, gradeStr, ',')) {
                        student.grades[subject].push_back(std::stoi(gradeStr));
                    }
                }
                journal.students.push_back(student);
            }

            file.close();
            updateTable();
            updateSubjectBox();
        } else {
            QMessageBox::critical(this, "Error", "Failed to open file");
        }
    }
}

void MainWindow::saveFile() {
    QString fileName = QFileDialog::getSaveFileName(this, "Save CSV", "", "CSV files (*.csv)");
    if (!fileName.isEmpty()) {
        std::ofstream file(fileName.toStdString());
        if (file.is_open()) {
            // Запись заголовков
            file << "Name";
            for (const auto& subject : journal.subjects) {
                file << "," << subject;
            }
            file << "\n";
            // Запись студентов и оценок
            for (const auto& student : journal.students) {
                file << student.name;
                for (const auto& subject : journal.subjects) {
                    file << ",";
                    if (student.grades.count(subject) > 0) {
                        for (const auto& grade : student.grades.at(subject)) {
                            file << grade;
                        }
                    }
                }
                file << "\n";
            }

            file.close();
        } else {
            QMessageBox::critical(this, "Error", "Failed to save file");
        }
    }
}

void MainWindow::updateTable() {
    table->clear();
    table->setRowCount(journal.students.size());
    table->setColumnCount(journal.subjects.size() + 2); // один столбец для имени студента и один для средней оценки
    for (int i = 0; i < journal.students.size(); ++i) {
        table->setItem(i, 0, new QTableWidgetItem(QString::fromStdString(journal.students[i].name)));
        int gradesTotalCount = 0;
        double gradesTotalSum = 0.0;
        for (int j = 0; j < journal.subjects.size(); ++j) {
            const auto& grades = journal.students[i].grades[journal.subjects[j]];
            if (!grades.empty()) {
                int sum = std::accumulate(grades.begin(), grades.end(), 0);
                double average = static_cast<double>(sum) / grades.size();
                gradesTotalSum += sum;
                gradesTotalCount += grades.size();
                table->setItem(i, j + 1, new QTableWidgetItem(QString::number(average)));
                updateCellColor(i, j + 1, average);
            }
        }
        if (gradesTotalCount > 0) {
            double totalAverage = gradesTotalSum / gradesTotalCount;
            table->setItem(i, journal.subjects.size() + 1, new QTableWidgetItem(QString::number(totalAverage)));
            updateCellColor(i, journal.subjects.size() + 1, totalAverage);
        }
    }
    QStringList headers;
    headers << "Name";
    for (const auto& subject : journal.subjects) {
        headers << QString::fromStdString(subject);
    }
    headers << "Average";
    table->setHorizontalHeaderLabels(headers);
}



void MainWindow::updateSubjectBox() {
    subjectBox->clear();
    for (const auto& subject : journal.subjects) {
        subjectBox->addItem(QString::fromStdString(subject));
    }
}

QStringList MainWindow::getStudentNames() {
    QStringList names;
    for (const auto& student : journal.students) {
        names << QString::fromStdString(student.name);    }
    return names;
}
