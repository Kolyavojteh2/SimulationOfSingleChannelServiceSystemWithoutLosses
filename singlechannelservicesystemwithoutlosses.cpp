#include "singlechannelservicesystemwithoutlosses.h"

SingleChannelServiceSystemWithoutLosses::SingleChannelServiceSystemWithoutLosses(QWidget *parent)
    : QMainWindow(parent)
{
    //
    // Input data
    //

    QDoubleValidator* p_validator = new QDoubleValidator;
    p_validator->setLocale(QLocale::English);

    QLabel* p_lbl_lambda = new QLabel(tr("lambda: "));
    QLineEdit* p_lineEdit_lambda = new QLineEdit;
    p_inputField_lambda = p_lineEdit_lambda;
    p_lineEdit_lambda->setValidator(p_validator);

    QLabel* p_lbl_st = new QLabel(tr("st = (1 / u): "));
    QLineEdit* p_lineEdit_st = new QLineEdit;
    p_inputField_st = p_lineEdit_st;
    p_lineEdit_st->setValidator(p_validator);

    QPushButton* p_button_calculate = new QPushButton(tr("Обчислити!"));
    connect(p_button_calculate, SIGNAL(clicked()),
            this, SLOT(slot_simulateSystem()));
    connect(p_lineEdit_lambda, SIGNAL(returnPressed()),
            p_button_calculate, SLOT(click()));
    connect(p_lineEdit_st, SIGNAL(returnPressed()),
            p_button_calculate, SLOT(click()));

    QPushButton* p_button_compareIndicators = new QPushButton(tr("Додати на графіки порівняння характеристик"));
    connect(p_button_compareIndicators, SIGNAL(clicked()),
            this, SLOT(slot_addToCompare()));

    QGridLayout* p_inputGrid = new QGridLayout;
    p_inputGrid->addWidget(p_lbl_lambda, 0, 0);
    p_inputGrid->addWidget(p_lineEdit_lambda, 0, 1);

    p_inputGrid->addWidget(p_lbl_st, 1, 0);
    p_inputGrid->addWidget(p_lineEdit_st, 1, 1);

    p_inputGrid->addWidget(p_button_calculate, 2, 0);

    QGroupBox* p_inputGroup = new QGroupBox(tr("Вхідні дані:"));
    p_inputGroup->setLayout(p_inputGrid);

    //
    // Output data
    //

    p_outputField = new QPlainTextEdit;
    p_outputField->setReadOnly(true);

    QGridLayout* p_outputGrid = new QGridLayout;
    p_outputGrid->addWidget(p_outputField);

    QGroupBox* p_outputGroup = new QGroupBox(tr("Результат розрахунку моделювання одноканальної системи без втрат:"));
    p_outputGroup->setLayout(p_outputGrid);

    QVBoxLayout* p_windowBox = new QVBoxLayout;
    p_windowBox->addWidget(p_inputGroup);
    p_windowBox->addWidget(p_outputGroup);

    this->setCentralWidget(new QWidget);
    this->centralWidget()->setLayout(p_windowBox);
}

SingleChannelServiceSystemWithoutLosses::PerformanceIndicators SingleChannelServiceSystemWithoutLosses::simulateSystem(const InputData input)
{
    auto calculate_P0 = [=](float c, float p) -> float
    {
        float result = 0.0;

        for (int s = 0; s <= c - 1; s++)
        {
            result += pow(c * p, s) / factorial(s);
        }

        result += pow(c * p, c) / factorial(c) / (1.0 - p);

        result = pow(result, -1);

        return result;
    };

    PerformanceIndicators indicators;
    indicators.L = input.rho / (1.0 - input.rho);
    indicators.Lq = indicators.L - input.rho;
    indicators.W = indicators.L / input.lambda;
    indicators.Wq = indicators.W - input.st;
    indicators.B = 1.0 / (pow(input.st, -1) - input.lambda);
    indicators.P0 = calculate_P0(1, input.rho);
    indicators.q = 1.0 - indicators.P0;

    return indicators;
}

void SingleChannelServiceSystemWithoutLosses::slot_simulateSystem()
{
    float lambda = this->p_inputField_lambda->text().toFloat();
    float st = this->p_inputField_st->text().toFloat();
    float iat = 1.0 / lambda;
    float rho = lambda * st;

    InputData inputData;
    inputData.lambda = lambda;
    inputData.st = st;
    inputData.iat = iat;
    inputData.rho = rho;

    auto indicators = simulateSystem(inputData);

    p_outputField->clear();

    p_outputField->appendPlainText(tr("Вхідні дані:"));
    p_outputField->appendPlainText(QString("\tlambda           \t= %1").arg(inputData.lambda));
    p_outputField->appendPlainText(QString("\tiat (1 / lambda) \t= %1").arg(inputData.iat));
    p_outputField->appendPlainText(QString("\tst  (1 / u)      \t= %1").arg(inputData.st));
    p_outputField->appendPlainText(QString("\trho (lambda / u) \t= %1").arg(inputData.rho));
    p_outputField->appendPlainText("");
    p_outputField->appendPlainText(tr("Показники ефективності:"));
    p_outputField->appendPlainText(QString("\tL  \t\t= %1").arg(indicators.L));
    p_outputField->appendPlainText(QString("\tLq \t\t= %1").arg(indicators.Lq));
    p_outputField->appendPlainText(QString("\tW  \t\t= %1").arg(indicators.W));
    p_outputField->appendPlainText(QString("\tWq \t\t= %1").arg(indicators.Wq));
    p_outputField->appendPlainText(QString("\tB  \t\t= %1").arg(indicators.B));
    p_outputField->appendPlainText(QString("\tP0 \t\t= %1").arg(indicators.P0));
    p_outputField->appendPlainText(QString("\tq  \t\t= %1").arg(indicators.q));
}

void SingleChannelServiceSystemWithoutLosses::slot_addToCompare()
{
    float lambda = this->p_inputField_lambda->text().toFloat();
    float st = this->p_inputField_st->text().toFloat();
    float iat = 1.0 / lambda;
    float rho = lambda * st;

    InputData inputData;
    inputData.lambda = lambda;
    inputData.st = st;
    inputData.iat = iat;
    inputData.rho = rho;

    auto indicators = simulateSystem(inputData);

    /// Доробити адаптовані графіки

    if (!p_compareCharts)
        p_compareCharts = new QWidget;

    QBarSet* p_barset_L = new QBarSet("L");
    QBarSet* p_barset_Lq = new QBarSet("Lq");
    QBarSet* p_barset_W = new QBarSet("W");
    QBarSet* p_barset_Wq = new QBarSet("Wq");
    QBarSet* p_barset_B = new QBarSet("B");
    QBarSet* p_barset_P0 = new QBarSet("P0");
    QBarSet* p_barset_q = new QBarSet("q");

    p_barset_L->append(indicators.L);
    p_barset_Lq->append(indicators.Lq);
    p_barset_W->append(indicators.W);
    p_barset_Wq->append(indicators.Wq);
    p_barset_B->append(indicators.B);
    p_barset_P0->append(indicators.P0);
    p_barset_q->append(indicators.q);




}
