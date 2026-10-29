#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <QPushButton>
#include <QKeyEvent>
#include <QStatusBar>
#include <QFontMetrics>
#include <cmath>

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
    setDisplay("0");
}

MainWindow::~MainWindow()
{
    delete ui;
}

// ======================================================================
// 数字按钮共用槽：btnNum0~btnNum9
// ======================================================================
void MainWindow::btnNumClicked()
{
    QPushButton *btn = qobject_cast<QPushButton *>(sender());
    if (!btn) return;
    int digit = btn->text().toInt();
    if (m_error) btnClearClicked();   // 错误状态下输入数字：先清空再输入
    enterDigit(digit);
    statusBar()->showMessage("输入数字: " + btn->text());
}

// ======================================================================
// 二元操作符 + - × ÷ 共用槽
// 按操作符时：若有待执行运算（连续运算）先算出上一步结果作为新操作数
// ======================================================================
void MainWindow::btnBinaryOperatorClicked()
{
    QPushButton *btn = qobject_cast<QPushButton *>(sender());
    if (!btn || m_error) return;

    QString op = btn->text();
    double currentVal = displayToDouble();

    if (!m_pendingOp.isEmpty() && !m_waitingForNewOp) {
        // 连续运算：先计算上一步 1+2+3 → 按+时先算 1+2
        double result = calculate(m_operand1, currentVal, m_pendingOp);
        if (std::isinf(result) || std::isnan(result)) {
            enterErrorState("除数不能为0");
            return;
        }
        m_operand1 = result;
        setDisplay(doubleToString(result));
    } else {
        // 首次按操作符，或按完操作符直接换操作符：覆盖待执行操作符
        m_operand1 = currentVal;
    }

    m_pendingOp = op;
    m_waitingForNewOp = true;   // 等待输入第二个操作数
    statusBar()->showMessage("操作符: " + op);
}

// ======================================================================
// 一元操作符 % 1/x x² √x 共用槽
// ======================================================================
void MainWindow::btnUnaryOperatorClicked()
{
    QPushButton *btn = qobject_cast<QPushButton *>(sender());
    if (!btn || m_error) return;

    double val = displayToDouble();
    QString op = btn->text();
    double result = val;

    if (op == "%") {
        result = val / 100.0;
    } else if (op == "1/x") {
        if (val == 0.0) { enterErrorState("除数不能为0"); return; }
        result = 1.0 / val;
    } else if (op == "x²") {
        result = val * val;
    } else if (op == "√x") {
        if (val < 0.0) { enterErrorState("不能对负数开根号"); return; }
        result = std::sqrt(val);
    }

    setDisplay(doubleToString(result));
    m_waitingForNewOp = true;   // 结果作为新的操作数
    statusBar()->showMessage("一元运算: " + op);
}

// ======================================================================
// 等于：执行待运算，之后回到“等待新输入”状态
// ======================================================================
void MainWindow::btnEqualClicked()
{
    if (m_error || m_pendingOp.isEmpty()) return;

    double operand2 = displayToDouble();
    double result = calculate(m_operand1, operand2, m_pendingOp);

    if (std::isinf(result) || std::isnan(result)) {
        enterErrorState("除数不能为0");
        return;
    }

    setDisplay(doubleToString(result));
    m_pendingOp = "";          // 结束本次运算
    m_waitingForNewOp = true;  // 计算完成后继续输入数字 → 开始新计算
    statusBar()->showMessage("= " + doubleToString(result));
}

// ======================================================================
// 全部清除 C：状态完全复位
// ======================================================================
void MainWindow::btnClearClicked()
{
    m_operand1 = 0.0;
    m_pendingOp = "";
    m_waitingForNewOp = true;
    m_error = false;
    setDisplay("0");
    statusBar()->showMessage("清除");
}

// ======================================================================
// 清除当前输入 CE：只清显示屏，不清已按下的操作符
// ======================================================================
void MainWindow::btnCEClicked()
{
    if (m_error) { btnClearClicked(); return; }
    m_displayText = "0";
    m_waitingForNewOp = true;   // 下一次数字输入从头开始
    setDisplay("0");
    statusBar()->showMessage("清除当前输入");
}

// ======================================================================
// 退格：删除最后一位（仅输入过程中有效）
// ======================================================================
void MainWindow::btnDelClicked()
{
    if (m_error || m_waitingForNewOp) return;
    if (m_displayText.length() > 1) {
        m_displayText.chop(1);
    } else {
        m_displayText = "0";
    }
    setDisplay(m_displayText);
}

