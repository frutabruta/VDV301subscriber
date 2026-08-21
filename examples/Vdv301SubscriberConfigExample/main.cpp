#include "mainwindow.h"

#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);


    QCommandLineParser qCommandLineParser;
    qCommandLineParser.addOption(QCommandLineOption("config", "Input file path", "file"));
    qCommandLineParser.process(a.arguments());

    QString cesta="";
    if(!qCommandLineParser.value("config").isEmpty())
    {
        cesta=qCommandLineParser.value("config");
    }
    else
    {
        cesta=QCoreApplication::applicationDirPath()+"/settings.ini";
    }

    MainWindow w(cesta,nullptr);
    w.show();
    return a.exec();
}
