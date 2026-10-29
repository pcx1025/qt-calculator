#include "mainwindow.h"

#include <QStatusBar>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
{
    setWindowTitle("计算器");
    statusBar()->showMessage("就绪");
}

MainWindow::~MainWindow()
{
}

// 槽函数与核心逻辑将在后续提交中实现
void MainWindow::btnNumClicked() {}
void MainWindow::btnBinaryOperatorClicked() {}
void MainWindow::btnUnaryOperatorClicked() {}
void MainWindow::btnEqualClicked() {}
void MainWindow::btnClearClicked() {}
void MainWindow::btnCEClicked() {}
void MainWindow::btnDelClicked() {}
void MainWindow::btnDotClicked() {}
void MainWindow::btnSignClicked() {}
void MainWindow::keyPressEvent(QKeyEvent *) {}
