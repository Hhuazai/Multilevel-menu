#include "ParseXML.h"
#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    ParseXML w;
    w.show();

    return a.exec();
}
