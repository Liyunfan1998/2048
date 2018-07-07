#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent) :
        QMainWindow(parent),
        timeTillNow(0),
        grid(),
        ai(&grid),
        aiPlay(false){
    ShowItem(grid.GetGrid());

    timer = new QTimer(this);
    aiTimer = new QTimer(this);

    timer->start(1000);
    aiTimer->start(200);
}

void MainWindow::GetMove() {
    if (aiPlay) {
        ui->move->display(DoMove());
        if (ai.count > 0) {
            ai.count--;
        }
        AddNumber();
    }
}

int MainWindow::DoMove() {
    if (aiPlay) {
        switch (ai.GetBest(4, -10000, 10000).first) {
            case 1:
                HandleUp();
                return 1;
            case 2:
                HandleRight();
                return 2;
            case 3:
                HandleDown();
                return 3;
            case 4:
                HandleLeft();
                return 4;
            default:
                return 5;
        }
    } else
        return 5;
}

MainWindow::~MainWindow() {
    delete timer;
    delete aiTimer;
    delete ui;
}

void MainWindow::updateLCD() {
}

void MainWindow::updateTimer() {
}

void MainWindow::keyPressEvent(QKeyEvent *k) {
    if (aiPlay) {
        return;
    }
    if (k->key() == Qt::Key_W || k->key() == Qt::Key_Up) {
        grid.SetLastPress(Grid::UP);
        HandleUp();
    } else if (k->key() == Qt::Key_S || k->key() == Qt::Key_Down) {
        grid.SetLastPress(Grid::DOWN);
        HandleDown();
    } else if (k->key() == Qt::Key_D || k->key() == Qt::Key_Right) {
        grid.SetLastPress(Grid::RIGHT);
        HandleRight();
    } else if (k->key() == Qt::Key_A || k->key() == Qt::Key_Left) {
        grid.SetLastPress(Grid::LEFT);
        HandleLeft();
    } else {
        return;
    }
    AddNumber();
}

QString MainWindow::Display(int temp) {
    switch (temp) {
        case 0:
            return 0;
        case 2:
            return ":/image/2";
        case 4:
            return ":/image/4";
        case 8:
            return ":/image/8";
        case 16:
            return ":/image/16";
        case 32:
            return ":/image/32";
        case 64:
            return ":/image/64";
        case 128:
            return ":/image/128";
        case 256:
            return ":/image/256";
        case 512:
            return ":/image/512";
        case 1024:
            return ":/image/1024";
        case 2048:
            return ":/image/2048";
        default:
            return 0;
    }
}

void MainWindow::HandleUp() {
    grid.HandleUp();
    ShowItem(grid.GetGrid());
}

void MainWindow::HandleDown() {
    grid.HandleDown();
    ShowItem(grid.GetGrid());
}

void MainWindow::HandleLeft() {
    grid.HandleLeft();
    ShowItem(grid.GetGrid());
}

void MainWindow::HandleRight() {
    grid.HandleRight();
    ShowItem(grid.GetGrid());
}


void MainWindow::Restart() {
    grid.Restart();
    ShowItem(grid.GetGrid());
    timeTillNow = 0;
    timer->start();
}

void MainWindow::Undo() {
    grid.Undo();
    timer->start();
    ShowItem(grid.GetGrid());
}

void MainWindow::AddNumber() {
    if (!grid.Equal()) {
        grid.AddNumber();
        ShowItem(grid.GetGrid());
    } else {
        grid.CheckLose();
    }
    if (grid.IsLose()) {
        ShowLose();
        timer->stop();
    }
}

void MainWindow::ShowLose() {
}
