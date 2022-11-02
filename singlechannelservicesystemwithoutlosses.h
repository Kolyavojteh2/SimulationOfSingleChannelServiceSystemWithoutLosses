#ifndef SINGLECHANNELSERVICESYSTEMWITHOUTLOSSES_H
#define SINGLECHANNELSERVICESYSTEMWITHOUTLOSSES_H

#include <QMainWindow>
#include <QtMath>

#include <QPlainTextEdit>
#include <QGroupBox>
#include <QGridLayout>
#include <QVBoxLayout>
#include <QLabel>
#include <QPushButton>
#include <QLineEdit>

#include <QValidator>

#include <QtCharts>

#include "AdditionalMath.h"

class SingleChannelServiceSystemWithoutLosses : public QMainWindow
{
    Q_OBJECT

private:
    QLineEdit* p_inputField_lambda;
    QLineEdit* p_inputField_st;

    QPlainTextEdit* p_outputField;

    QWidget* p_compareWidget = nullptr;
    QChart* p_compareChart_L = nullptr;
    QChart* p_compareChart_Lq = nullptr;
    QChart* p_compareChart_W = nullptr;
    QChart* p_compareChart_Wq = nullptr;
    QChart* p_compareChart_B = nullptr;
    QChart* p_compareChart_P0 = nullptr;
    QChart* p_compareChart_q = nullptr;

    struct InputData
    {
        float lambda, st, iat, rho;
    };

    struct PerformanceIndicators
    {
        float L, Lq, W, Wq, B, P0, q;
    };

public:
    SingleChannelServiceSystemWithoutLosses(QWidget *parent = nullptr);

    PerformanceIndicators simulateSystem(const InputData input);

protected:
    void closeEvent(QCloseEvent *event) override;

public slots:
    void slot_simulateSystem();
    void slot_addToCompare();
};
#endif // SINGLECHANNELSERVICESYSTEMWITHOUTLOSSES_H
