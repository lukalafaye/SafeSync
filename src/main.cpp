#include "mainwindow.h"
#include <QApplication>

// Fonction principale du projet
int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    MainWindow w;
    w.show();

    return a.exec();
}
