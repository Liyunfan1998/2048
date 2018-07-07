#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QtCore>
#include <QKeyEvent>
#include "grid.h"
#include "ai.h"



namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();
    void keyPressEvent( QKeyEvent *k );

public slots:
    void updateLCD();
    void updateTimer();
    void GetMove();

private:
    Ui::MainWindow *ui;
    int timeTillNow;
    Grid grid;
    Ai ai;
    bool aiPlay;


    void ShowItem(int *t);
    void ShowLose();
    QString Display(int temp);
    void HandleUp();
    void HandleDown();
    void HandleLeft();
    void HandleRight();
    void Restart();
    void Undo();
    void AiSwitch(){aiPlay = !aiPlay;}
    void Hint();
    int DoMove();
    void AddNumber();
    QTimer *timer;
    QTimer *aiTimer;
};

#endif // MAINWINDOW_H