// ======================================================================
// 小数点：调用共用的 enterDot()
// ======================================================================
void MainWindow::btnDotClicked()
{
    if (m_error) btnClearClicked();
    enterDot();
}

// ======================================================================
// 正负号 ±
// ======================================================================
void MainWindow::btnSignClicked()
{
    if (m_error) return;
    if (m_displayText.startsWith("-")) {
        m_displayText = m_displayText.mid(1);
    } else if (m_displayText != "0") {
        m_displayText = "-" + m_displayText;
    }
    setDisplay(m_displayText);
}

// ======================================================================
// 键盘事件：数字/小数点走与鼠标共用的 enterDigit/enterDot，
// 操作符/等号/退格/清除直接触发对应按钮 click()，保证两套输入
// 复用同一套处理逻辑、行为完全一致
// ======================================================================
void MainWindow::keyPressEvent(QKeyEvent *event)
{
    int key = event->key();

    // 数字 0~9（主键盘与小键盘，用 event->text() 统一判断）
    const QString text = event->text();
    if (text.length() == 1 && text.at(0).isDigit()) {
        if (m_error) btnClearClicked();
        enterDigit(text.at(0).digitValue());
        return;
    }

    switch (key) {
    case Qt::Key_Period:
    case Qt::Key_Comma:
        if (m_error) btnClearClicked();
        enterDot();
        break;
    case Qt::Key_Plus:      ui->btnPlus->click();     break;
    case Qt::Key_Minus:     ui->btnMinus->click();    break;
    case Qt::Key_Asterisk:  ui->btnMultiple->click(); break;
    case Qt::Key_Slash:     ui->btnDivide->click();   break;
    case Qt::Key_Enter:
    case Qt::Key_Return:
    case Qt::Key_Equal:     ui->btnEqual->click();    break;
    case Qt::Key_Backspace: ui->btnDel->click();      break;
    case Qt::Key_Escape:    ui->btnClear->click();    break;
    case Qt::Key_Delete:    ui->btnCE->click();       break;
    case Qt::Key_Percent:   ui->btnPercentage->click(); break;
    default:
        QMainWindow::keyPressEvent(event);
    }
}

// ======================================================================
// 内部辅助函数
// ======================================================================

// 更新显示屏，并根据文本长度自适应字号，避免错误提示被截断
void MainWindow::setDisplay(const QString &text)
{
    m_displayText = text;
    ui->display->setText(text);

    QFont font = ui->display->font();
    int baseSize = 26;   // 默认字号（pt）
    font.setPointSize(baseSize);
    QFontMetrics fm(font);
    while (fm.horizontalAdvance(text) > ui->display->width() - 20 && baseSize > 12) {
        baseSize -= 2;
        font.setPointSize(baseSize);
        fm = QFontMetrics(font);
    }
    ui->display->setFont(font);
}

double MainWindow::displayToDouble() const
{
    return m_displayText.toDouble();
}

QString MainWindow::doubleToString(double value) const
{
    if (std::isinf(value) || std::isnan(value)) return "错误";
    return QString::number(value, 'g', 12);
}

// 进入错误状态：显示提示，等待用户按数字/C 重新开始
void MainWindow::enterErrorState(const QString &msg)
{
    m_error = true;
    m_pendingOp = "";
    setDisplay(msg);
    statusBar()->showMessage("错误: " + msg);
}

// 输入一位数字（鼠标与键盘共用）
void MainWindow::enterDigit(int digit)
{
    if (m_waitingForNewOp) {
        // 新操作数的第一位：替换显示（计算结果后继续输入即重新开始）
        m_displayText = QString::number(digit);
        m_waitingForNewOp = false;
    } else {
        if (m_displayText == "0")
            m_displayText = QString::number(digit);   // 0 开头则替换
        else
            m_displayText += QString::number(digit);
    }
    setDisplay(m_displayText);
}

// 输入小数点（鼠标与键盘共用）：防止同一操作数重复输入小数点
void MainWindow::enterDot()
{
    if (m_waitingForNewOp) {
        m_displayText = "0.";       // 新操作数以 0. 开头
        m_waitingForNewOp = false;
    } else {
        if (!m_displayText.contains('.'))
            m_displayText += '.';   // 已有小数点则忽略本次输入
    }
    setDisplay(m_displayText);
}

// 执行二元运算
double MainWindow::calculate(double a, double b, const QString &op) const
{
    if (op == "+") return a + b;
    if (op == "-") return a - b;
    if (op == "×") return a * b;
    if (op == "÷") return (b == 0.0) ? NAN : a / b;   // 除零返回 NaN，由调用方转错误状态
    return b;
}
