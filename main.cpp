#include <QCoreApplication>
#include "app/ProcessorApp.hpp"

int main(int argc, char *argv[])
{
    QCoreApplication qtApp(argc, argv);

    ProcessorApp app;
    app.run();

    return qtApp.exec();
}