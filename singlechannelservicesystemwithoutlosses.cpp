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
    connect(p_lineEdit_lambda, SIGNAL(returnPressed()),
            p_button_compareIndicators, SLOT(click()));
    connect(p_lineEdit_st, SIGNAL(returnPressed()),
            p_button_compareIndicators, SLOT(click()));

    QGridLayout* p_inputGrid = new QGridLayout;
    p_inputGrid->addWidget(p_lbl_lambda, 0, 0);
    p_inputGrid->addWidget(p_lineEdit_lambda, 0, 1);

    p_inputGrid->addWidget(p_lbl_st, 1, 0);
    p_inputGrid->addWidget(p_lineEdit_st, 1, 1);

    p_inputGrid->addWidget(p_button_calculate, 2, 0);
    p_inputGrid->addWidget(p_button_compareIndicators, 2, 1);

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
    this->setWindowTitle(tr("Моделювання одноканальної системи без втрат"));
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

void SingleChannelServiceSystemWithoutLosses::closeEvent(QCloseEvent* event)
{
    Q_UNUSED(event)

    QApplication::exit(0);
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

    if (!p_compareWidget)
    {
        p_compareWidget = new QWidget;

        auto initChart = [](const QString tittle) -> QChart*
        {
            QChart* chart = new QChart;
            chart->setTitle(tittle);
            chart->addSeries(new QBarSeries);
            chart->setAnimationOptions(QChart::SeriesAnimations);

            QStringList categories;
            categories << "";
            QBarCategoryAxis *axisX = new QBarCategoryAxis();
            axisX->append(categories);
            chart->addAxis(axisX, Qt::AlignBottom);
            dynamic_cast<QBarSeries*>(chart->series().takeFirst())->attachAxis(axisX);

            QValueAxis *axisY = new QValueAxis();
            axisY->setRange(0, 1);
            chart->addAxis(axisY, Qt::AlignLeft);
            dynamic_cast<QBarSeries*>(chart->series().takeFirst())->attachAxis(axisY);

            chart->legend()->setVisible(true);
            chart->legend()->setAlignment(Qt::AlignmentFlag::AlignRight);

            return chart;
        };

        p_compareChart_L = initChart("L");
        p_compareChart_Lq = initChart("Lq");
        p_compareChart_W = initChart("W");
        p_compareChart_Wq = initChart("Wq");
        p_compareChart_B = initChart("B");
        p_compareChart_P0 = initChart("P0");
        p_compareChart_q = initChart("q");

        auto initChartView = [](QChart* chart) -> QChartView*
        {
            QChartView* p_view = new QChartView(chart);
            p_view->setRenderHint(QPainter::Antialiasing);

            return p_view;
        };

        QChartView* p_view_L = initChartView(p_compareChart_L);
        QChartView* p_view_Lq = initChartView(p_compareChart_Lq);
        QChartView* p_view_W = initChartView(p_compareChart_W);
        QChartView* p_view_Wq = initChartView(p_compareChart_Wq);
        QChartView* p_view_B = initChartView(p_compareChart_B);
        QChartView* p_view_P0 = initChartView(p_compareChart_P0);
        QChartView* p_view_q = initChartView(p_compareChart_q);

        QGridLayout* p_gridCharts = new QGridLayout;
        p_gridCharts->addWidget(p_view_L, 0, 0);
        p_gridCharts->addWidget(p_view_Lq, 0, 1);
        p_gridCharts->addWidget(p_view_W, 0, 2);
        p_gridCharts->addWidget(p_view_Wq, 1, 0);
        p_gridCharts->addWidget(p_view_B, 1, 1);
        p_gridCharts->addWidget(p_view_P0, 1, 2);
        p_gridCharts->addWidget(p_view_q, 2, 0);

        p_compareWidget->setLayout(p_gridCharts);
        p_compareWidget->setWindowState(Qt::WindowState::WindowMaximized);
        p_compareWidget->setWindowTitle(tr("Графіки порівняння"));
        p_compareWidget->show();
    }

    auto findMaxFromChart = [](QChart* chart) -> float
    {
        auto sets = dynamic_cast<QBarSeries*>(chart->series().takeFirst())->barSets();

        float max_value = 0.0;
        for (auto &current_set : sets)
        {
            for (int i = 0; i < current_set->count(); i++)
            {
                if (max_value < current_set->at(i))
                    max_value = current_set->at(i);
            }
        }

        return max_value;
    };

    auto findMinFromChart = [](QChart* chart) -> float
    {
        auto sets = dynamic_cast<QBarSeries*>(chart->series().takeFirst())->barSets();

        float min_value = 0.0;
        for (auto &current_set : sets)
        {
            for (int i = 0; i < current_set->count(); i++)
            {
                if (min_value > current_set->at(i))
                    min_value = current_set->at(i);
            }
        }

        return min_value;
    };

    auto fillChart = [&findMaxFromChart, &findMinFromChart](QChart* chart, InputData input, float value) {
        QBarSet* p_set = new QBarSet(QString("lambda=%1 (1/u)=%2").arg(input.lambda).arg(input.st));
        p_set->append(value);

        dynamic_cast<QBarSeries*>(chart->series().takeFirst())->append(p_set);

        int maxRangeValue = ceil(findMaxFromChart(chart));
        chart->axes(Qt::Vertical).takeFirst()->setMax(maxRangeValue);

        int minRangeValue = findMinFromChart(chart);
        chart->axes(Qt::Vertical).takeFirst()->setMin(minRangeValue);
    };

    fillChart(p_compareChart_L, inputData, indicators.L);
    fillChart(p_compareChart_Lq, inputData, indicators.Lq);
    fillChart(p_compareChart_W, inputData, indicators.W);
    fillChart(p_compareChart_Wq, inputData, indicators.Wq);
    fillChart(p_compareChart_B, inputData, indicators.B);
    fillChart(p_compareChart_P0, inputData, indicators.P0);
    fillChart(p_compareChart_q, inputData, indicators.q);

    p_compareWidget->show();
}
