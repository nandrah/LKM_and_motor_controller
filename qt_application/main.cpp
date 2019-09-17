#include "finalproject.h"
#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    FinalProject w;
    w.show();

    return a.exec();
}
