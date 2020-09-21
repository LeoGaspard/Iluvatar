#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QFileDialog>
#include <QInputDialog>
#include <QColorDialog>
#include <QColor>
#include <QLabel>
#include <vector>
#include <iostream>
#include "src/molecule.h"

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void on_toolButtonFile_clicked();

    void on_toolButtonSymmetry_clicked();

    void on_toolButtonColor_clicked();

    void on_toolButtonSymmetrize_clicked();

private:
    Ui::MainWindow *ui;
    Molecule* mol;
    QString fileName;
};

#endif // MAINWINDOW_H
