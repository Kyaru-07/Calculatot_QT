#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

QT_BEGIN_NAMESPACE
namespace Ui {
class MainWindow;
}
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow() override;

private slots:
           void onButtonClicked();

private:
    Ui::MainWindow *ui;

    void pressKey(const QString &buttonId);
    void inputDigit(const QString &digit);
    void inputDot();
    void inputBackSpace();
    void clearAll();
    void clearEntry();

    void setOperator(const QString &op);
    void evaluate();
    bool applyPending(double rhs);
    void showError(const QString &message);

    double currentValue() const;
    QString formatNumber(double value) const;

    void updateDisplay();

    QString m_current;
    double m_lhs=0.0;
    QString m_op;
    bool m_startNew=true;
    bool m_error=false;

    static constexpr int KMaxDigits=16;
};

#endif // MAINWINDOW_H
