#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

class QLineEdit;

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

protected:
    // 键盘事件：与鼠标按键复用同一套处理逻辑
    void keyPressEvent(QKeyEvent *event) override;

private slots:
    // 数字按钮 btnNum0~btnNum9 共用
    void btnNumClicked();
    // 二元操作符 btnPlus/btnMinus/btnMultiple/btnDivide 共用
    void btnBinaryOperatorClicked();
    // 一元操作符 btnPercentage/btnInverse/btnSquare/btnSqrt 共用
    void btnUnaryOperatorClicked();
    // 等于 btnEqual
    void btnEqualClicked();
    // 全部清除 btnClear（C）
    void btnClearClicked();
    // 清除当前输入 btnCE
    void btnCEClicked();
    // 退格 btnDel
    void btnDelClicked();
    // 小数点 btnDot
    void btnDotClicked();
    // 正负号 btnSign
    void btnSignClicked();

private:
    // ---- 状态变量 ----
    double m_operand1 = 0.0;   // 第一个操作数（或连续运算时的累计结果）
    QString m_pendingOp;       // 待执行的二元操作符，空表示尚无操作符
    bool m_waitingForNewOp = true; // 下一次输入是否开始新操作数
    bool m_error = false;      // 错误状态（除零、负数开方等）
    QString m_displayText;     // 当前显示文本

    // ---- 控件指针（由 UI 设计器生成的 ui 对象管理）----
    Ui::MainWindow *ui;

    // ---- 辅助函数（鼠标与键盘共用的核心输入逻辑）----
    void setDisplay(const QString &text);   // 更新显示并自适应字号
    double displayToDouble() const;
    QString doubleToString(double value) const;
    void enterErrorState(const QString &msg);
    void enterDigit(int digit);             // 输入一位数字
    void enterDot();                        // 输入小数点（防重复）
    double calculate(double a, double b, const QString &op) const;
};

#endif // MAINWINDOW_H
