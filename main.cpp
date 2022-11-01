#include "singlechannelservicesystemwithoutlosses.h"

#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    SingleChannelServiceSystemWithoutLosses w;
    w.resize(200, 500);
    w.show();
    return a.exec();
}
