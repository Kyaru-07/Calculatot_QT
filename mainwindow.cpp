#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    const QList<QPushButton *>buttons=findChildren<QPushButton *>();
    for(QPushButton *btn:buttons){
        connect(btn,&QPushButton::clicked,this,&MainWindow::onButtonClicked);
    }
    updateDisplay();
}

void MainWindow::onButtonClicked(){
    if(auto *btn=qobject_cast<QPushButton *>(sender()))
        pressKey(btn->objectName());
}

//按键分流
void MainWindow::pressKey(const QString &id){
    if(id.startsWith(QLatin1String("Btn"))){
        inputDigit(id.right(1));
        return;
    }

    if(id==QLatin1String("btnDot")){
        inputDot();
        return;
    }
    if(id==QLatin1String("btnBackSpace")){
        inputBackSpace();
        return;
    }
    if(id==QLatin1String("btnClear")){
        clearAll();
        return;
    }
    if(id==QLatin1String("btnClearEntry")){
        clearEntry();
        return;
    }
    if(id==QLatin1String("btnAdd")){
        setOperator(QStringLiteral("+"));
        return;
    }
    if(id==QLatin1String("btnSub")){
        setOperator(QStringLiteral("-"));
        return;
    }
    if(id==QLatin1String("btnMulti")){
        setOperator(QStringLiteral("*"));
        return;
    }
    if(id==QLatin1String("btnDivide")){
        setOperator(QStringLiteral("/"));
        return;
    }
    if(id==QLatin1String("btnEquals")){
        evaluate();
        return;
    }
}

//刷新显示
void MainWindow::updateDisplay(){
    ui->lineEditDisplay->setText(m_current.isEmpty()?QStringLiteral("0"):m_current);
}

//读取当前值
double MainWindow::currentValue() const{
    return m_current.toDouble();
}

QString MainWindow::formatNumber(double value) const{
    if(!qIsFinite(value))
        return QStringLiteral("错误");
    QString text=QString::number(value,'g',12);

    if(text==QLatin1String("-0"))
        text=QStringLiteral("0");
    return text;
}

//处理数字输入
void MainWindow::inputDigit(const QString &digit){
    if(m_error){
        clearAll();
    }
    if(m_startNew){
        m_current.clear();
        m_startNew=false;
    }
    if(m_current==QLatin1String("0"))
        m_current.clear();
    if(m_current.size()>=KMaxDigits)
        return;

    m_current+=digit;
    updateDisplay();
}

//处理小数点输入
void MainWindow::inputDot(){
    if(m_error){
        clearAll();
    }
    if(m_startNew){
        m_current=QStringLiteral("0");
        m_startNew=false;
    }
    if(m_current.contains(QLatin1Char('.')))
        return;
    if(m_current.isEmpty())
        m_current=QStringLiteral("0");
    if(m_current.size()>=KMaxDigits){
        return;
    }
    m_current+=QLatin1Char('.');
    updateDisplay();
}

//处理退格输入
void MainWindow::inputBackSpace(){
    if(m_error){
        clearAll();
        return;
    }
    if(m_startNew){
        return;
    }
    m_current.chop(1);
    if(m_current.isEmpty()||m_current==QLatin1String("-"))
        m_current=QStringLiteral("0");

    updateDisplay();
}

//处理清空
void MainWindow::clearAll(){
    m_current.clear();
    m_lhs=0.0;
    m_op.clear();
    m_startNew=true;
    m_error=false;
    updateDisplay();
}

//处理清空输入
void MainWindow::clearEntry(){
    if(m_error){
        clearAll();
        return;
    }
    m_current.clear();
    m_startNew=true;
    updateDisplay();
}

//处理运算符输入
void MainWindow::setOperator(const QString &op){
    if(m_error){
        return;
    }
    if(!m_op.isEmpty()&&!m_startNew){
        if(!applyPending(currentValue()))
            return;
    }else{
        m_lhs=currentValue();
    }
    m_op=op;
    m_startNew=true;
    updateDisplay();
}

bool MainWindow::applyPending(double rhs){
    double result=0.0;

    if(m_op==QLatin1String("+")){
        result=m_lhs+rhs;
    }else if(m_op==QLatin1String("-")){
        result=m_lhs-rhs;
    }else if(m_op==QLatin1String("*")){
        result=m_lhs*rhs;
    }else if(m_op==QLatin1String("/")){
        if(qFuzzyIsNull(rhs)){
            showError(QStringLiteral("除数不能为0"));
            return false;
        }
        result=m_lhs/rhs;
    }else{
        return false;
    }

    m_lhs=result;
    m_current=formatNumber(result);
    return true;
}

//处理=输入
void MainWindow::evaluate(){
    if(m_error||m_op.isEmpty()){
        return;
    }

    const double rhs=m_startNew?m_lhs:currentValue();

    if(!applyPending(rhs)){
        return;
    }

    m_op.clear();
    m_startNew=true;
    updateDisplay();
}

void MainWindow::showError(const QString &message){
    m_error=true;
    m_current=message;
    m_op.clear();
    m_startNew=true;
    updateDisplay();
}

MainWindow::~MainWindow()
{
    delete ui;
}
