#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "schooljournal.h"
#include <QMainWindow>
#include <QTableWidget>
#include <QComboBox>

class MainWindow : public QMainWindow {
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    QStringList getStudentNames();
private slots:
    void addStudent();
    void addSubject();
    void addGrade();
    void openFile();
    void saveFile();

private:
    SchoolJournal journal;
    QTableWidget* table;
    QComboBox* subjectBox;
    void clearData();
    void clearGrades();
    void updateTable();
    void updateSubjectBox();
    void updateCellColor(int row, int column, double grade);
};

#endif // MAINWINDOW_H
