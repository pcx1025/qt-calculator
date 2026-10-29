#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <QPushButton>
#include <QStatusBar>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    // 界面（控件、布局、样式表）全部由 UI 设计器的 .ui 文件生成
    ui->setupUi(this);
    setWindowTitle("计算器");

    // ---- 信号与槽连接（按功能分组，避免大段重复代码）----

    // 数字按钮 btnNum0~btnNum9 共用一个槽，用 sender() 区分来源
    for (int d = 0; d <= 9; ++d) {
        QPushButton *btn = findChild<QPushButton *>(QString("btnNum%1").arg(d));
        if (btn)
            connect(btn, &QPushButton::clicked, this, &MainWindow::btnNumClicked);
    }

    // 二元操作符 + - × ÷ 共用一个槽
    for (const char *name : {"btnPlus", "btnMinus", "btnMultiple", "btnDivide"}) {
        QPushButton *btn = findChild<QPushButton *>(name);
        if (btn)
            connect(btn, &QPushButton::clicked, this, &MainWindow::btnBinaryOperatorClicked);
    }

    // 一元操作符 % 1/x x² √x 共用一个槽
    for (const char *name : {"btnPercentage", "btnInverse", "btnSquare", "btnSqrt"}) {
        QPushButton *btn = findChild<QPushButton *>(name);
        if (btn)
            connect(btn, &QPushButton::clicked, this, &MainWindow::btnUnaryOperatorClicked);
    }

    // 其余功能键一一对应
    connect(ui->btnEqual,  &QPushButton::clicked, this, &MainWindow::btnEqualClicked);
    connect(ui->btnClear,  &QPushButton::clicked, this, &MainWindow::btnClearClicked);
    connect(ui->btnCE,     &QPushButton::clicked, this, &MainWindow::btnCEClicked);
    connect(ui->btnDel,    &QPushButton::clicked, this, &MainWindow::btnDelClicked);
    connect(ui->btnDot,    &QPushButton::clicked, this, &MainWindow::btnDotClicked);
    connect(ui->btnSign,   &QPushButton::clicked, this, &MainWindow::btnSignClicked);

    statusBar()->showMessage("就绪");
    ui->display->setText("0");
}

MainWindow::~MainWindow()
{
    delete ui;
}

// 计算逻辑将在下一提交中实现
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
